/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "AudioPlayerWrapper.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/AudioPlayerStatus.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <cmath>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(AUDIOPLAYERWRAPPER, "yarp.device.AudioPlayerWrapper")
constexpr double DEFAULT_THREAD_PERIOD = 0.02; // seconds
}

AudioPlayerWrapper::AudioPlayerWrapper() :
        PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}

AudioPlayerWrapper::~AudioPlayerWrapper()
{
    m_irender = nullptr;
}

/**
  * Specify which sensor this thread has to read from.
  */

bool AudioPlayerWrapper::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(m_irender);
    }

    if (nullptr == m_irender)
    {
        yCError(AUDIOPLAYERWRAPPER, "Subdevice passed to attach method is invalid");
        return false;
    }

    if (!m_irender->getPlaybackAudioBufferMaxSize(m_max_buffer_size))
    {
        yCError(AUDIOPLAYERWRAPPER, "getPlaybackAudioBufferMaxSize failed\n");
        return false;
    }

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool AudioPlayerWrapper::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_irender = nullptr;
    return true;
}

bool AudioPlayerWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }
    reply.clear();

    if (command.get(0).asString() == "start")
    {
        m_irender->startPlayback();
        m_irender->isPlaying(m_isPlaying);
        reply.addVocab32(VOCAB_OK);
    }
    else if (command.get(0).asString() == "stop")
    {
        m_irender->stopPlayback();
        m_irender->isPlaying(m_isPlaying);
        reply.addVocab32(VOCAB_OK);
    }
    else if (command.get(0).asString() == "sw_audio_gain")
    {
        double val = command.get(1).asFloat64();
        if (val>=0)
        {
            m_irender->setSWGain(val);
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(AUDIOPLAYERWRAPPER) << "Invalid audio gain";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (command.get(0).asString() == "hw_audio_gain")
    {
        double val = command.get(1).asFloat64();
        if (val >= 0)
        {
            m_irender->setHWGain(val);
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(AUDIOPLAYERWRAPPER) << "Invalid audio gain";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (command.get(0).asString() == "clear")
    {
        m_irender->resetPlaybackAudioBuffer();
        reply.addVocab32(VOCAB_OK);
    }
    else if (command.get(0).asString() == "help")
    {
        reply.addVocab32("many");
        reply.addString("start");
        reply.addString("stop");
        reply.addString("clear");
        reply.addString("sw_audio_gain <gain>");
        reply.addString("hw_audio_gain <gain>");
    }
    else
    {
        yCError(AUDIOPLAYERWRAPPER) << "Invalid command";
        reply.addVocab32(VOCAB_ERR);
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
    if (!parseParams(config)) { return false; }

    std::string audioInPortName = m_name + "/audio:i";
    std::string rpcPortName = m_name + "/rpc:i";
    std::string statusPortName = m_name + "/status:o";

    if (!m_audioInPort.open(audioInPortName))
    {
        yCError(AUDIOPLAYERWRAPPER, "Failed to open port %s", audioInPortName.c_str());
        return false;
    }
    if (!m_statusPort.open(statusPortName))
    {
        yCError(AUDIOPLAYERWRAPPER, "Failed to open port %s", statusPortName.c_str());
        return false;
    }
    if (!m_rpcPort.open(rpcPortName))
    {
        yCError(AUDIOPLAYERWRAPPER, "Failed to open port %s", rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    yCInfo(AUDIOPLAYERWRAPPER) << "Using a 'playback_network_buffer_size' of" << m_playback_network_buffer_size << "s";
    yCInfo(AUDIOPLAYERWRAPPER) << "Increase this value to robustify the real-time audio stream (it will increase latency too)";

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

void AudioPlayerWrapper::afterStart(bool success)
{
    if(success)
    {
        if (m_start)
        {
            m_irender->startPlayback();
            m_irender->isPlaying(m_isPlaying);
        }
    }
}

void AudioPlayerWrapper::run()
{
    double current_time = yarp::os::Time::now();

    Sound* s = m_audioInPort.read(false);
    if (s != nullptr)
    {
        if (m_debug)
        {
            yCDebug(AUDIOPLAYERWRAPPER) << "Received sound of:" << s->getSamples() << " samples";
        }

        scheduled_sound_type ss;
#if 1
        //This is simple, but we don't know how big the sound is...
        ss.scheduled_time = current_time + m_playback_network_buffer_size;
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
    if (m_debug)
    {
        static double printer_wdt = yarp::os::Time::now();
        if (yarp::os::Time::now() - printer_wdt > 1.0)
        {
            yCDebug(AUDIOPLAYERWRAPPER) << m_current_buffer_size.getSamples() << "/" << m_max_buffer_size.getSamples() << "samples";
            printer_wdt = yarp::os::Time::now();
        }
    }

    m_irender->isPlaying(m_isPlaying);

    //status port
    yarp::dev::AudioPlayerStatus status;
    status.enabled = m_isPlaying;
    status.current_buffer_size = m_current_buffer_size.getSamples();
    status.max_buffer_size = m_max_buffer_size.getSamples();
    m_statusPort.write(status);
}

bool AudioPlayerWrapper::close()
{
    yCTrace(AUDIOPLAYERWRAPPER, "AudioPlayerWrapper::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    return true;
}
