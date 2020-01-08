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

#ifndef YARP_OVRHEADSET_TEXTURESTATIC_H
#define YARP_OVRHEADSET_TEXTURESTATIC_H

#include <GL/glew.h>
#include <OVR_CAPI.h>


class TextureStatic
{
public:
    struct Image {
        unsigned int width;
        unsigned int height;
        unsigned int bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
        const char *pixel_data;
    };

    TextureStatic(ovrSession session, const TextureStatic::Image &img);
    ~TextureStatic();

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

#endif // YARP_OVRHEADSET_TEXTURESTATIC_H
