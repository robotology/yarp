/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogStream.h>

#include <fakeMicrophone.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

#define HW_CHANNELS         2
#define SAMPLING_RATE       44100
#define CHUNK_SIZE          512
#define SLEEP_TIME          0.005
#define SAMPLES_TO_BE_COPIED 64

typedef unsigned short int audio_sample_16t;

fakeMicrophone::fakeMicrophone() : PeriodicThread(DEFAULT_PERIOD)
{
    m_inputBuffer = 0;
    m_audio_filename = "audio.wav";
    m_bpnt = 0;
    m_getSoundIsNotBlocking = false;
    m_isRecording = false;
}

fakeMicrophone::~fakeMicrophone()
{
    close();
}

bool fakeMicrophone::open(yarp::os::Searchable &config)
{
    //sets the thread period
    double period;
    if( config.check("period"))
    {
        period = config.find("period").asFloat64();
        setPeriod(period);
        yInfo() << "Using chosen period of " << period << " s";
    }
    else
    {
        yInfo() << "Using default period of " << DEFAULT_PERIOD << " s";
    }

    //sets the filename
    if (config.check("audio_file"))
    {
        m_audio_filename = config.find("audio_file").asString();
    }
    else
    {
        yInfo() << "--audio_file option not found. Using default:" << m_audio_filename;
    }

    //opens the file
    bool ret = yarp::sig::file::read(m_audioFile, m_audio_filename.c_str());
    if (ret == false)
    {
        yError() << "unable to open file" << m_audio_filename.c_str();
        return false;
    }

    //sets the fake mic configuration equal to the audio file
    m_cfg_numSamples = m_audioFile.getSamples();
    m_cfg_numChannels = m_audioFile.getChannels();
    m_cfg_frequency = m_audioFile.getFrequency();
    m_cfg_bytesPerSample = m_audioFile.getBytesPerSample();
    
    AudioBufferSize buffer_size(m_cfg_numSamples, m_cfg_numChannels, m_cfg_bytesPerSample);
    m_inputBuffer = new yarp::dev::CircularAudioBuffer_16t("fake_mic_buffer", buffer_size);
    
    //start the capture thread
    start();
    return true;
}

bool fakeMicrophone::close()
{
    fakeMicrophone::stop();
    if (m_inputBuffer)
    {
        delete m_inputBuffer;
        m_inputBuffer = 0;
    }
    return true;
}


bool fakeMicrophone::threadInit()
{
    return true;
}


void fakeMicrophone::run()
{
    // when not recording, do nothing
    if (!m_isRecording)
    {
        return;
    }

    // Just acquire raw data and put them in the buffer
    auto p = m_audioFile.getInterleavedAudioRawData();
    size_t fsize_in_samples = m_audioFile.getSamples();
    size_t bps = m_audioFile.getBytesPerSample();

    //each iteration, which occurs every xxx ms, I copy a bunch of samples in the buffer.
    //When the pointer reaches the end of the sound (audioFile), just restart from the beginning in an endless loop
    for (size_t i = 0; i < SAMPLES_TO_BE_COPIED; i++)
    {
        if (m_bpnt >= fsize_in_samples)
        {
            m_bpnt = 0;
        }
        m_inputBuffer->write(p.at(i));
        m_bpnt++;
    }
#ifdef ADVANCED_DEBUG
    yDebug() << "b_pnt" << m_bpnt << "/" << fsize_in_bytes << " bytes";
#endif
}

bool fakeMicrophone::startRecording()
{
    m_isRecording = true;
    resetRecordingAudioBuffer();
    return true;
}


bool fakeMicrophone::stopRecording()
{
    m_isRecording = false;
    resetRecordingAudioBuffer();
    return true;
}


bool fakeMicrophone::getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    size = this->m_inputBuffer->getMaxSize();
    return true;
}


bool fakeMicrophone::getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    size = this->m_inputBuffer->size();
    return true;
}


bool fakeMicrophone::resetRecordingAudioBuffer()
{
    m_inputBuffer->clear();
    return true;
}

bool fakeMicrophone::getSound(yarp::sig::Sound& sound)
{
    if (m_isRecording == false)
    {
        this->startRecording();
    }

    size_t buff_size_in_samples = 0;
    size_t buff_size_watchdog = 0;
    do
    {
        buff_size_in_samples = m_inputBuffer->size().getSamples();
        if (buff_size_watchdog > 100)
        {
            if (buff_size_in_samples == 0)
            {
                yError() << "fakeMicrophone::getSound() Buffer size is still zero after 100 iterations, returning";
                return false;
            }
            else
            {
                yDebug() << "fakeMicrophone::getSound() Buffer size is " << buff_size_in_samples << "/" << this->m_cfg_numSamples << " samples, after 100 iterations";
                if (m_getSoundIsNotBlocking)
                {
                    yError() << "fakeMicrophone::getSound() is in not-blocking mode, returning";
                    return false;
                }
            }
        }
        buff_size_watchdog++;
        yarp::os::SystemClock::delaySystem(SLEEP_TIME);
    } while (buff_size_in_samples < this->m_cfg_numSamples);

    //prepare the sound data struct
    if (sound.getChannels() != this->m_cfg_numChannels && sound.getSamples() != this->m_cfg_numSamples)
    {
        sound.resize(this->m_cfg_numSamples, this->m_cfg_numChannels);
    }
    sound.setFrequency(this->m_cfg_frequency);

    //fill the sound data struct, reading samples from the circular buffer
    for (size_t i = 0; i<this->m_cfg_numSamples; i++)
        for (size_t j = 0; j<this->m_cfg_numChannels; j++)
        {
            audio_sample_16t s = m_inputBuffer->read();
            sound.set(s, i, j);
        }
    auto debug_p = sound.getInterleavedAudioRawData();

    return true;
}
