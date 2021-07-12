/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

    /**
     * Check if the playback has been enabled (e.g. via startPlayback()/stopPlayback())
     * @param playback_enabled the status of the device
     * @return true/false upon success/failure
     */
    virtual bool isPlaying(bool& playback_enabled) = 0;

    virtual bool getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) = 0;

    virtual bool getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) = 0;

    virtual bool resetPlaybackAudioBuffer() = 0;

    /**
     * Sets a software gain for the played audio
     * @param gain the audio gain (1.0 is the default value)
     * @return true/false upon success/failure
     */
    virtual bool setSWGain(double gain) = 0;

    /**
     * Sets the hardware gain of the playback device (if supported by the hardware)
     * @param gain the audio gain (1.0 is the default value)
     * @return true/false upon success/failure
     */
    virtual bool setHWGain(double gain) = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IAUDIORENDER_H
