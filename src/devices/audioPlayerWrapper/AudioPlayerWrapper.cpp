/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define _USE_MATH_DEFINES

#include "AudioPlayerWrapper.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/LogStream.h>

#include <cmath>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

#define DEFAULT_THREAD_PERIOD 0.02 //s

AudioPlayerWrapper::AudioPlayerWrapper() :
        PeriodicThread(DEFAULT_THREAD_PERIOD),
        m_irender(nullptr),
        m_period(DEFAULT_THREAD_PERIOD),
        m_buffer_delay(5.0), //seconds
        m_isDeviceOwned(false),
        m_debug_enabled(false)
{
}

AudioPlayerWrapper::~AudioPlayerWrapper()
{
    m_irender = nullptr;
}

/**
  * Specify which sensor this thread has to read from.
  */

bool AudioPlayerWrapper::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yError("AudioPlayerWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(m_irender);
    }

    if (nullptr == m_irender)
    {
        yError("AudioPlayerWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(m_irender);

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool AudioPlayerWrapper::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_irender = nullptr;
    return true;
}

void AudioPlayerWrapper::attach(yarp::dev::IAudioRender *irend)
{
    m_irender = irend;
}

void AudioPlayerWrapper::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_irender = nullptr;
}

bool AudioPlayerWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) return false;
    reply.clear();

    if (command.get(0).asString() == "start")
    {
        m_irender->startPlayback();
        reply.addVocab(VOCAB_OK);
    }
    else if (command.get(0).asString() == "stop")
    {
        m_irender->stopPlayback();
        reply.addVocab(VOCAB_OK);
    }
    else if (command.get(0).asString() == "clear")
    {
        m_irender->resetPlaybackAudioBuffer();
        reply.addVocab(VOCAB_OK);
    }
    else if (command.get(0).asString() == "help")
    {
        reply.addVocab(yarp::os::Vocab::encode("many"));
        reply.addString("start");
        reply.addString("stop");
        reply.addString("clear");
    }
    else
    {
        yError() << "Invalid command";
        reply.addVocab(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }
    return true;
}

bool AudioPlayerWrapper::threadInit()
{
    return true;
}

bool AudioPlayerWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (config.check("debug"))
    {
        m_debug_enabled = true;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asFloat64();
    }

    string name = "/audioPlayerWrapper";
    if (config.check("name"))
    {
        name = config.find("name").asString();
    }
    m_audioInPortName = name + "/audio:i";
    m_rpcPortName = name + "/rpc:i";
    m_statusPortName = name + "/status:o";

    if(!initialize_YARP(config) )
    {
        yError() << "AudioPlayerWrapper: Error initializing YARP ports";
        return false;
    }

    if (config.check("playback_network_buffer_lenght"))
    {
        m_buffer_delay = config.find("playback_network_buffer_lenght").asFloat64();
    }
    yInfo() << "Using a 'playback_network_buffer_lenght' of" << m_buffer_delay << "s";
    yInfo() << "Increase this value to robustify the real-time audio stream (it will increase latency too)";

    if(config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if(!m_driver.open(p) || !m_driver.isValid())
        {
            yError() << "audioPlayerWrapper: failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&m_driver, "1");
        if(!attachAll(driverlist))
        {
            yError() << "audioPlayerWrapper: failed to open subdevice.. check params";
            return false;
        }
        m_isDeviceOwned = true;
    }


    if (m_irender == nullptr)
    {
        yError("m_irender is null\n");
        return false;
    }

    bool b=m_irender->getPlaybackAudioBufferMaxSize(m_max_buffer_size);
    if (!b)
    {
        yError("getPlaybackAudioBufferMaxSize failed\n");
        return false;
    }

    return true;
}

bool AudioPlayerWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_audioInPort.open(m_audioInPortName))
    {
        yError("AudioPlayerWrapper: failed to open port %s", m_audioInPortName.c_str());
        return false;
    }
    if (!m_statusPort.open(m_statusPortName))
    {
        yError("AudioPlayerWrapper: failed to open port %s", m_statusPortName.c_str());
        return false;
    }
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yError("AudioPlayerWrapper: failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);
    return true;
}

void AudioPlayerWrapper::threadRelease()
{
    m_audioInPort.interrupt();
    m_audioInPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();
    m_statusPort.interrupt();
    m_statusPort.close();
}

void AudioPlayerWrapper::run()
{
    double current_time = yarp::os::Time::now();

    Sound* s = m_audioInPort.read(false);
    if (s != nullptr)
    {
        scheduled_sound_type ss;
#if 1
        //This is simple, but we don't know how big the sound is...
        ss.scheduled_time = current_time + m_buffer_delay;
#elif 0
        //This is ok, but it doesn't work if the sounds have different durations...
        ss.scheduled_time = current_time + 5.0 * s.getDuration();
#else
        ss.scheduled_time = current_time + m_buffer_delay > 5.0 * s.getDuration() ? (m_buffer_delay) : (5.0 * s.getDuration());
#endif
        ss.sound_data = *s;
        m_sound_buffer.push(ss);
    }

    if (!m_sound_buffer.empty() && current_time > m_sound_buffer.front().scheduled_time)
    {
        m_irender->renderSound(m_sound_buffer.front().sound_data);
        m_sound_buffer.pop();
    }

    m_irender->getPlaybackAudioBufferCurrentSize(m_current_buffer_size);
    if (m_debug_enabled)
    {
        static double printer_wdt = yarp::os::Time::now();
        if (yarp::os::Time::now() - printer_wdt > 1.0)
        {
            yDebug() << m_current_buffer_size.getSamples() << "/" << m_max_buffer_size.getSamples() << "samples";
            printer_wdt = yarp::os::Time::now();
        }
    }
}

bool AudioPlayerWrapper::close()
{
    yTrace("AudioPlayerWrapper::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();
    return true;
}
