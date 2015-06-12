/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OVRHEADSET_INPUTCALLBACK_H
#define YARP_OVRHEADSET_INPUTCALLBACK_H

#include "ImageType.h"

#include <yarp/os/PortReaderBuffer.h>
#include <yarp/sig/Image.h>


class TextureBuffer;

class InputCallback: public yarp::os::TypedReaderCallback<ImageType>
{
public:
    InputCallback(int eye);
    ~InputCallback();

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
