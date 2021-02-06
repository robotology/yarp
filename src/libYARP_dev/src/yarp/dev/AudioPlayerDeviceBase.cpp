/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

constexpr double c_sleep_time=0.005;

YARP_LOG_COMPONENT(AUDIOPLAYER_BASE, "yarp.devices.AudioPlayerDeviceBase")

//the following macros should never be modified and are used only for development purposes
#define AUTOMATIC_REC_START 0
#define DEBUG_TIME_SPENT 0
#define BUFFER_AUTOCLEAR 0

bool AudioPlayerDeviceBase::getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    size = this->m_outputBuffer->size();
    return true;
}

bool AudioPlayerDeviceBase::getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
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
    return true;
}

bool AudioPlayerDeviceBase::renderSound(const yarp::sig::Sound& sound) { return false; }
bool AudioPlayerDeviceBase::startPlayback() { return false; }
bool AudioPlayerDeviceBase::stopPlayback() { return false; }

bool AudioPlayerDeviceBase::resetPlaybackAudioBuffer()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    this->m_outputBuffer->clear();
    return true;
}
