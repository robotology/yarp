/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IAUDIORENDER_H
#define YARP_DEV_IAUDIORENDER_H

#include <yarp/sig/Sound.h>
#include <yarp/dev/api.h>
#include <yarp/sig/AudioBufferSize.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {

/**
 * @ingroup dev_iface_media
 *
 * Interface for rendering a YARP-format sound and controlling its reproduction ona device
 */

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
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue renderSound(const yarp::sig::Sound& sound) = 0;

    /**
     * Start the playback.
     *
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue startPlayback() = 0;

    /**
     * Stop the playback.
     *
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue stopPlayback() = 0;

    /**
     * Check if the playback has been enabled (e.g. via startPlayback()/stopPlayback())
     * @param playback_enabled the status of the device
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue isPlaying(bool& playback_enabled) = 0;

    virtual yarp::dev::ReturnValue getPlaybackAudioBufferMaxSize(yarp::sig::AudioBufferSize& size) = 0;

    virtual yarp::dev::ReturnValue getPlaybackAudioBufferCurrentSize(yarp::sig::AudioBufferSize& size) = 0;

    virtual yarp::dev::ReturnValue resetPlaybackAudioBuffer() = 0;

    /**
     * Sets a software gain for the played audio
     * @param gain the audio gain (1.0 is the default value)
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue setSWGain(double gain) = 0;

    /**
     * Sets the hardware gain of the playback device (if supported by the hardware)
     * @param gain the audio gain (1.0 is the default value)
     * @return a ReturnValue, convertible to true/false
     */
    virtual yarp::dev::ReturnValue setHWGain(double gain) = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_IAUDIORENDER_H
