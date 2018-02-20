/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_AUDIOGRABBERINTERFACES_H
#define YARP_DEV_AUDIOGRABBERINTERFACES_H

#include <yarp/sig/Sound.h>

#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class IAudioGrabberSound;
        class IAudioRender;
    }
}

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format sound block from a device.
 */
class YARP_dev_API yarp::dev::IAudioGrabberSound
{
public:
    /**
     * Destructor.
     */
    virtual ~IAudioGrabberSound(){}

    /**
     * Get a sound from a device.
     *
     * @param sound the sound to be filled
     * @return true/false upon success/failure
     */
    virtual bool getSound(yarp::sig::Sound& sound) = 0;

    /**
     * Start the recording.
     *
     * @return true/false upon success/failure
     */
    virtual bool startRecording() = 0;

     /**
     * Stop the recording.
     *
     * @return true/false upon success/failure
     */
    virtual bool stopRecording() = 0;
};


class YARP_dev_API yarp::dev::IAudioRender
{
public:
    /**
     * Destructor.
     */
    virtual ~IAudioRender(){}

    /**
     * Render a sound using a device (i.e. send it to the speakers).
     *
     * @param sound the sound to be rendered
     * @return true/false upon success/failure
     */
    virtual bool renderSound(yarp::sig::Sound& sound) = 0;
};


#endif // YARP_DEV_AUDIOGRABBERINTERFACES_H
