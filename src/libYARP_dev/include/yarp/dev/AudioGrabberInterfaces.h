/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/dev/AudioBufferSize.h>

namespace yarp {
    namespace dev {
        class IAudioGrabberSound;
        class IAudioRender;
    }
}

constexpr yarp::conf::vocab32_t VOCAB_AUDIO_INTERFACE = yarp::os::createVocab('s', 'n', 'd');
constexpr yarp::conf::vocab32_t VOCAB_AUDIO_START     = yarp::os::createVocab('r', 'u', 'n');
constexpr yarp::conf::vocab32_t VOCAB_AUDIO_STOP      = yarp::os::createVocab('s', 't', 'o', 'p');

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
     * Examples:
     * getSound(s, 0,   100,    0.1); returns a sound whose size can vary between 0 and 100, with a maximum blocking time of 0.1
     * getSound(s, 100, 100,    0.0); returns a sound with exact size of 100. It may block forever (more specifically, until sound size is at least 100).
     * getSound(s, 100, 100000, 0.0); returns a sound with a minimum size of 0, while trying to transfer all the internal buffer. It may block forever (more specifically, until sound size is at least 100).
     * @param sound the sound to be filled
     * @param min_number_of_samples. The function will block until the driver is able to collect at least min_number_of_samples. If set to zero, the function may return empty sounds.
     * @param max_number_of_samples. The function will block until the driver is either able to collect max_number_of_samples or the timeout expires.
     * @param max_samples_timeout_s. The timeout (in seconds) to retrieve max_number_of_samples.
     * @return true upon success, false for an invalid set of parameters, such as max_number_of_samples<min_number_of_samples, etc.
     */
    virtual bool getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s) = 0;

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

    virtual bool getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size) = 0;

    virtual bool getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size) = 0;

    virtual bool resetRecordingAudioBuffer() = 0;
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


#endif // YARP_DEV_AUDIOGRABBERINTERFACES_H
