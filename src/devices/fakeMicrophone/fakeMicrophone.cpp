/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "fakeMicrophone.h"

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogStream.h>

#include <mutex>
#include <string>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

#define HW_CHANNELS         2
#define SAMPLING_RATE       44100
#define CHUNK_SIZE          512
#define SLEEP_TIME          0.005
#define SAMPLES_TO_BE_COPIED 512

typedef unsigned short int audio_sample_16t;

fakeMicrophone::fakeMicrophone() :
        PeriodicThread(DEFAULT_PERIOD),
        m_isRecording(false),
        m_cfg_numSamples(0),
        m_cfg_numChannels(0),
        m_cfg_frequency(0),
        m_cfg_bytesPerSample(0),
        m_audio_filename("audio.wav"),
        m_bpnt(0),
        m_inputBuffer(nullptr),
        m_getSoundIsNotBlocking(false)
{
}

fakeMicrophone::~fakeMicrophone()
{
    close();
}

bool fakeMicrophone::open(yarp::os::Searchable &config)
{
    //sets the thread period
    if(config.check("period"))
    {
        double period = config.find("period").asFloat64();
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
    const size_t EXTRA_SPACE = 2;
    AudioBufferSize buffer_size(m_cfg_numSamples*EXTRA_SPACE, m_cfg_numChannels, m_cfg_bytesPerSample);
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
//     size_t bps = m_audioFile.getBytesPerSample();

    //each iteration, which occurs every xxx ms, I copy a bunch of samples in the buffer.
    //When the pointer reaches the end of the sound (audioFile), just restart from the beginning in an endless loop
    for (size_t i = 0; i < SAMPLES_TO_BE_COPIED; i++)
    {
        if (m_bpnt >= fsize_in_samples)
        {
            m_bpnt = 0;
        }
        m_inputBuffer->write((unsigned short)(p.at(m_bpnt).get()));
        m_bpnt++;
    }
#ifdef ADVANCED_DEBUG
    yDebug() << "b_pnt" << m_bpnt << "/" << fsize_in_bytes << " bytes";
#endif
}

bool fakeMicrophone::startRecording()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isRecording = true;
#ifdef BUFFER_AUTOCLEAR
    this->m_recDataBuffer->clear();
#endif
    yInfo() << "fakeMicrophone started recording";
    return true;
}


bool fakeMicrophone::stopRecording()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isRecording = false;
#ifdef BUFFER_AUTOCLEAR
    this->m_recDataBuffer->clear();
#endif
    yInfo() << "fakeMicrophone stopped recording";
    return true;
}


bool fakeMicrophone::getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    size = this->m_inputBuffer->getMaxSize();
    return true;
}


bool fakeMicrophone::getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    size = this->m_inputBuffer->size();
    return true;
}


bool fakeMicrophone::resetRecordingAudioBuffer()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_inputBuffer->clear();
    yDebug() << "PortAudioRecorderDeviceDriver::resetRecordingAudioBuffer";
    return true;
}

bool fakeMicrophone::getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s)
{
    //check for something_to_record
    {
#ifdef AUTOMATIC_REC_START
        if (m_isRecording == false)
        {
            this->startRecording();
        }
#else
        double debug_time = yarp::os::Time::now();
        while (m_isRecording == false)
        {
            if (yarp::os::Time::now() - debug_time > 5.0)
            {
                yInfo() << "getSound() is currently waiting. Use ::startRecording() to start the audio stream";
                debug_time = yarp::os::Time::now();
            }
            yarp::os::SystemClock::delaySystem(SLEEP_TIME);
        }
#endif
    }

    //prevents simultaneous start/stop/reset etc.
    //std::lock_guard<std::mutex> lock(m_mutex); //This must be used carefully

    //check on input parameters
    if (max_number_of_samples < min_number_of_samples)
    {
        yError() << "max_number_of_samples must be greater than min_number_of_samples!";
        return false;
    }
    if (max_number_of_samples > this->m_cfg_numSamples)
    {
        yWarning() << "max_number_of_samples bigger than the internal audio buffer! It will be truncated to:" << this->m_cfg_numSamples;
        max_number_of_samples = this->m_cfg_numSamples;
    }

    //wait until the desired number of samples are obtained
    size_t buff_size = 0;
    double start_time = yarp::os::Time::now();
    double debug_time = yarp::os::Time::now();
    do
    {
        buff_size = m_inputBuffer->size().getSamples();
        if (buff_size >= max_number_of_samples) { break; }
        if (buff_size >= min_number_of_samples && yarp::os::Time::now() - start_time > max_samples_timeout_s) { break; }
        if (m_isRecording == false) { break; }

        if (yarp::os::Time::now() - debug_time > 1.0)
        {
            debug_time = yarp::os::Time::now();
            yDebug() << "PortAudioRecorderDeviceDriver::getSound() Buffer size is " << buff_size << "/" << max_number_of_samples << " after 1s";
        }

        yarp::os::SystemClock::delaySystem(SLEEP_TIME);
    }
    while (true);

    //prepare the sound data struct
    size_t samples_to_be_copied = buff_size;
    if (samples_to_be_copied > max_number_of_samples) samples_to_be_copied = max_number_of_samples;
    if (sound.getChannels() != this->m_cfg_numChannels && sound.getSamples() != samples_to_be_copied)
    {
        sound.resize(samples_to_be_copied, this->m_cfg_numChannels);
    }
    sound.setFrequency(this->m_cfg_frequency);

    //fill the sound data struct, reading samples from the circular buffer
#ifdef DEBUG_TIME_SPENT
    double ct1 = yarp::os::Time::now();
#endif
    for (size_t i = 0; i< samples_to_be_copied; i++)
        for (size_t j = 0; j<this->m_cfg_numChannels; j++)
        {
            int16_t s = (int16_t)(m_inputBuffer->read());
            sound.set(s, i, j);
        }

    auto debug_p = sound.getInterleavedAudioRawData();
#ifdef DEBUG_TIME_SPENT
    double ct2 = yarp::os::Time::now();
    yDebug() << ct2 - ct1;
#endif
    return true;
}
