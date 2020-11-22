/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "audioToFileDevice.h"

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <mutex>
#include <string>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(AUDIOTOFILE, "yarp.device.audioToFileDevice")
}

typedef unsigned short int audio_sample_16t;

audioToFileDevice::audioToFileDevice() :
        m_audio_filename("audio_out.wav")
{
}

audioToFileDevice::~audioToFileDevice()
{
    close();
}

bool audioToFileDevice::open(yarp::os::Searchable &config)
{
    if (config.check("file_name"))
    {
        m_audio_filename=config.find("file_name").asString();
        yCInfo(AUDIOTOFILE) << "Audio will be save on exit to file:" << m_audio_filename;
        return true;
    }
    else
    {
        yCInfo(AUDIOTOFILE) << "No `file_name` option specified. Audio will be save on exit to default file:" << m_audio_filename;
    }
    return true;
}

bool audioToFileDevice::close()
{
    if (m_sounds.size() == 0) { return true; }

    //we need to set the number of channels and the frequency before calling the
    //concatenation operator
    m_audioFile.setFrequency(m_sounds.front().getFrequency());
    m_audioFile.resize(0,m_sounds.front().getChannels());
    while (!m_sounds.empty())
    {
        m_audioFile += m_sounds.front();
        m_sounds.pop_front();
    }

    bool ok = yarp::sig::file::write(m_audioFile, m_audio_filename.c_str());
    if (ok)
    {
        yCDebug(AUDIOTOFILE) <<"Wrote audio to:" << m_audio_filename;
    }

    return true;
}

bool audioToFileDevice::getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    //size = 0;
    return true;
}

bool audioToFileDevice::getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    //size = 0;
    return true;
}

bool audioToFileDevice::resetPlaybackAudioBuffer()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sounds.clear();
    return true;
}

bool audioToFileDevice::startPlayback()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    yCDebug(AUDIOTOFILE) << "start";
    m_playback_running = true;
    return true;
}

bool audioToFileDevice::stopPlayback()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    yCDebug(AUDIOTOFILE) << "stop";
    m_playback_running = false;
    return true;
}

bool audioToFileDevice::renderSound(const yarp::sig::Sound& sound)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_playback_running)
    {
        m_sounds.push_back(sound);
    }
    return true;
}
