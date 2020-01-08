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

#ifndef YARP_OVRHEADSET_INPUTCALLBACK_H
#define YARP_OVRHEADSET_INPUTCALLBACK_H

#include "ImageType.h"

#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>


class TextureBuffer;

class InputCallback: public yarp::os::BufferedPort<ImageType>
{
public:
    InputCallback(int eye);
    ~InputCallback();

    using yarp::os::BufferedPort<ImageType>::onRead;
    virtual void onRead(ImageType &img);

    TextureBuffer *eyeRenderTexture;
    int eye;
    int expected;
    unsigned int droppedFrames;
    unsigned int lastImageWidth;
    unsigned int lastImageHeight;

    float rollOffset;
    float pitchOffset;
    float yawOffset;
};

#endif // YARP_OVRHEADSET_INPUTCALLBACK_H
