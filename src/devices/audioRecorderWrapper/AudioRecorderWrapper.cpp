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

#include "AudioRecorderWrapper.h"
#include <yarp/os/LogStream.h>

using namespace yarp::dev;

#define DEFAULT_THREAD_PERIOD 0.02 //s

#ifdef DEBUG_TIME_SPENT
double last_time;
#endif

AudioRecorderWrapper::AudioRecorderWrapper() :
        PeriodicThread(DEFAULT_THREAD_PERIOD),
        m_mic(nullptr),
        m_period(DEFAULT_THREAD_PERIOD),
        m_min_number_of_samples_over_network(11250),
        m_max_number_of_samples_over_network(11250),
        m_getSound_timeout(1.0),
        m_isDeviceOwned(false)
#ifdef DEBUG_TIME_SPENT
        , last_time(yarp::os::Time::now()),
#endif
{
    m_stamp.update();
}

AudioRecorderWrapper::~AudioRecorderWrapper()
{
    if (m_mic != nullptr)
    {
        close();
    }
}

bool AudioRecorderWrapper::open(yarp::os::Searchable& config)
{
    if (config.check("period"))
    {
        m_period = config.find("period").asFloat64();
    }

    if (config.check("subdevice"))
    {
        yarp::os::Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!m_driver.open(p) || !m_driver.isValid())
        {
            yError() << "AudioRecorderWrapper: failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&m_driver, "1");
        if (!attachAll(driverlist))
        {
            yError() << "AudioRecorderWrapper: failed to open subdevice.. check params";
            return false;
        }
        m_isDeviceOwned = true;
    }

    if (m_mic == nullptr)
    {
        yError("Failed to open IAudioGrabberSound interface");
        return false;
    }

    // Get parameter samples_over_network
    if (config.check("min_samples_over_network"))
    {
        m_min_number_of_samples_over_network = config.find("min_samples_over_network").asInt64();
    }
    if (config.check("max_samples_over_network"))
    {
        m_max_number_of_samples_over_network = config.find("max_samples_over_network").asInt64();
    }
    yInfo() << "Wrapper configured to produce packets with the following size (in samples): " <<
                m_min_number_of_samples_over_network << " < samples < " << m_max_number_of_samples_over_network;


    // Get parameter samples_over_network
    if (config.check("max_samples_timeout"))
    {
        m_getSound_timeout = config.find("max_samples_timeout").asFloat64();
    }
    yInfo() << "Wrapper configured with max_samples_timeout: " << m_getSound_timeout << "s";

    // Set the streaming port
    std::string portname = "/audioRecorderWrapper";
    if (config.check("name"))
    {
        portname= config.find("name").asString();
    }
    if (m_streamingPort.open(portname + "/audio:o") == false)
    {
        yError() << "Unable to open port" << portname;
        return false;
    }

    // Set the RPC port
    if (m_rpcPort.open(portname + "/rpc") == false)
    {
        yError() << "Unable to open port" << portname + "/rpc";
        return false;
    }
    m_rpcPort.setReader(*this);

    // Wait a little and then start if requested
    if (config.check("start")) {
        yarp::os::SystemClock::delaySystem(1);
        m_mic->startRecording();
    }

    return true;
}

bool AudioRecorderWrapper::close()
{
    if (m_mic != nullptr)
    {
        PeriodicThread::stop();
        m_mic->stopRecording();
        m_mic = nullptr;

        m_streamingPort.interrupt();
        m_streamingPort.close();
        m_rpcPort.interrupt();
        m_rpcPort.close();

        return true;
    }
    return false;
}

void AudioRecorderWrapper::run()
{
#ifdef DEBUG_TIME_SPENT
    double current_time = yarp::os::Time::now();
    yDebug() << current_time - m_last_time;
    m_last_time = current_time;
#endif

    if (m_mic == nullptr)
    {
        yError() << "The IAudioGrabberSound interface is not available yet!";
        return;
    }

#ifdef PRINT_DEBUG_MESSAGES
    {
        audio_buffer_size buf_max;
        audio_buffer_size buf_cur;
        mic->getRecordingAudioBufferMaxSize(buf_max);
        mic->getRecordingAudioBufferCurrentSize(buf_cur);
        yDebug() << "BEFORE Buffer status:" << buf_cur.getBytes() << "/" << buf_max.getBytes() << "bytes";
    }
#endif

    yarp::sig::Sound snd;
    m_mic->getSound(snd, m_min_number_of_samples_over_network, m_max_number_of_samples_over_network, m_getSound_timeout);

    if (snd.getSamples() < m_min_number_of_samples_over_network ||
        snd.getSamples() < m_max_number_of_samples_over_network)
    {
            yWarning() << "subdevice->getSound() is not producing sounds of the requested size ("
                       << m_min_number_of_samples_over_network << "<"
                       << snd.getSamples() << "<"
                       << m_max_number_of_samples_over_network << ") failed";
    }

#ifdef PRINT_DEBUG_MESSAGES
    {
        audio_buffer_size buf_max;
        audio_buffer_size buf_cur;
        mic->getRecordingAudioBufferMaxSize(buf_max);
        mic->getRecordingAudioBufferCurrentSize(buf_cur);
        yDebug() << "AFTER Buffer status:" << buf_cur.getBytes() << "/" << buf_max.getBytes() << "bytes";
    }
#endif
#ifdef PRINT_DEBUG_MESSAGES
    yDebug() << "Sound size:" << snd.getSamples()*snd.getChannels()*snd.getBytesPerSample() << " bytes";
    yDebug();
#endif

    //prepare the timestamp
    m_stamp.update();
    m_streamingPort.setEnvelope(m_stamp);

    //check before sending data
    if (snd.getSamples() == 0)
    {
        yError() << "Subdevice produced sound of 0 samples!";
        return;
    }
    if (snd.getChannels() == 0)
    {
        yError() << "Subdevice produced sound of 0 channels!";
        return;
    }
    if (snd.getFrequency() == 0)
    {
        yError() << "Subdevice produced sound with 0 frequency!";
        return;
    }

    //send data
    m_streamingPort.write(snd);
}

bool AudioRecorderWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) return false;
    reply.clear();

    if (command.get(0).asString()=="start")
    {
        m_mic->startRecording();
        reply.addVocab(VOCAB_OK);
    }
    else if (command.get(0).asString() == "stop")
    {
        m_mic->stopRecording();
        reply.addVocab(VOCAB_OK);
    }
    else if (command.get(0).asString() == "clear")
    {
        m_mic->resetRecordingAudioBuffer();
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

bool AudioRecorderWrapper::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yError("AudioPlayerWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(m_mic);
    }

    if (nullptr == m_mic)
    {
        yError("AudioPlayerWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(m_mic);

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool AudioRecorderWrapper::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_mic = nullptr;
    return true;
}

void AudioRecorderWrapper::attach(yarp::dev::IAudioGrabberSound *igrab)
{
    m_mic = igrab;
}

void AudioRecorderWrapper::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_mic = nullptr;
}

bool AudioRecorderWrapper::threadInit()
{
    return true;
}

void AudioRecorderWrapper::threadRelease()
{
/*    m_audioInPort.interrupt();
    m_audioInPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();
    m_statusPort.interrupt();
    m_statusPort.close();*/
}
