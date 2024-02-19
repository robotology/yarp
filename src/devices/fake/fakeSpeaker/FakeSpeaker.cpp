/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeSpeaker.h"

#include <string>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

constexpr double c_DEFAULT_PERIOD = 0.01;   //s

namespace {
YARP_LOG_COMPONENT(FAKESPEAKER, "yarp.device.fakeSpeaker")
}

typedef unsigned short int audio_sample_16t;

FakeSpeaker::FakeSpeaker() :
        PeriodicThread(c_DEFAULT_PERIOD)
{
}

FakeSpeaker::~FakeSpeaker()
{
    close();
}

bool FakeSpeaker::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) {return false;}

    bool b = configurePlayerAudioDevice(config.findGroup("AUDIO_BASE"), "fakeSpeaker");
    if (!b) { return false; }

    //sets the thread period
    setPeriod(m_period);

    //start the capture thread
    start();
    return true;
}

bool FakeSpeaker::close()
{
    FakeSpeaker::stop();

    //wait until the thread is stopped...

    return true;
}


bool FakeSpeaker::threadInit()
{
    return true;
}

void FakeSpeaker::run()
{
    // when not playing, do nothing
    if (!m_playback_enabled)
    {
        return;
    }
    // if the buffer is empty, do nothing
    if (m_outputBuffer->size().getSamples() == 0)
    {
        return;
    }

    //playing implies emptying all the buffer
    size_t siz_sam = m_outputBuffer->size().getSamples();
    size_t siz_chn = m_outputBuffer->size().getChannels();
    size_t siz_byt = m_outputBuffer->size().getBytes();
    size_t buffer_size = siz_sam * siz_chn;
    for (size_t i = 0; i<buffer_size; i++)
    {
        audio_sample_16t s = m_outputBuffer->read();
        s= audio_sample_16t(double(s)*m_hw_gain);
        // if a stop is received during the playback, then exits...
        if (!m_playback_enabled) {return;}
    }

    //the playback is complete...
    if (m_outputBuffer->size().getSamples()==0)
    {
        yCDebug(FAKESPEAKER) << "Sound Playback complete";
        yCDebug(FAKESPEAKER) << "Played " << siz_sam << " samples, " << siz_chn << " channels, " << siz_byt << " bytes";
    }
}

bool FakeSpeaker::setHWGain(double gain)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (gain > 0)
    {
        m_hw_gain = gain;
        return true;
    }
    return false;
}

bool FakeSpeaker::configureDeviceAndStart()
{
    yCError(FAKESPEAKER, "configureDeviceAndStart() Not yet implemented");
    return true;
}

bool FakeSpeaker::interruptDeviceAndClose()
{
    yCError(FAKESPEAKER, "interruptDeviceAndClose() Not yet implemented");
    return true;
}
