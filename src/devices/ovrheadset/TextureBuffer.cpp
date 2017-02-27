/*
 * Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "TextureBuffer.h"
#include "GLDebug.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include <OVR_CAPI_GL.h>


TextureBuffer::TextureBuffer(int w, int h, int eye, ovrSession session) :
        session(session),
        textureSwapChain(nullptr),
        textureSwapChainSize(0),
        width(w),
        height(h),
        components(eye == 2 ? 4 : 3),
        // see http://stackoverflow.com/questions/27294882/glteximage2d-fails-with-error-1282-using-pbo-bad-texture-resolution
        padding((4 - (w * components) % 4) % 4),
        rowSize(w * components + padding),
        bufferSize(rowSize * h),
        pboIds(nullptr),
        ptr(nullptr),
        dataReady(true),
        missingFrames(0),
        imageWidth(0),
        imageHeight(0),
        eye(eye)
{
    YARP_UNUSED(eye);
    yTrace();
    checkGlErrorMacro;
    createTextureAndBuffers();
}

TextureBuffer::~TextureBuffer()
{
    yTrace();

    deleteTextureAndBuffers();
}

void TextureBuffer::resize(int w, int h)
{
    yTrace();

    mutex.lock();

    deleteTextureAndBuffers();

    width = w;
    height = h;
    padding = (4 - (w * components) % 4) % 4,
    rowSize = w * components + padding,
    bufferSize = rowSize * h,

    createTextureAndBuffers();
}

void TextureBuffer::update()
{
    mutex.lock();

    if (dataReady) {
        dataReady = false;

        GLuint texId;
        int index;

        ovr_GetTextureSwapChainCurrentIndex(session, textureSwapChain, &index);
        ovr_GetTextureSwapChainBufferGL(session, textureSwapChain, index, &texId);

        // bind the texture and PBO
        glBindTexture(GL_TEXTURE_2D, texId);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);

        if (ptr) {
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
            ptr = nullptr;
        }

        // copy pixels from PBO to texture object
        // Use offset instead of ponter.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, (components == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, 0);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        // Commit current texture and retrieve the next one
        ovr_CommitTextureSwapChain(session, textureSwapChain);
        ovr_GetTextureSwapChainCurrentIndex(session, textureSwapChain, &index);
        ovr_GetTextureSwapChainBufferGL(session, textureSwapChain, index, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        // bind PBO to update texture source
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);

        // Note that glMapBufferARB() causes sync issue.
        // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
        // for GPU to finish its job. To avoid waiting (stall), you can call
        // first glBufferDataARB() with NULL pointer before glMapBufferARB().
        // If you do that, the previous data in PBO will be discarded and
        // glMapBufferARB() returns a new allocated pointer immediately
        // even if GPU is still working with the previous data.
        glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, 0, GL_DYNAMIC_DRAW);

        // map the buffer object into client's memory
        ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        // The buffer will be filled by the InputCallback, and then
        // unmapped on next update() call, therefore there is nothing
        // else to do here.

        // it is good idea to release PBOs with ID 0 after use.
        // Once bound with 0, all pixel operations behave normal ways.
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        checkGlErrorMacro;

    } else {
        ++missingFrames;
    }

    mutex.unlock();
}

void TextureBuffer::createTextureAndBuffers()
{
    yTrace();
    checkGlErrorMacro;

    ovrTextureSwapChainDesc desc = {};
    desc.Type = ovrTexture_2D;
    desc.ArraySize = 1;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.StaticImage = ovrFalse;

    if (!ovr_CreateTextureSwapChainGL(session, &desc, &textureSwapChain) == ovrSuccess) {
        yError() << "Failed to create texture swap chain";
        return;
    }
    checkGlErrorMacro;

    ovr_GetTextureSwapChainLength(session, textureSwapChain, &textureSwapChainSize);

    // Set parameters for each texture in the swap chain
    for (int i = 0; i < textureSwapChainSize; ++i) {
        unsigned int texId;
        ovr_GetTextureSwapChainBufferGL(session, textureSwapChain, i, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Create one buffer for each texture in the swap chain
    pboIds = new GLuint[textureSwapChainSize];
    glGenBuffers(textureSwapChainSize, pboIds);
    for (int i = 0; i < textureSwapChainSize; ++i) {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, 0, GL_DYNAMIC_DRAW);
        // Map and clear the buffer
        ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        memset(ptr, 0, bufferSize);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        ptr = nullptr;
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
    ptr = nullptr;

    // Map the first buffer of the swap chain
    GLuint texId;
    int index;
    ovr_GetTextureSwapChainCurrentIndex(session, textureSwapChain, &index);
    ovr_GetTextureSwapChainBufferGL(session, textureSwapChain, index, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, 0, GL_DYNAMIC_DRAW);
    ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    checkGlErrorMacro;
}

void TextureBuffer::deleteTextureAndBuffers()
{
    yTrace();

    mutex.lock();

    if (ptr) {
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        ptr = nullptr;
    }

    glDeleteBuffers(textureSwapChainSize, pboIds);
    delete pboIds;

    ovr_DestroyTextureSwapChain(session, textureSwapChain);

    mutex.unlock();
}
