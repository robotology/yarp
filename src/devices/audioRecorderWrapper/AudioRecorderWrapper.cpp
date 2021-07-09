/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "AudioRecorderWrapper.h"
#include <yarp/os/LogStream.h>
#include <yarp/dev/audioRecorderStatus.h>

using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(AUDIORECORDERWRAPPER, "yarp.device.AudioRecorderWrapper")
constexpr double DEFAULT_THREAD_PERIOD = 0.02; // seconds
constexpr size_t DEFAULT_MIN_NUMBER_OF_SAMPLES_OVER_NETWORK = 11250;
constexpr size_t DEFAULT_MAX_NUMBER_OF_SAMPLES_OVER_NETWORK = 11250;
constexpr double DEFAULT_GETSOUND_TIMEOUT = 1.0;

}


AudioRecorderWrapper::AudioRecorderWrapper() :
        m_period(DEFAULT_THREAD_PERIOD),
        m_min_number_of_samples_over_network(DEFAULT_MIN_NUMBER_OF_SAMPLES_OVER_NETWORK),
        m_max_number_of_samples_over_network(DEFAULT_MAX_NUMBER_OF_SAMPLES_OVER_NETWORK),
        m_getSound_timeout(DEFAULT_GETSOUND_TIMEOUT)
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
            yCError(AUDIORECORDERWRAPPER) << "Failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&m_driver, "1");
        if (!attachAll(driverlist))
        {
            yCError(AUDIORECORDERWRAPPER) << "Failed to open subdevice.. check params";
            return false;
        }
        m_isDeviceOwned = true;
    }

    if (m_mic == nullptr)
    {
        yCError(AUDIORECORDERWRAPPER, "Failed to open IAudioGrabberSound interface");
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
    yCInfo(AUDIORECORDERWRAPPER) << "Wrapper configured to produce packets with the following size (in samples): " <<
                m_min_number_of_samples_over_network << " < samples < " << m_max_number_of_samples_over_network;


    // Get parameter samples_over_network
    if (config.check("max_samples_timeout"))
    {
        m_getSound_timeout = config.find("max_samples_timeout").asFloat64();
    }
    yCInfo(AUDIORECORDERWRAPPER) << "Wrapper configured with max_samples_timeout: " << m_getSound_timeout << "s";

    // Set the streaming port
    std::string portname = "/audioRecorderWrapper";
    if (config.check("name"))
    {
        portname= config.find("name").asString();
    }
    if (m_streamingPort.open(portname + "/audio:o") == false)
    {
        yCError(AUDIORECORDERWRAPPER) << "Unable to open port" << portname + "/audio:o";
        return false;
    }
    if (m_statusPort.open(portname + "/status:o") == false)
    {
        yCError(AUDIORECORDERWRAPPER) << "Unable to open port" << portname + "/status:o";
        return false;
    }
    // Set the RPC port
    if (m_rpcPort.open(portname + "/rpc") == false)
    {
        yCError(AUDIORECORDERWRAPPER) << "Unable to open port" << portname + "/rpc";
        return false;
    }
    m_rpcPort.setReader(*this);

    bool b = m_mic->getRecordingAudioBufferMaxSize(m_max_buffer_size);
    if (!b)
    {
        yCError(AUDIORECORDERWRAPPER, "getPlaybackAudioBufferMaxSize failed\n");
        return false;
    }

    // Wait a little and then start if requested
    if (config.check("start")) {
        yarp::os::SystemClock::delaySystem(1);
        m_mic->startRecording();
        m_mic->isRecording(m_isRecording);
    }

    return true;
}

