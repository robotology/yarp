// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP2_AUDIO_GRABBER_INTERFACES__
#define __YARP2_AUDIO_GRABBER_INTERFACES__

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


#endif
