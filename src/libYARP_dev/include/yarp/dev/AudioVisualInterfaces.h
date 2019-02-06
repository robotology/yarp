/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_AUDIOVISUALINTERFACES_H
#define YARP_DEV_AUDIOVISUALINTERFACES_H

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/os/PortablePair.h>

namespace yarp{
    namespace dev {
        class IAudioVisualGrabber;
        class IAudioVisualStream;
        class IFrameWriterAudioVisual;
        typedef yarp::os::PortablePair<yarp::sig::ImageOf<yarp::sig::PixelRgb>,
                                       yarp::sig::Sound> ImageRgbSound;
    }
}

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image and sound from a device.
 */
class YARP_dev_API yarp::dev::IAudioVisualGrabber
{
public:
    /**
     * Destructor.
     */
    virtual ~IAudioVisualGrabber(){}

    /**
     * Get an image and sound
     *
     * @param image the image to be filled
     * @param sound the sound to be filled
     * @return true/false upon success/failure
     */
    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) = 0;
};


/**
 * @ingroup dev_iface_media
 *
 * Write a YARP-format image and sound to a device.
 */
class YARP_dev_API yarp::dev::IFrameWriterAudioVisual
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameWriterAudioVisual(){}

    /**
     * Write an image and sound
     *
     * @param image the image to be written
     * @param sound the sound to be written
     * @return true/false upon success/failure
     */
    virtual bool putAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) = 0;
};


/**
 * @ingroup dev_iface_media
 *
 * For streams capable of holding different kinds of content,
 * check what they actually have.
 *
 */
class YARP_dev_API yarp::dev::IAudioVisualStream {
public:
    /**
     * Destructor.
     */
    virtual ~IAudioVisualStream(){}

    virtual bool hasAudio() = 0;

    virtual bool hasVideo() = 0;

    virtual bool hasRawVideo() {
        return hasVideo();
    }
};

#endif // YARP_DEV_AUDIOVISUALINTERFACES_H
