/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_OVRHEADSET_TEXTUREBUFFER_H
#define YARP_OVRHEADSET_TEXTUREBUFFER_H

#include <GL/glew.h>
#include <OVR_Math.h>
#include <mutex>
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

    void resize(size_t w = 0, size_t h = 0);
    void update(const yarp::sig::Image& img);
    void update();
    void lock();
    void unlock();

    ovrSession session;
    ovrTextureSwapChain textureSwapChain;
    int textureSwapChainSize;

    size_t width;
    size_t height;
    unsigned int components;
    unsigned int padding;
    size_t rowSize;
    size_t bufferSize;
    double alpha;

    ovrPosef eyePose;

    GLuint *pboIds;

    GLubyte* ptr;
    bool dataReady;
    std::mutex mutex;

    unsigned int missingFrames;

    size_t imageWidth;
    size_t imageHeight;

private:

    bool initialized;
    bool singleThread;

    void createTextureAndBuffers();
    void deleteTextureAndBuffers();
    int eye;
};

#endif // YARP_OVRHEADSET_TEXTUREBUFFER_H
