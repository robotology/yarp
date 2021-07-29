/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include <yarp/dev/AudioPlayerDeviceBase.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <limits>
#include <cmath>
#include <functional>

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

constexpr double c_sleep_time=0.005;

YARP_LOG_COMPONENT(AUDIOPLAYER_BASE, "yarp.devices.AudioPlayerDeviceBase")

//the following macros should never be modified and are used only for development purposes
#define DEBUG_TIME_SPENT 0

//Default device parameters
#define DEFAULT_SAMPLE_RATE  (44100)
#define DEFAULT_NUM_CHANNELS    (2)
#define DEFAULT_SAMPLE_SIZE     (2)

bool AudioPlayerDeviceBase::getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    if (m_outputBuffer == nullptr)
    {
        yCError(AUDIOPLAYER_BASE) << "getPlaybackAudioBufferCurrentSize() called, but no audio buffer is allocated yet";
        return false;
    }
    //no lock guard is needed here
    size = this->m_outputBuffer->size();
    return true;
}

bool AudioPlayerDeviceBase::getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    if (m_outputBuffer == nullptr)
    {
        yCError(AUDIOPLAYER_BASE) << "getPlaybackAudioBufferMaxSize() called, but no audio buffer is allocated yet";
        return false;
    }
    //no lock guard is needed here
    size = this->m_outputBuffer->getMaxSize();
    return true;
}

AudioPlayerDeviceBase::~AudioPlayerDeviceBase()
{
    delete m_outputBuffer;
}

bool AudioPlayerDeviceBase::setSWGain(double gain)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (gain>0)
    {
        m_sw_gain = gain;
        return true;
    }
    return false;
}

bool AudioPlayerDeviceBase::startPlayback()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_playback_enabled = true;
    if (m_enable_buffer_autoclear && this->m_outputBuffer)
        {this->m_outputBuffer->clear();}
    yCInfo(AUDIOPLAYER_BASE) << "Playback started";
    return true;
}

bool AudioPlayerDeviceBase::stopPlayback()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_playback_enabled = false;
    if (m_enable_buffer_autoclear && this->m_outputBuffer)
        {this->m_outputBuffer->clear();}
    yCInfo(AUDIOPLAYER_BASE) << "Playback stopped";
    return true;
}

bool AudioPlayerDeviceBase::isPlaying(bool& playback_enabled)
{
    playback_enabled = m_playback_enabled;
    return true;
}

bool AudioPlayerDeviceBase::resetPlaybackAudioBuffer()
{
    if (m_outputBuffer == nullptr)
    {
        yCError(AUDIOPLAYER_BASE) << "resetPlaybackAudioBuffer() called, but no audio buffer is allocated yet";
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    this->m_outputBuffer->clear();
    return true;
}

bool AudioPlayerDeviceBase::appendSound(const yarp::sig::Sound& sound)
{
    //Do I need a lockguard?
    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i = 0; i < num_samples; i++) {
        for (size_t j = 0; j < num_channels; j++) {
            m_outputBuffer->write(sound.get(i, j));
        }
    }

    return true;
}

bool AudioPlayerDeviceBase::immediateSound(const yarp::sig::Sound& sound)
{
    //Do I need a lockguard?
    m_outputBuffer->clear();

    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i = 0; i < num_samples; i++) {
        for (size_t j = 0; j < num_channels; j++) {
            m_outputBuffer->write(sound.get(i, j));
        }
    }

    return true;
}

bool AudioPlayerDeviceBase::renderSound(const yarp::sig::Sound& sound)
{
    //prevents simultaneous start/stop/reset etc.
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    size_t freq = sound.getFrequency();
    size_t chans = sound.getChannels();
    if (freq == 0)
    {
        yCError(AUDIOPLAYER_BASE) << "received a bad audio sample of frequency 0";
        return false;
    }
    if (chans == 0)
    {
        yCError(AUDIOPLAYER_BASE) << "received a bad audio sample with 0 channels";
        return false;
    }

    //process the sound, if required
    yarp::sig::Sound procsound = sound;
    if (m_sw_gain != 1.0)
    {
        procsound.amplify(m_sw_gain);
    }

    if (freq != this->m_audioplayer_cfg.frequency ||
        chans != this->m_audioplayer_cfg.numChannels)
    {
        //wait for current playback to finish
        waitUntilPlaybackStreamIsComplete();

        //reset the driver
        yCInfo(AUDIOPLAYER_BASE, "***** audio driver configuration changed, resetting");
        yCInfo(AUDIOPLAYER_BASE) << "changing from: " << this->m_audioplayer_cfg.numChannels << "channels, " << this->m_audioplayer_cfg.frequency << " Hz, ->" <<
            chans << "channels, " << freq << " Hz";

        bool was_playing = this->m_playback_enabled;
        //close is called in order to destroy the buffer
        this->interruptDeviceAndClose();

        //The device is re-opened with new configuration parameters
        m_audioplayer_cfg.numChannels = (int)(chans);
        m_audioplayer_cfg.frequency = (int)(freq);
        bool ok = configureDeviceAndStart();
        if (ok == false)
        {
            yCError(AUDIOPLAYER_BASE, "error occurred during audio driver reconfiguration, aborting");
            return false;
        }
        //restore the playback_enabled status before device reconfiguration
        if (was_playing)
        {
            this->startPlayback();
        }
    }

    if (m_renderMode == RENDER_IMMEDIATE) {
        return immediateSound(procsound);
    } else if (m_renderMode == RENDER_APPEND) {
        return appendSound(procsound);
    }

    return false;
}

