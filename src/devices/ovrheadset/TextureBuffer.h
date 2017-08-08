/*
 * Copyright (C) 2015-2017 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OVRHEADSET_TEXTUREBUFFER_H
#define YARP_OVRHEADSET_TEXTUREBUFFER_H

#include <GL/glew.h>
#include <OVR_Math.h>
#include <yarp/os/Mutex.h>
#include <yarp/sig/Image.h>


class TextureBuffer
{
public:

    void fromImage(ovrSession inSession, const yarp::sig::Image& img, double alpha = 1.0);

    //only for single thread contexts
    TextureBuffer();

    //multithread safe
    TextureBuffer(int w, int h, int eye, ovrSession session);

    ~TextureBuffer();

    void resize(int w = 0, int h = 0);
    void update(const yarp::sig::Image& img);
    void update();
    void lock();
    void unlock();

    ovrSession session;
    ovrTextureSwapChain textureSwapChain;
    int textureSwapChainSize;

    unsigned int width;
    unsigned int height;
    unsigned int components;
    unsigned int padding;
    unsigned int rowSize;
    unsigned int bufferSize;
    double       alpha;

    ovrPosef eyePose;

    GLuint *pboIds;

    GLubyte* ptr;
    bool dataReady;
    yarp::os::Mutex mutex;

    unsigned int missingFrames;

    unsigned int imageWidth;
    unsigned int imageHeight;

private:

    bool initialized;
    bool singleThread;

    void createTextureAndBuffers();
    void deleteTextureAndBuffers();
    int eye;
};

#endif // YARP_OVRHEADSET_TEXTUREBUFFER_H