bool AudioRecorderWrapper::close()
{
    if (m_mic != nullptr)
    {
        m_dataThread->stop();
        m_statusThread->stop();
        m_mic->stopRecording();
        m_mic = nullptr;

        m_streamingPort.interrupt();
        m_streamingPort.close();
        m_rpcPort.interrupt();
        m_rpcPort.close();
        m_statusPort.interrupt();
        m_statusPort.close();

        return true;
    }
    return false;
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
        m_mic->isRecording(m_isRecording);
        reply.addVocab32(VOCAB_OK);
    }
    else if (command.get(0).asString() == "stop")
    {
        m_mic->stopRecording();
        m_mic->isRecording(m_isRecording);
        reply.addVocab32(VOCAB_OK);
    }
    else if (command.get(0).asString() == "sw_audio_gain")
    {
        double val = command.get(1).asFloat64();
        if (val >= 0)
        {
            m_mic->setSWGain(val);
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(AUDIORECORDERWRAPPER) << "Invalid audio gain";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (command.get(0).asString() == "hw_audio_gain")
    {
        double val = command.get(1).asFloat64();
        if (val >= 0)
        {
            m_mic->setHWGain(val);
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(AUDIORECORDERWRAPPER) << "Invalid audio gain";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (command.get(0).asString() == "clear")
    {
        m_mic->resetRecordingAudioBuffer();
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
        yCError(AUDIORECORDERWRAPPER) << "Invalid command";
        reply.addVocab32(VOCAB_ERR);
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
        yCError(AUDIORECORDERWRAPPER, "Cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(m_mic);
    }

    if (nullptr == m_mic)
    {
        yCError(AUDIORECORDERWRAPPER, "Subdevice passed to attach method is invalid");
        return false;
    }
    attach(m_mic);

    m_dataThread = new AudioRecorderDataThread(this);
    m_statusThread = new AudioRecorderStatusThread(this);
    m_dataThread->setPeriod(m_period);
    m_dataThread->start();
    m_statusThread->start();
    return true;
}

bool AudioRecorderWrapper::detachAll()
{
    if (m_dataThread->isRunning())
    {
        m_dataThread->stop();
    }
    if (m_statusThread->isRunning())
    {
        m_statusThread->stop();
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
    if (m_dataThread->isRunning())
    {
        m_dataThread->stop();
    }
    if (m_statusThread->isRunning())
    {
        m_statusThread->stop();
    }
    m_mic = nullptr;
}

void AudioRecorderStatusThread::run()
{
    m_ARW->m_mic->getRecordingAudioBufferCurrentSize(m_ARW->m_current_buffer_size);
    if (m_ARW->m_debug_enabled)
    {
        static double printer_wdt = yarp::os::Time::now();
        if (yarp::os::Time::now() - printer_wdt > 1.0)
        {
            yCDebug(AUDIORECORDERWRAPPER) << m_ARW->m_current_buffer_size.getSamples() << "/" << m_ARW->m_max_buffer_size.getSamples() << "samples";
            printer_wdt = yarp::os::Time::now();
        }
    }

    m_ARW->m_mic->isRecording(m_ARW->m_isRecording);

    //status port
    yarp::dev::audioRecorderStatus status;
    status.enabled = m_ARW->m_isRecording;
    status.current_buffer_size = m_ARW->m_current_buffer_size.getSamples();
    status.max_buffer_size = m_ARW->m_max_buffer_size.getSamples();
    m_ARW->m_statusPort.write(status);
}

void AudioRecorderDataThread::run()
{
    if (0)
    {
        //debug: print the duration of the thread
        double current_time = yarp::os::Time::now();
        yCDebug(AUDIORECORDERWRAPPER) << (current_time - m_ARW->m_debug_last_time);
        m_ARW->m_debug_last_time = current_time;
    }

    if (m_ARW->m_mic == nullptr)
    {
        yCError(AUDIORECORDERWRAPPER) << "The IAudioGrabberSound interface is not available yet!";
        return;
    }

#ifdef PRINT_DEBUG_MESSAGES
    {
        audio_buffer_size buf_max;
        audio_buffer_size buf_cur;
        mic->getRecordingAudioBufferMaxSize(buf_max);
        mic->getRecordingAudioBufferCurrentSize(buf_cur);
        yCDebug(AUDIORECORDERWRAPPER) << "BEFORE Buffer status:" << buf_cur.getBytes() << "/" << buf_max.getBytes() << "bytes";
    }
#endif

    yarp::sig::Sound snd;
    m_ARW->m_mic->getSound(snd, m_ARW->m_min_number_of_samples_over_network, m_ARW->m_max_number_of_samples_over_network, m_ARW->m_getSound_timeout);

    if (snd.getSamples() < m_ARW->m_min_number_of_samples_over_network ||
        snd.getSamples() < m_ARW->m_max_number_of_samples_over_network)
    {
        yCWarning(AUDIORECORDERWRAPPER) << "subdevice->getSound() is not producing sounds of the requested size ("
            << m_ARW->m_min_number_of_samples_over_network << "<"
            << snd.getSamples() << "<"
            << m_ARW->m_max_number_of_samples_over_network << ") failed";
    }

#ifdef PRINT_DEBUG_MESSAGES
    {
        audio_buffer_size buf_max;
        audio_buffer_size buf_cur;
        mic->getRecordingAudioBufferMaxSize(buf_max);
        mic->getRecordingAudioBufferCurrentSize(buf_cur);
        yCDebug(AUDIORECORDERWRAPPER) << "AFTER Buffer status:" << buf_cur.getBytes() << "/" << buf_max.getBytes() << "bytes";
    }
#endif
#ifdef PRINT_DEBUG_MESSAGES
    yCDebug(AUDIORECORDERWRAPPER) << "Sound size:" << snd.getSamples() * snd.getChannels() * snd.getBytesPerSample() << " bytes";
    yCDebug(AUDIORECORDERWRAPPER);
#endif

    //prepare the timestamp
    m_ARW->m_stamp.update();
    m_ARW->m_streamingPort.setEnvelope(m_ARW->m_stamp);

    //check before sending data
    if (snd.getSamples() == 0)
    {
        yCError(AUDIORECORDERWRAPPER) << "Subdevice produced sound of 0 samples!";
        return;
    }
    if (snd.getChannels() == 0)
    {
        yCError(AUDIORECORDERWRAPPER) << "Subdevice produced sound of 0 channels!";
        return;
    }
    if (snd.getFrequency() == 0)
    {
        yCError(AUDIORECORDERWRAPPER) << "Subdevice produced sound with 0 frequency!";
        return;
    }

    //send data
    m_ARW->m_streamingPort.write(snd);
}
