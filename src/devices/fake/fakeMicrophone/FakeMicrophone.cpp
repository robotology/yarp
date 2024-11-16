/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeMicrophone.h"

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <mutex>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

constexpr double c_DEFAULT_PERIOD=0.01;   //s

namespace {
YARP_LOG_COMPONENT(FAKEMICROPHONE, "yarp.device.fakeMicrophone")
}

typedef unsigned short int audio_sample_16t;

FakeMicrophone::FakeMicrophone() :
        PeriodicThread(c_DEFAULT_PERIOD)
{
}

FakeMicrophone::~FakeMicrophone()
{
    close();
}

bool FakeMicrophone::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) {return false;}

    std::string debug_cfg_string = config.toString();
    yarp::os::Bottle& bb = config.findGroup("AUDIO_BASE");
    bool b = configureRecorderAudioDevice(bb, "fakeMicrophone");
    if (!b) { return false; }

    setPeriod(m_period);

    if      (m_waveform == "sine")     { m_waveform_enum = waveform_t::sine; }
    else if (m_waveform == "sawtooth") { m_waveform_enum = waveform_t::sawtooth; }
    else if (m_waveform == "square")   { m_waveform_enum = waveform_t::square; }
    else if (m_waveform == "constant") { m_waveform_enum = waveform_t::constant; }
    else    { yError() << "Unsupported value for waveform parameter"; return false; }

    if      (m_waveform_enum == waveform_t::sine)      { yCInfo(FAKEMICROPHONE) << "Using sine waveform, signal amplitude=" << m_signal_amplitude << ", signal frequency=" << m_signal_frequency; }
    else if (m_waveform_enum == waveform_t::sawtooth)  { yCInfo(FAKEMICROPHONE) << "Using sawtooth waveform, signal amplitude=" << m_signal_amplitude << ", signal frequency=" << m_signal_frequency; }
    else if (m_waveform_enum == waveform_t::square)    { yCInfo(FAKEMICROPHONE) << "Using square waveform, signal amplitude=" << m_signal_amplitude << ", signal frequency=" << m_signal_frequency; }
    else if (m_waveform_enum == waveform_t::constant)  { yCInfo(FAKEMICROPHONE) << "Using constant waveform, signal amplitude="<< m_signal_amplitude << ", signal frequency=" << m_signal_frequency; }

    //data structure initialization
    //m_audiorecorder_cfg.numSamples = tmp_freq * 5; //5sec
    //const size_t EXTRA_SPACE = 2;
    //AudioBufferSize buffer_size(m_audiorecorder_cfg.numSamples*EXTRA_SPACE, m_audiorecorder_cfg.numChannels, m_audiorecorder_cfg.bytesPerSample);
    //m_inputBuffer = new yarp::dev::CircularAudioBuffer_16t("fake_mic_buffer", buffer_size);

    m_max_count.resize(m_audiorecorder_cfg.numChannels);
    m_counter.resize(m_audiorecorder_cfg.numChannels);

    //start the capture thread
    start();
    return true;
}

bool FakeMicrophone::close()
{
    FakeMicrophone::stop();

    //wait until the thread is stopped...

    return true;
}

bool FakeMicrophone::setHWGain(double gain)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (gain > 0)
    {
        m_hw_gain = gain;
        return true;
    }
    return false;
}

bool FakeMicrophone::threadInit()
{
    return true;
}


void FakeMicrophone::run()
{
    // when not recording, do nothing
    if (!m_recording_enabled)
    {
        return;
    }

    //fill the buffer with a generated tone.
    //each iteration, which occurs every xxx ms (thread period), I copy a fixed amount of
    //samples (m_driver_frame_size) in the buffer. This operation cannot be interrupted by stopping the device
    //with m_recording_enabled=false.
    for (size_t i = 0; i < m_driver_frame_size; i++)
    {
        // Default values:
        // this signal has amplitude (-32000,32000)
        // the first channel has frequency 440Hz (A4 note)
        // the second channel has frequency 220Hz etc.
        // and so on..
        if (m_waveform_enum == waveform_t::sine)
        {
            for (size_t i = 0; i < m_audiorecorder_cfg.numChannels; i++)
            {
                m_max_count[i] = double(m_audiorecorder_cfg.frequency) / m_signal_frequency / double(i + 1);
                double elem1 = double(m_signal_amplitude * sin(double(m_counter[i]) / m_max_count[i] * 2 * M_PI));
                m_inputBuffer->write(elem1 * m_hw_gain);
                m_counter[i]++;
                if (m_counter[i] >= m_max_count[i]) {
                    m_counter[i] = 0;
                }
            }
        }
        else if(m_waveform_enum == waveform_t::sawtooth)
        {
            for (size_t i = 0; i < m_audiorecorder_cfg.numChannels; i++)
            {
                m_max_count[i] = double(m_audiorecorder_cfg.frequency) / m_signal_frequency / double(i + 1);
                double elem1 = m_signal_amplitude * 2.0 * (double(m_counter[i])/ m_max_count[i]) - m_signal_amplitude;
                m_inputBuffer->write(elem1 * m_hw_gain);
                m_counter[i]++;
                if (m_counter[i] >= m_max_count[i]) {
                    m_counter[i] = 0;
                }
            }
        }
        else if (m_waveform_enum == waveform_t::square)
        {
            for (size_t i = 0; i < m_audiorecorder_cfg.numChannels; i++)
            {
                m_max_count[i] = double(m_audiorecorder_cfg.frequency) / m_signal_frequency / double(i + 1);
                double elem1 = m_counter[i] < m_max_count[i]/2  ? m_signal_amplitude : 0;
                m_inputBuffer->write(elem1 * m_hw_gain);
                m_counter[i]++;
                if (m_counter[i] >= m_max_count[i]) {
                    m_counter[i] = 0;
                }
            }
        }
        else if (m_waveform_enum == waveform_t::constant)
        {
            for (size_t i = 0; i < m_audiorecorder_cfg.numChannels; i++)
            {
                m_inputBuffer->write(m_signal_amplitude * m_hw_gain / double(i + 1));
                m_counter[i]++;
                if (m_counter[i] >= m_max_count[i]) {
                    m_counter[i] = 0;
                }
            }
        }
        else
        {
            yCInfo(FAKEMICROPHONE) << "Not implemented/unreachable code";
        }
    }
}
