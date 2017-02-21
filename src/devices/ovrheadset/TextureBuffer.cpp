/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "TextureBuffer.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include "OVR_CAPI_GL.h"

#if OVR_PRODUCT_VERSION == 0 && OVR_MAJOR_VERSION <= 5
TextureBuffer::TextureBuffer(int w, int h, int eye) :
#elif OVR_PRODUCT_VERSION == 0 && OVR_MAJOR_VERSION <= 7
TextureBuffer::TextureBuffer(int w, int h, int eye, ovrHmd hmd) :
        hmd(hmd),
        textureSet(nullptr),
#elif OVR_PRODUCT_VERSION == 0 && OVR_MAJOR_VERSION <= 8
TextureBuffer::TextureBuffer(int w, int h, int eye, ovrSession session) :
        session(session),
        textureSet(nullptr),
#else
TextureBuffer::TextureBuffer(int w, int h, int eye, ovrSession session) :
        session(session),
        textureSwapChain(0),
#endif
        width(w),
        height(h),
        // see http://stackoverflow.com/questions/27294882/glteximage2d-fails-with-error-1282-using-pbo-bad-texture-resolution
        padding((4 - (w*3) % 4) % 4),
        rowSize(w*3 + padding),
        bufferSize(rowSize * h),
        pboIndex(0),
        pboNextIndex(1),
        ptr(nullptr),
        dataReady(true),
        missingFrames(0),
        imageWidth(0),
        imageHeight(0),
        eye(eye)
{
    YARP_UNUSED(eye);
    yTrace();

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
    padding = (4 - (w*3) % 4) % 4,
    rowSize = w*3 + padding,
    bufferSize = rowSize * h,

    pboIndex = 0;
    pboNextIndex = 1;

    createTextureAndBuffers();
}


static void checkGlError(const char* file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        switch(error) {
        case GL_INVALID_ENUM:
            yError() << "OpenGL Error GL_INVALID_ENUM: GLenum argument out of range";
            break;
        case GL_INVALID_VALUE:
            yError() << "OpenGL Error GL_INVALID_VALUE: Numeric argument out of range";
            break;
        case GL_INVALID_OPERATION:
            yError() << "OpenGL Error GL_INVALID_OPERATION: Operation illegal in current state";
            break;
        case GL_STACK_OVERFLOW:
            yError() << "OpenGL Error GL_STACK_OVERFLOW: Command would cause a stack overflow";
            break;
        case GL_OUT_OF_MEMORY:
            yError() << "OpenGL Error GL_OUT_OF_MEMORY: Not enough memory left to execute command";
            break;
        default:
            yError() << "OpenGL Error " << error;
            break;
        }
    }
    yAssert(error == 0);
}

#define checkGlErrorMacro checkGlError(__FILE__, __LINE__)


void TextureBuffer::update()
{
    mutex.lock();

    if (dataReady) {
        dataReady = false;

        // "pboIndex" is used to copy pixels from a PBO to a texture object
        // "pboNextIndex" is used to update pixels in the other PBO
        pboIndex = (pboIndex + 1) % 2;
        pboNextIndex = (pboIndex + 1) % 2;

        // bind the texture and PBO
        glBindTexture(GL_TEXTURE_2D, texId);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[pboIndex]);

        if (ptr) {
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
            ptr = nullptr;
        }

        // copy pixels from PBO to texture object
        // Use offset instead of ponter.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);

        // bind PBO to update texture source
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[pboNextIndex]);

        // Note that glMapBufferARB() causes sync issue.
        // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
        // for GPU to finish its job. To avoid waiting (stall), you can call
        // first glBufferDataARB() with NULL pointer before glMapBufferARB().
        // If you do that, the previous data in PBO will be discarded and
        // glMapBufferARB() returns a new allocated pointer immediately
        // even if GPU is still working with the previous data.
        glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, 0, GL_STREAM_DRAW);

        // map the buffer object into client's memory
        ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        // The buffer will be filled by the InputCallback, and then
        // unmapped on next update() call, therefore there is nothing
        // else to do here.

        // it is good idea to release PBOs with ID 0 after use.
        // Once bound with 0, all pixel operations behave normal ways.
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    } else {
        ++missingFrames;
    }

    mutex.unlock();
}


void TextureBuffer::createTextureAndBuffers()
{
    yTrace();

#if OVR_PRODUCT_VERSION == 0 && OVR_MAJOR_VERSION <= 5
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
#else
# if OVR_PRODUCT_VERSION == 0 && OVR_MAJOR_VERSION <= 6
    ovrHmd_CreateSwapTextureSetGL(hmd, GL_RGBA, width, height, &textureSet);
# elif OVR_PRODUCT_VERSION == 0 && OVR_MAJOR_VERSION <= 7
    ovr_CreateSwapTextureSetGL(hmd, GL_RGBA, width, height, &textureSet);
# elif OVR_PRODUCT_VERSION == 0 && OVR_MAJOR_VERSION <= 8
    ovr_CreateSwapTextureSetGL(session, GL_RGBA, width, height, &textureSet);
# else
    // FIXME 0.8->1.3


    ovrTextureSwapChainDesc desc = {};
    desc.Type = ovrTexture_2D;
    desc.ArraySize = 1;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.StaticImage = ovrFalse;

    if (ovr_CreateTextureSwapChainGL(session, &desc, &textureSwapChain) == ovrSuccess) {
        // FIXME 0.8->1.3: Check error?
    }
# endif

# if OVR_PRODUCT_VERSION == 0 && OVR_MAJOR_VERSION <= 8
    for (int i = 0; i < textureSet->TextureCount; ++i) {
        ovrGLTexture* tex = (ovrGLTexture*)&textureSet->Textures[i];
        glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);
# else
    int count = 0;
    ovr_GetTextureSwapChainLength(session, textureSwapChain, &count);
    for (int i = 0; i < count; ++i) {
        unsigned int texId;
        ovr_GetTextureSwapChainBufferGL(session, textureSwapChain, 0, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
# endif
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // FIXME 0.5->0.6
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }
#endif
    checkGlErrorMacro;

    glGenBuffers(2, pboIds);
    for (int i = 0; i < 2; ++i) {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, 0, GL_STREAM_DRAW);
        glClear(GL_COLOR_BUFFER_BIT);
    }

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

    glDeleteBuffers(2, pboIds);
    pboIds[0] = 0;
    pboIds[1] = 0;

    glDeleteTextures(1, &texId);
    texId = 0;

    pboIndex = 0;
    pboNextIndex = 1;

    mutex.unlock();
}
