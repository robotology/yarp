/*
 * Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OVRHEADSET_TEXTUREBUFFER_H
#define YARP_OVRHEADSET_TEXTUREBUFFER_H

#include <GL/glew.h>
#include <OVR_Math.h>
#include <yarp/os/Mutex.h>


class TextureBuffer
{
public:
    TextureBuffer(int w, int h, int eye, ovrSession session);
    ~TextureBuffer();

    void resize(int w = 0, int h = 0);
    void update();

    ovrSession session;
    ovrTextureSwapChain textureSwapChain;
    int textureSwapChainSize;

    unsigned int width;
    unsigned int height;
    unsigned int components;
    unsigned int padding;
    unsigned int rowSize;
    unsigned int bufferSize;

    ovrPosef eyePose;

    GLuint *pboIds;

    GLubyte* ptr;
    bool dataReady;
    yarp::os::Mutex mutex;

    unsigned int missingFrames;

    unsigned int imageWidth;
    unsigned int imageHeight;

private:
    void createTextureAndBuffers();
    void deleteTextureAndBuffers();
    int eye;
};

#endif // YARP_OVRHEADSET_TEXTUREBUFFER_H
