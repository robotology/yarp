/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

#define HW_CHANNELS         2
#define SAMPLING_RATE       44100

namespace {
YARP_LOG_COMPONENT(FAKESPEAKER, "yarp.device.fakeSpeaker")
}

typedef unsigned short int audio_sample_16t;

fakeSpeaker::fakeSpeaker() :
        PeriodicThread(DEFAULT_PERIOD)
{
}

fakeSpeaker::~fakeSpeaker()
{
    close();
}

bool fakeSpeaker::open(yarp::os::Searchable &config)
{
    //sets the thread period
    if( config.check("period"))
    {
        double period = config.find("period").asFloat64();
        setPeriod(period);
        yCInfo(FAKESPEAKER) << "Using chosen period of " << period << " s";
    }
    else
    {
        yCInfo(FAKESPEAKER) << "Using default period of " << DEFAULT_PERIOD << " s";
    }

    //configuration of the simulated audio card
    m_cfg_numSamples = config.check("samples",Value(SAMPLING_RATE),"Number of samples per network packet.").asInt32();
    m_cfg_numChannels = config.check("channels",Value(HW_CHANNELS),"Number of audio channels.").asInt32();
    m_cfg_frequency = config.check("frequency",Value(SAMPLING_RATE),"Sampling frequency.").asInt32();
    m_cfg_bytesPerSample = config.check("channels",Value(2),"Bytes per sample.").asInt8();

    AudioBufferSize buffer_size(m_cfg_numSamples, m_cfg_numChannels, m_cfg_bytesPerSample);
    m_outputBuffer = new yarp::dev::CircularAudioBuffer_16t("fake_speaker_buffer", buffer_size);

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
        audio_sample_16t s = m_outputBuffer->read();
        YARP_UNUSED(s);
    }
    yCDebug(FAKESPEAKER) << "Sound Playback complete";
    yCDebug(FAKESPEAKER) << "Played " << siz_sam << " samples, " << siz_chn << " channels, " << siz_byt << " bytes";

    m_isPlaying = false;
#ifdef ADVANCED_DEBUG
    yCDebug(FAKESPEAKER) << "b_pnt" << m_bpnt << "/" << fsize_in_bytes << " bytes";
#endif
}

bool fakeSpeaker::setHWGain(double gain)
{
    yCError(FAKESPEAKER, "setHWGain() Not yet implemented");
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

void fakeSpeaker::waitUntilPlaybackStreamIsComplete()
{
    yCError(FAKESPEAKER, "waitUntilPlaybackStreamIsComplete() Not yet implemented");
    return;
}
