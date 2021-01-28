/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#define _USE_MATH_DEFINES

#include <yarp/dev/AudioRecorderDeviceBase.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <limits>
#include <cmath>
#include <functional>

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

constexpr double c_sleep_time=0.005;

YARP_LOG_COMPONENT(AUDIORECORDER_BASE, "yarp.devices.AudioRecorderDeviceBase")

//the following macros should never be modified and are used only for development purposes
#define AUTOMATIC_REC_START 0
#define DEBUG_TIME_SPENT 0
#define BUFFER_AUTOCLEAR 0

bool AudioRecorderDeviceBase::getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s)
{
    //check for something_to_record
    {
#if AUTOMATIC_REC_START
        if (m_isRecording == false)
        {
            this->startRecording();
        }
#else
        double debug_time = yarp::os::Time::now();
        while (m_isRecording == false)
        {
            if (yarp::os::Time::now() - debug_time > 5.0)
            {
                yCInfo(AUDIORECORDER_BASE) << "getSound() is currently waiting. Use startRecording() to start the audio stream";
                debug_time = yarp::os::Time::now();
            }
            yarp::os::SystemClock::delaySystem(c_sleep_time);
        }
#endif
    }

    //prevents simultaneous start/stop/reset etc.
    //std::lock_guard<std::mutex> lock(m_mutex); //This must be used carefully

    //check on input parameters
    if (max_number_of_samples < min_number_of_samples)
    {
        yCError(AUDIORECORDER_BASE) << "max_number_of_samples must be greater than min_number_of_samples!";
        return false;
    }
    if (max_number_of_samples > this->m_audiorecorder_cfg.numSamples)
    {
        yCWarning(AUDIORECORDER_BASE) << "max_number_of_samples bigger than the internal audio buffer! It will be truncated to:" << this->m_audiorecorder_cfg.numSamples;
        max_number_of_samples = this->m_audiorecorder_cfg.numSamples;
    }

    //wait until the desired number of samples are obtained
    size_t buff_size = 0;
    double start_time = yarp::os::Time::now();
    double debug_time = yarp::os::Time::now();
    do
    {
        buff_size = m_inputBuffer->size().getSamples();
        if (buff_size >= max_number_of_samples) { break; }
        if (buff_size >= min_number_of_samples && yarp::os::Time::now() - start_time > max_samples_timeout_s) { break; }
        if (m_isRecording == false) { break; }

        if (yarp::os::Time::now() - debug_time > 1.0)
        {
            debug_time = yarp::os::Time::now();
            yCDebug(AUDIORECORDER_BASE) << "getSound() Buffer size is " << buff_size << "/" << max_number_of_samples << " after 1s";
        }

        yarp::os::SystemClock::delaySystem(c_sleep_time);
    } while (true);

    //prepare the sound data struct
    size_t samples_to_be_copied = buff_size;
    if (samples_to_be_copied > max_number_of_samples) samples_to_be_copied = max_number_of_samples;
    if (sound.getChannels() != this->m_audiorecorder_cfg.numChannels && sound.getSamples() != samples_to_be_copied)
    {
        sound.resize(samples_to_be_copied, this->m_audiorecorder_cfg.numChannels);
    }
    sound.setFrequency(this->m_audiorecorder_cfg.frequency);

    //fill the sound data struct, reading samples from the circular buffer
#if DEBUG_TIME_SPENT
    double ct1 = yarp::os::Time::now();
#endif
    for (size_t i = 0; i < samples_to_be_copied; i++)
        for (size_t j = 0; j < this->m_audiorecorder_cfg.numChannels; j++)
        {
            int16_t s = (int16_t)(m_inputBuffer->read());
            sound.set(s, i, j);
        }

    auto debug_p = sound.getInterleavedAudioRawData();
#if DEBUG_TIME_SPENT
    double ct2 = yarp::os::Time::now();
    yCDebug(AUDIORECORDER_BASE) << ct2 - ct1;
#endif
    return true;
}

bool AudioRecorderDeviceBase::getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    size = this->m_inputBuffer->getMaxSize();
    return true;
}


bool AudioRecorderDeviceBase::getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    size = this->m_inputBuffer->size();
    return true;
}


bool AudioRecorderDeviceBase::resetRecordingAudioBuffer()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_inputBuffer->clear();
    yCDebug(AUDIORECORDER_BASE) << "resetRecordingAudioBuffer";
    return true;
}

bool AudioRecorderDeviceBase::startRecording()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isRecording = true;
#if BUFFER_AUTOCLEAR
    this->m_recDataBuffer->clear();
#endif
    yCInfo(AUDIORECORDER_BASE) << "Recording started";
    return true;
}


bool AudioRecorderDeviceBase::stopRecording()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isRecording = false;
#if BUFFER_AUTOCLEAR
    this->m_recDataBuffer->clear();
#endif
    yCInfo(AUDIORECORDER_BASE) << "Recording stopped";
    return true;
}

AudioRecorderDeviceBase::~AudioRecorderDeviceBase()
{
    delete m_inputBuffer;
}
