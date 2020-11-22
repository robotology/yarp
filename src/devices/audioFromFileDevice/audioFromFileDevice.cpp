/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "audioFromFileDevice.h"

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <mutex>
#include <string>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

#define SAMPLES_TO_BE_COPIED 512
#define DEFAULT_PERIOD 0.01   //s

namespace {
YARP_LOG_COMPONENT(AUDIOFROMFILE, "yarp.device.audioFromFileDevice")
}

typedef unsigned short int audio_sample_16t;

audioFromFileDevice::audioFromFileDevice() :
        PeriodicThread(DEFAULT_PERIOD),
        m_audio_filename("audio.wav"),
        m_bpnt(0)
{
}

audioFromFileDevice::~audioFromFileDevice()
{
    close();
}

bool audioFromFileDevice::open(yarp::os::Searchable &config)
{
    //sets the thread period
    if(config.check("period"))
    {
        double period = config.find("period").asFloat64();
        setPeriod(period);
        yCInfo(AUDIOFROMFILE) << "Using chosen period of " << period << " s";
    }
    else
    {
        yCInfo(AUDIOFROMFILE) << "Using default period of " << DEFAULT_PERIOD << " s";
    }

    //sets the filename
    if (config.check("file_name"))
    {
        m_audio_filename = config.find("file_name").asString();
        yCInfo(AUDIOFROMFILE) << "Audio will loaded from file:" << m_audio_filename;
    }
    else
    {
        yCInfo(AUDIOFROMFILE) << "No `file_name` option specified. Audio will loaded from default file:" << m_audio_filename;
    }

    //opens the file
    bool ret = yarp::sig::file::read(m_audioFile, m_audio_filename.c_str());
    if (ret == false)
    {
        yCError(AUDIOFROMFILE) << "Unable to open file" << m_audio_filename.c_str();
        return false;
    }

    //sets the audio configuration equal to the audio file
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

bool audioFromFileDevice::close()
{
    audioFromFileDevice::stop();
    if (m_inputBuffer)
    {
        delete m_inputBuffer;
        m_inputBuffer = 0;
    }
    return true;
}


bool audioFromFileDevice::threadInit()
{
    return true;
}


void audioFromFileDevice::run()
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
    yCDebug(AUDIOFROMFILE) << "b_pnt" << m_bpnt << "/" << fsize_in_bytes << " bytes";
#endif
}
