/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
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
    TextureBuffer(int w, int h, int eye);
    ~TextureBuffer();

    void resize(int w = 0, int h = 0);
    void update();

    unsigned int width;
    unsigned int height;
    unsigned int padding;
    unsigned int rowSize;
    unsigned int bufferSize;

    ovrPosef eyePose;

    GLuint texId;
    GLuint pboIds[2];
    unsigned int pboIndex;
    unsigned int pboNextIndex;

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
