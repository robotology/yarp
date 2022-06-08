/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

constexpr double c_DEFAULT_PERIOD=0.01;   //s

namespace {
YARP_LOG_COMPONENT(AUDIOFROMFILE, "yarp.device.audioFromFileDevice")
}

typedef unsigned short int audio_sample_16t;

audioFromFileDevice::audioFromFileDevice() :
        PeriodicThread(c_DEFAULT_PERIOD)
{
}

audioFromFileDevice::~audioFromFileDevice()
{
    delete m_inputBuffer;
}

bool audioFromFileDevice::setHWGain(double gain)
{
    yCInfo(AUDIOFROMFILE) << "Not yet implemented";
    return false;
}

bool audioFromFileDevice::open(yarp::os::Searchable &config)
{
    if (config.check("help"))
    {
        yCInfo(AUDIOFROMFILE, "Some examples:");
        yCInfo(AUDIOFROMFILE, "yarpdev --device audioFromFileDevice --help");
        yCInfo(AUDIOFROMFILE, "yarpdev --device AudioRecorderWrapper --subdevice audioFromFileDevice --start");
        yCInfo(AUDIOFROMFILE, "yarpdev --device AudioRecorderWrapper --subdevice audioFromFileDevice --start --file_name myaudio.wav");
        return false;
    }

    bool b = configureRecorderAudioDevice(config.findGroup("AUDIO_BASE"), "audioFromFileDevice");
    if (!b) { return false; }

    //sets the thread period
    if(config.check("period"))
    {
        double period = config.find("period").asFloat64();
        setPeriod(period);
        yCInfo(AUDIOFROMFILE) << "Using chosen period of " << period << " s";
    }
    else
    {
        yCInfo(AUDIOFROMFILE) << "Using default period of " << c_DEFAULT_PERIOD << " s";
    }

    //sets the number of samples processed atomically every thread iteration
    if (config.check("driver_frame_size"))
    {
        m_samples_to_be_copied = config.find("driver_frame_size").asFloat64();
    }
    yCDebug(AUDIOFROMFILE) << m_samples_to_be_copied << " will be processed every iteration";

    //sets the filename
    if (config.check("file_name"))
    {
        m_audio_filename = config.find("file_name").asString();
        yCInfo(AUDIOFROMFILE) << "Audio will loaded from file:" << m_audio_filename;
    }
    else
    {
        yCInfo(AUDIOFROMFILE) << "No `file_name` option specified. Audio will be loaded from default file:" << m_audio_filename;
    }

    //opens the file
    bool ret = yarp::sig::file::read(m_audioFile, m_audio_filename.c_str());
    if (ret == false)
    {
        yCError(AUDIOFROMFILE) << "Unable to open file" << m_audio_filename.c_str();
        return false;
    }
    yCInfo(AUDIOFROMFILE) << "Loaded file has the following properties: samples:" << m_audioFile.getSamples() << " channels:"<< m_audioFile.getChannels() << " bytes per samples:" << m_audioFile.getBytesPerSample();
    if (m_audioFile.getChannels() != this->m_audiorecorder_cfg.numChannels)
    {
        yCInfo(AUDIOFROMFILE) << "Number of channels mismatch!";
        return false;
    }

    //sets the audio configuration equal to the audio file
    //constexpr size_t c_EXTRA_SPACE = 2;
    //AudioBufferSize buffer_size(m_audiorecorder_cfg.numSamples* c_EXTRA_SPACE, m_audiorecorder_cfg.numChannels, m_audiorecorder_cfg.bytesPerSample);
    //m_inputBuffer = new yarp::dev::CircularAudioBuffer_16t("fake_mic_buffer", buffer_size);

    m_datap = m_audioFile.getInterleavedAudioRawData();
    m_fsize_in_samples = m_audioFile.getSamples();

    //start the capture thread
    start();
    return true;
}

bool audioFromFileDevice::close()
{
    audioFromFileDevice::stop();
    return true;
}


bool audioFromFileDevice::threadInit()
{
    return true;
}


void audioFromFileDevice::run()
{
    // when not recording, do nothing
    if (!m_recording_enabled)
    {
        return;
    }

    // Just acquire raw data and put them in the buffer
    //each iteration, which occurs every xxx ms (thread period), I copy a fixed amount of
    //samples (m_samples_to_be_copied) in the buffer. This operation cannot be interrupted by stopping the device
    //with m_recording_enabled=false. When the pointer reaches the end of the sound (audioFile),
    //just restart from the beginning in an endless loop
    size_t chan_num = m_audioFile.getChannels();
    for (size_t i = 0; i < m_samples_to_be_copied; i++)
    {
        if (m_bpnt >= m_fsize_in_samples)
        {
            m_bpnt = 0;
        }
        for (size_t c=0; c< chan_num; c++)
        {
            m_inputBuffer->write((unsigned short)(m_datap.at(m_bpnt* chan_num +c).get()));
        }
        m_bpnt++;
    }

    if (m_audiobase_debug)
    {
        yCDebug(AUDIOFROMFILE) << "b_pnt" << m_bpnt << "/" << m_fsize_in_samples << " samples";
    }
}
