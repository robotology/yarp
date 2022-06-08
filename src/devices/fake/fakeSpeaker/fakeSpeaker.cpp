/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeSpeaker.h"

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

fakeSpeaker::fakeSpeaker() :
        PeriodicThread(c_DEFAULT_PERIOD)
{
}

fakeSpeaker::~fakeSpeaker()
{
    close();
}

bool fakeSpeaker::open(yarp::os::Searchable &config)
{
    if (config.check("help"))
    {
        yCInfo(FAKESPEAKER, "Some examples:");
        yCInfo(FAKESPEAKER, "yarpdev --device fakeSpeaker --help");
        yCInfo(FAKESPEAKER, "yarpdev --device AudioPlayerWrapper --subdevice fakeSpeaker --start");
        return false;
    }

    bool b = configurePlayerAudioDevice(config.findGroup("AUDIO_BASE"), "fakeSpeaker");
    if (!b) { return false; }

    //sets the thread period
    if( config.check("period"))
    {
        double period = config.find("period").asFloat64();
        setPeriod(period);
        yCInfo(FAKESPEAKER) << "Using chosen period of " << period << " s";
    }
    else
    {
        yCInfo(FAKESPEAKER) << "Using default period of " << c_DEFAULT_PERIOD << " s";
    }

    //start the capture thread
    start();
    return true;
}

bool fakeSpeaker::close()
{
    fakeSpeaker::stop();

    //wait until the thread is stopped...

    return true;
}


bool fakeSpeaker::threadInit()
{
    return true;
}

void fakeSpeaker::run()
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

bool fakeSpeaker::setHWGain(double gain)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (gain > 0)
    {
        m_hw_gain = gain;
        return true;
    }
    return false;
}

bool fakeSpeaker::configureDeviceAndStart()
{
    yCError(FAKESPEAKER, "configureDeviceAndStart() Not yet implemented");
    return true;
}

bool fakeSpeaker::interruptDeviceAndClose()
{
    yCError(FAKESPEAKER, "interruptDeviceAndClose() Not yet implemented");
    return true;
}
