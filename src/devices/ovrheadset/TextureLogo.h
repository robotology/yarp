/*
 * Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OVRHEADSET_TEXTURELOGO_H
#define YARP_OVRHEADSET_TEXTURELOGO_H

#include <GL/glew.h>
#include <OVR_Math.h>
#include <yarp/os/Mutex.h>


class TextureLogo
{
public:
    struct Image {
        unsigned int width;
        unsigned int height;
        unsigned int bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
        const char *pixel_data;
    };

    TextureLogo(ovrSession session);
    ~TextureLogo();

    void update();

    ovrSession session;
    ovrTextureSwapChain textureSwapChain;

    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_pixel;
    unsigned int padding;
    unsigned int rowSize;
    unsigned int bufferSize;

    GLuint chainTexId;
    GLuint texId;

    GLubyte* ptr;

private:
    void createTexture();
    void deleteTexture();
};

class TextureCrosshairs
{
public:
    TextureCrosshairs(ovrSession session);
    ~TextureCrosshairs();

    void update();

    ovrSession session;
    ovrTextureSwapChain textureSwapChain;

    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_pixel;
    unsigned int padding;
    unsigned int rowSize;
    unsigned int bufferSize;

    GLuint chainTexId;
    GLuint texId;

    GLubyte* ptr;

private:
    void createTexture();
    void deleteTexture();
};



#endif // YARP_OVRHEADSET_TEXTURELOGO_H
