// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2_AUDIO_GRABBER_INTERFACES__
#define __YARP2_AUDIO_GRABBER_INTERFACES__

#include <yarp/sig/Sound.h>

namespace yarp {
    namespace dev {
        class IAudioGrabberSound;
    }
}

/**
 * Read a YARP-format sound block from a device.
 */
class yarp::dev::IAudioGrabberSound
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
};


#endif
