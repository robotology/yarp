/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "AudioRecorder_nws_yarp.h"
#include <yarp/os/LogStream.h>
#include <yarp/dev/AudioRecorderStatus.h>

using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(AUDIORECORDER_NWS_YARP, "yarp.device.audioRecorder_nws_yarp")
constexpr double DEFAULT_THREAD_PERIOD = 0.02; // seconds
constexpr size_t DEFAULT_MIN_NUMBER_OF_SAMPLES_OVER_NETWORK = 11250;
constexpr size_t DEFAULT_MAX_NUMBER_OF_SAMPLES_OVER_NETWORK = 11250;
constexpr double DEFAULT_GETSOUND_TIMEOUT = 1.0;

}


AudioRecorder_nws_yarp::AudioRecorder_nws_yarp() :
        m_period(DEFAULT_THREAD_PERIOD),
        m_min_number_of_samples_over_network(DEFAULT_MIN_NUMBER_OF_SAMPLES_OVER_NETWORK),
        m_max_number_of_samples_over_network(DEFAULT_MAX_NUMBER_OF_SAMPLES_OVER_NETWORK),
        m_getSound_timeout(DEFAULT_GETSOUND_TIMEOUT)
{
    m_stamp.update();
}

AudioRecorder_nws_yarp::~AudioRecorder_nws_yarp()
{
    close();
}

bool AudioRecorder_nws_yarp::open(yarp::os::Searchable& config)
{
    m_config.fromString(config.toString());

    if (config.check("period"))
    {
        m_period = config.find("period").asFloat64();
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
    yCInfo(AUDIORECORDER_NWS_YARP) << "Wrapper configured to produce packets with the following size (in samples): " <<
                m_min_number_of_samples_over_network << " < samples < " << m_max_number_of_samples_over_network;

    // Get parameter samples_over_network
    if (config.check("max_samples_timeout"))
    {
        m_getSound_timeout = config.find("max_samples_timeout").asFloat64();
    }
    yCInfo(AUDIORECORDER_NWS_YARP) << "Wrapper configured with max_samples_timeout: " << m_getSound_timeout << "s";

    // Set the streaming port
    std::string portname = "/audioRecorder_nws";
    if (config.check("name"))
    {
        portname= config.find("name").asString();
    }
    if (m_streamingPort.open(portname + "/audio:o") == false)
    {
        yCError(AUDIORECORDER_NWS_YARP) << "Unable to open port" << portname + "/audio:o";
        return false;
    }
    if (m_statusPort.open(portname + "/status:o") == false)
    {
        yCError(AUDIORECORDER_NWS_YARP) << "Unable to open port" << portname + "/status:o";
        return false;
    }
    // Set the RPC port
    if (m_rpcPort.open(portname + "/rpc") == false)
    {
        yCError(AUDIORECORDER_NWS_YARP) << "Unable to open port" << portname + "/rpc";
        return false;
    }
    m_rpcPort.setReader(*this);

    return true;
}

bool AudioRecorder_nws_yarp::close()
{
    detach();

    m_streamingPort.interrupt();
    m_streamingPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();
    m_statusPort.interrupt();
    m_statusPort.close();

    if (m_dataThread)
    {
        delete m_dataThread;
        m_dataThread = nullptr;
    }
    if (m_statusThread)
    {
        delete m_statusThread;
        m_statusThread = nullptr;
    }
    return true;
}

bool AudioRecorder_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }
    yCDebug(AUDIORECORDER_NWS_YARP) << "read() Command failed";
    return false;
}