bool AudioPlayerDeviceBase::configurePlayerAudioDevice(yarp::os::Searchable& config, string device_name)
{
    m_audioplayer_cfg.frequency = config.check("rate", Value(0), "audio sample rate (0=automatic)").asInt32();
    m_audioplayer_cfg.numSamples = config.check("samples", Value(0), "number of samples per network packet (0=automatic). For chunks of 1 second of recording set samples=rate. Channels number is handled internally.").asInt32();
    m_audioplayer_cfg.numChannels = config.check("channels", Value(0), "number of audio channels (0=automatic, max is 2)").asInt32();
    m_hw_gain = config.check("hw_gain", Value(1.0), "HW gain").asFloat32();
    m_sw_gain = config.check("sw_gain", Value(1.0), "SW gain").asFloat32();

    if (m_audioplayer_cfg.numChannels == 0) {
        m_audioplayer_cfg.numChannels = DEFAULT_NUM_CHANNELS;
    }
    if (m_audioplayer_cfg.frequency == 0) {
        m_audioplayer_cfg.frequency = DEFAULT_SAMPLE_RATE;
    }
    if (m_audioplayer_cfg.numSamples == 0) {
        m_audioplayer_cfg.numSamples = m_audioplayer_cfg.frequency; //  by default let's use chunks of 1 second
    }

    if (config.check("render_mode_append"))
    {
        m_renderMode = RENDER_APPEND;
    }
    if (config.check("render_mode_immediate"))
    {
        m_renderMode = RENDER_IMMEDIATE;
    }

    yCInfo(AUDIOPLAYER_BASE) << "Device configured with the following options:";
    yCInfo(AUDIOPLAYER_BASE) << "Frequency:" << m_audioplayer_cfg.frequency;
    yCInfo(AUDIOPLAYER_BASE) << "Samples (buffer size):" << m_audioplayer_cfg.numSamples;
    yCInfo(AUDIOPLAYER_BASE) << "Channels:" << m_audioplayer_cfg.numChannels;
    yCInfo(AUDIOPLAYER_BASE) << "BytesForSample:" << m_audioplayer_cfg.bytesPerSample;
    yCInfo(AUDIOPLAYER_BASE) << "HW gain:" << m_hw_gain;
    yCInfo(AUDIOPLAYER_BASE) << "SW gain:" << m_sw_gain;
    yCInfo(AUDIOPLAYER_BASE) << "Render mode:" << (m_renderMode==RENDER_APPEND?"append":"immediate");

    //create the buffer
    AudioBufferSize buffer_size(m_audioplayer_cfg.numSamples, m_audioplayer_cfg.numChannels, m_audioplayer_cfg.bytesPerSample);
    if (m_outputBuffer == nullptr)
    {
        m_outputBuffer = new yarp::dev::CircularAudioBuffer_16t(device_name, buffer_size);
    }

    //additional options
    m_enable_buffer_autoclear = config.check("buffer_autoclear", Value(false), "Automatically clear the buffer every time the devices is started/stopped").asBool();
    m_audiobase_debug = config.check("debug", Value(false), "Enable debug mode").asBool();

    return true;
}

void AudioPlayerDeviceBase::waitUntilPlaybackStreamIsComplete()
{
    if (m_outputBuffer == nullptr)
    {
        yCError(AUDIOPLAYER_BASE) << "waitUntilPlaybackStreamIsComplete() called, but no audio buffer is allocated yet";
        return;
    }

    while (m_outputBuffer->size().getSamples() != 0)
    {
        yarp::os::Time::delay(0.20);
    }
}
