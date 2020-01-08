/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IAUDIORENDER_H
#define YARP_DEV_IAUDIORENDER_H

#include <yarp/sig/Sound.h>
#include <yarp/dev/api.h>
#include <yarp/dev/AudioBufferSize.h>

namespace yarp {
namespace dev {

class YARP_dev_API IAudioRender
{
public:
    /**
     * Destructor.
     */
    virtual ~IAudioRender();

    /**
     * Render a sound using a device (i.e. send it to the speakers).
     *
     * @param sound the sound to be rendered
     * @return true/false upon success/failure
     */
    virtual bool renderSound(const yarp::sig::Sound& sound) = 0;

    /**
     * Start the playback.
     *
     * @return true/false upon success/failure
     */
    virtual bool startPlayback() = 0;

    /**
     * Stop the playback.
     *
     * @return true/false upon success/failure
     */
    virtual bool stopPlayback() = 0;

    virtual bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) = 0;

    virtual bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) = 0;

    virtual bool resetPlaybackAudioBuffer() = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IAUDIORENDER_H