bool AudioRecorder_nws_yarp::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(m_mic);
    }

    if (nullptr == m_mic)
    {
        yCError(AUDIORECORDER_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    m_RPC.setInterface(m_mic);

    m_dataThread = new AudioRecorderDataThread(this);
    m_statusThread = new AudioRecorderStatusThread(this);
    m_dataThread->setPeriod(m_period);
    m_dataThread->start();
    m_statusThread->start();

    // Wait a little and then start if requested
    if (m_config.check("start")) {
        yCDebug(AUDIORECORDER_NWS_YARP) << "Auto start requested";
        yarp::os::SystemClock::delaySystem(1);
        m_mic->startRecording();
    }

    return true;
}

bool AudioRecorder_nws_yarp::detach()
{
    if (m_dataThread)
    {
        m_dataThread->stop();
    }
    if (m_statusThread)
    {
        m_statusThread->stop();
    }

    if (m_mic)
    {
        m_mic->stopRecording();
        m_mic = nullptr;
    }
    return true;
}

void AudioRecorderStatusThread::run()
{
    yarp::dev::AudioBufferSize device_buffer_current_size;
    yarp::dev::AudioBufferSize device_buffer_max_size;
    m_ARW->m_mic->getRecordingAudioBufferCurrentSize(device_buffer_current_size);
    m_ARW->m_mic->getRecordingAudioBufferMaxSize(device_buffer_max_size);

    if (m_ARW->m_debug_enabled)
    {
        static double printer_wdt = yarp::os::Time::now();
        if (yarp::os::Time::now() - printer_wdt > 1.0)
        {
            yCDebug(AUDIORECORDER_NWS_YARP) << device_buffer_current_size.getSamples() << "/" << device_buffer_max_size.getSamples() << "samples";
            printer_wdt = yarp::os::Time::now();
        }
    }


    bool isRecording;
    m_ARW->m_mic->isRecording(isRecording);

    //status port
    yarp::dev::AudioRecorderStatus status;
    status.enabled = isRecording;
    status.current_buffer_size = device_buffer_current_size.getSamples();
    status.max_buffer_size = device_buffer_max_size.getSamples();
    m_ARW->m_statusPort.write(status);
}

void AudioRecorderDataThread::run()
{
    if (0)
    {
        //debug: print the duration of the thread
        double current_time = yarp::os::Time::now();
        yCDebug(AUDIORECORDER_NWS_YARP) << (current_time - m_ARW->m_debug_last_time);
        m_ARW->m_debug_last_time = current_time;
    }

    if (m_ARW->m_mic == nullptr)
    {
        yCError(AUDIORECORDER_NWS_YARP) << "The IAudioGrabberSound interface is not available yet!";
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


    bool isRecording;
    m_ARW->m_mic->isRecording(isRecording);
    if (isRecording)
    {
        //acquire sound data from attached device
        yarp::sig::Sound current_sound;
        m_ARW->m_mic->getSound(current_sound, m_ARW->m_min_number_of_samples_over_network, m_ARW->m_max_number_of_samples_over_network, m_ARW->m_getSound_timeout);
        if (current_sound.getSamples() < m_ARW->m_min_number_of_samples_over_network ||
            current_sound.getSamples() < m_ARW->m_max_number_of_samples_over_network)
        {
            yCWarning(AUDIORECORDER_NWS_YARP) << "subdevice->getSound() is not producing sounds of the requested size ("
                << m_ARW->m_min_number_of_samples_over_network << "<"
                << current_sound.getSamples() << "<"
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
            yCDebug(AUDIORECORDERWRAPPER) << "Sound size:" << current_sound.getSamples() * current_sound.getChannels() * current_sound.getBytesPerSample() << " bytes";
            yCDebug(AUDIORECORDERWRAPPER);
        #endif

        //send data
        if (m_ARW->m_send_sound_on_stop)
        {
            if (m_ARW->m_snd.getSamples() > 0) {m_ARW->m_snd += current_sound;}
            else                               {m_ARW->m_snd = current_sound;}
        }
        else
        {
            m_ARW->m_snd = current_sound;
            sendSoundAndClear(m_ARW->m_snd);
        }
    }
    else
    {
        if (m_ARW->m_snd.getSamples()>0)
        {
            sendSoundAndClear(m_ARW->m_snd);
        }
    }
}

bool AudioRecorderDataThread::sendSoundAndClear(yarp::sig::Sound& s)
{
    //check before sending data
    if (s.getSamples() == 0)
    {
        yCError(AUDIORECORDER_NWS_YARP) << "Subdevice produced sound of 0 samples!";
        return false;
    }
    if (s.getChannels() == 0)
    {
        yCError(AUDIORECORDER_NWS_YARP) << "Subdevice produced sound of 0 channels!";
        return false;
    }
    if (s.getFrequency() == 0)
    {
        yCError(AUDIORECORDER_NWS_YARP) << "Subdevice produced sound with 0 frequency!";
        return false;
    }

    //prepare the timestamp
    m_ARW->m_stamp.update();
    m_ARW->m_streamingPort.setEnvelope(m_ARW->m_stamp);

    //send data
    if (m_ARW->m_streamingPort.getOutputCount()>0)
    {
        m_ARW->m_streamingPort.write(s);
    }

    s = yarp::sig::Sound(); //erase all data;
    return true;
}
