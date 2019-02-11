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

#include <fakeSpeaker.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

#define HW_CHANNELS         2
#define SAMPLING_RATE       44100
#define CHUNK_SIZE          512
#define SLEEP_TIME          0.005
#define SAMPLES_TO_BE_COPIED 64

/**
 * This device implements a fake analog sensor
 * emulating an IMU
 * @author Alberto Cardellino
 */
fakeSpeaker::fakeSpeaker() : PeriodicThread(DEFAULT_PERIOD)
{
    m_bpnt = 0;
    m_renderSoundImmediate = false;
}

fakeSpeaker::~fakeSpeaker()
{
    close();
}

bool fakeSpeaker::open(yarp::os::Searchable &config)
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

    //configuration of the simulated audio card
    m_cfg_numSamples = config.find("samples").asInt32();
    m_cfg_numChannels = config.find("getChannels").asInt32();
    m_cfg_frequency = config.find("Frequency").asInt32();
    m_cfg_bytesPerSample = config.find("BytesPerSample").asInt8();
    
    audio_buffer_size buffer_size(m_cfg_numSamples, m_cfg_numChannels, m_cfg_bytesPerSample);
    m_outputBuffer = new yarp::dev::circularAudioBuffer("fake_speaker_buffer", buffer_size);
    
    //start the capture thread
    start();
    return true;
}

bool fakeSpeaker::close()
{
    fakeSpeaker::stop();
    if (m_outputBuffer)
    {
        delete m_outputBuffer;
        m_outputBuffer = 0;
    }
    return true;
}


bool fakeSpeaker::threadInit()
{
    return true;
}


void fakeSpeaker::run()
{
    // when not playing, do nothing
    if (!m_isPlaying)
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
        SAMPLE s = m_outputBuffer->read();
    }
    yDebug() << "Sound Playback complete";
    yDebug() << "Played " << siz_sam << " samples, " << siz_chn << " channels, " << siz_byt << " bytes";

    m_isPlaying = false;
#ifdef ADVANCED_DEBUG
    yDebug() << "b_pnt" << m_bpnt << "/" << fsize_in_bytes << " bytes";
#endif
}

bool fakeSpeaker::getPlaybackAudioBufferMaxSize(yarp::dev::audio_buffer_size& size)
{
    size = this->m_outputBuffer->getMaxSize();
    return true;
}


bool fakeSpeaker::getPlaybackAudioBufferCurrentSize(yarp::dev::audio_buffer_size& size)
{
    size = this->m_outputBuffer->size();
    return true;
}


bool fakeSpeaker::resetPlaybackAudioBuffer()
{
    m_outputBuffer->clear();
    return true;
}

bool fakeSpeaker::renderSound(const yarp::sig::Sound& sound)
{
    if (m_renderSoundImmediate) m_outputBuffer->clear();

    size_t num_bytes = sound.getBytesPerSample();
    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i = 0; i<num_samples; i++)
        for (size_t j = 0; j<num_channels; j++)
            m_outputBuffer->write(sound.get(i, j));
    auto debug_p = sound.getInterleavedAudioRawData();

    m_isPlaying = true;
    return true;
}
