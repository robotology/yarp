/*
 * Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "InputCallback.h"
#include "TextureBuffer.h"

#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

InputCallback::InputCallback(int eye) :
        yarp::os::BufferedPort<ImageType>(),
        eyeRenderTexture(nullptr),
        eye(eye),
        expected(0),
        droppedFrames(0),
        lastImageWidth(0),
        lastImageHeight(0),
        rollOffset(0.0f),
        pitchOffset(0.0f),
        yawOffset(0.0f)
{
    yTrace();
}


InputCallback::~InputCallback()
{
    yTrace();

    if (eyeRenderTexture) {
        delete eyeRenderTexture;
    }
    eyeRenderTexture = nullptr;
}


void InputCallback::onRead(ImageType &img)
{
    int delaycnt = 0;
    while (eyeRenderTexture->dataReady && delaycnt <= 3) {
        yarp::os::SystemClock::delaySystem(0.001);
        ++delaycnt;
    }

    if (eyeRenderTexture->dataReady) {
        ++droppedFrames;
    }

// Debug dropped frames using alternating 30x30 red and green squares
// at the bottom right of the image, see gazebo_yarp_plugins camera
// plugin.
#define DEBUG_SQUARES 0
#if DEBUG_SQUARES
    int found = -1;
    for (int i = 0; i < 10; ++i) {
        unsigned char* pix = img.getPixelAddress(img.width()-15-(i*30),img.height()-15);
        if(i % 2) {
            // Check if this is a RED pixel (255,0,0)
            // lossy carriers will change the value, so we look for a
            // pixel close enought to the one we are looking for
            if (pix[0] <= 5 && pix[1] >= 250 && pix[2] <= 5) {
                found = i;
                break;
            }
        } else {
            // Check if this is a GREEN pixel (255,0,0)
            if (pix[0] >= 250 && pix[1] <= 5 && pix[2] <= 5) {
                found = i;
                break;
            }
        }
    }
    if (found != expected) {
        yWarning() << "InputCallback" << (eye==0?"left ":"right") << "    expected" << expected << "found" << found << "next" <<  (found + 1) % 10;
    }
    expected = (found + 1) % 10;
#endif // DEBUG_SQUARES

    eyeRenderTexture->mutex.lock();

    if(eyeRenderTexture->ptr) {
        unsigned int w = img.width();
        unsigned int h = img.height();
        unsigned int rs = img.getRowSize();
        unsigned char *data = img.getRawImage();

        // update data directly on the mapped buffer
        if (eyeRenderTexture->width == w && eyeRenderTexture->height == h) {
            // Texture and image have the same size: no problems
            memcpy(eyeRenderTexture->ptr, data, eyeRenderTexture->bufferSize);
        } else if (eyeRenderTexture->width >= w && eyeRenderTexture->height >= h) {
            // Texture is larger than image: image is centered in the texture
            int x = (eyeRenderTexture->width - w)/2;
            int y = (eyeRenderTexture->height - h)/2;
            for (unsigned int i = 0; i < h; ++i) {
                unsigned char* textureStart = eyeRenderTexture->ptr + (y+i)*eyeRenderTexture->rowSize + x*3;
                unsigned char* dataStart = data + (i*rs);
                memcpy(textureStart, dataStart, rs);
            }
        } else {
            // Texture is smaller than image: image is cropped
            int x = (w - eyeRenderTexture->width)/2;
            int y = (h - eyeRenderTexture->height)/2;
            for (unsigned int i = 0; i < eyeRenderTexture->width; ++i) {
                unsigned char* textureStart = eyeRenderTexture->ptr + (y+i)*(i*eyeRenderTexture->rowSize) + x*3;
                unsigned char* dataStart = data + (y+i)*rs + x*3;
                memcpy(textureStart, dataStart, eyeRenderTexture->rowSize);
            }
        }

//        float roll = OVR::DegreeToRad(static_cast<float>(-img.roll)) + rollOffset;
//        float pitch = OVR::DegreeToRad(static_cast<float>(img.pitch)) + rollOffset;
//        float yaw = OVR::DegreeToRad(static_cast<float>(img.yaw)) + rollOffset;
//        float x = static_cast<float>(img.x);
//        float y = static_cast<float>(img.y);
//        float z = static_cast<float>(img.z);

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float roll = rollOffset;
        float pitch = pitchOffset;
        float yaw = yawOffset;

        yarp::os::Bottle b;
        yarp::os::BufferedPort<ImageType>::getEnvelope(b);
        if (b.size() == 3) {
            roll += OVR::DegreeToRad(static_cast<float>(b.get(0).asDouble()));
            pitch += OVR::DegreeToRad(static_cast<float>(b.get(1).asDouble()));
            yaw += OVR::DegreeToRad(static_cast<float>(b.get(2).asDouble()));
        }
        //yDebug() << b.toString() << roll << pitch << yaw;

        eyeRenderTexture->eyePose.Orientation.w = (float)(- cos(roll/2) * cos(pitch/2) * cos(yaw/2) - sin(roll/2) * sin(pitch/2) * sin(yaw/2));
        eyeRenderTexture->eyePose.Orientation.x = (float)(- cos(roll/2) * sin(pitch/2) * cos(yaw/2) - sin(roll/2) * cos(pitch/2) * sin(yaw/2));
        eyeRenderTexture->eyePose.Orientation.y = (float)(- cos(roll/2) * cos(pitch/2) * sin(yaw/2) + sin(roll/2) * sin(pitch/2) * cos(yaw/2));
        eyeRenderTexture->eyePose.Orientation.z = (float)(- sin(roll/2) * cos(pitch/2) * cos(yaw/2) + cos(roll/2) * sin(pitch/2) * sin(yaw/2));

        eyeRenderTexture->eyePose.Position.x = x;
        eyeRenderTexture->eyePose.Position.y = y;
        eyeRenderTexture->eyePose.Position.z = z;


        eyeRenderTexture->imageWidth = img.width();
        eyeRenderTexture->imageHeight = img.height();

        eyeRenderTexture->dataReady = true;
    }
    eyeRenderTexture->mutex.unlock();
}
