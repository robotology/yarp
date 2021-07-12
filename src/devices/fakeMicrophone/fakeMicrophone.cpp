/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeMicrophone.h"

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

fakeMicrophone::fakeMicrophone() :
        PeriodicThread(c_DEFAULT_PERIOD)
{
}

fakeMicrophone::~fakeMicrophone()
{
    close();
}

bool fakeMicrophone::open(yarp::os::Searchable &config)
{
    if (config.check("help"))
    {
        yCInfo(FAKEMICROPHONE, "Some examples:");
        yCInfo(FAKEMICROPHONE, "yarpdev --device fakeMicrophone --help");
        yCInfo(FAKEMICROPHONE, "yarpdev --device AudioRecorderWrapper --subdevice fakeMicrophone --start");
        yCInfo(FAKEMICROPHONE, "yarpdev --device AudioRecorderWrapper --subdevice fakeMicrophone --start --signal_frequency 400 --waveform sine");
        yCInfo(FAKEMICROPHONE, "yarpdev --device AudioRecorderWrapper --subdevice fakeMicrophone --start --signal_frequency 400 --waveform sawtooth");
        yCInfo(FAKEMICROPHONE, "yarpdev --device AudioRecorderWrapper --subdevice fakeMicrophone --start --signal_frequency 400 --waveform square");
        yCInfo(FAKEMICROPHONE, "yarpdev --device AudioRecorderWrapper --subdevice fakeMicrophone --start --waveform constant");
        return false;
    }

    bool b = configureRecorderAudioDevice(config.findGroup("AUDIO_BASE"), "fakeMicrophone");
    if (!b) { return false; }

    //sets the thread period
    if(config.check("period"))
    {
        double period = config.find("period").asFloat64();
        setPeriod(period);
        yCInfo(FAKEMICROPHONE) << "Using chosen period of " << period << " s";
    }
    else
    {
        yCInfo(FAKEMICROPHONE) << "Using default period of " << c_DEFAULT_PERIOD << " s";
    }

    if (config.check("signal_frequency"))
    {
        m_sig_freq = config.find("signal_frequency").asInt32();
    }

    if (config.check("amplitude"))
    {
        m_wave_amplitude = config.find("amplitude").asInt32();
    }

    //sets the number of samples processed atomically every thread iteration
    if (config.check("driver_frame_size"))
    {
        m_samples_to_be_copied = config.find("driver_frame_size").asFloat64();
    }
    yCDebug(FAKEMICROPHONE) << m_samples_to_be_copied << " will be processed every iteration";

    if (config.check("waveform"))
    {
        std::string waveform = config.find("waveform").asString();
        if      (config.find("waveform").toString() == "sine") { m_waveform = waveform_t::sine; }
        else if (config.find("waveform").toString() == "sawtooth") { m_waveform = waveform_t::sawtooth; }
        else if (config.find("waveform").toString() == "square") { m_waveform = waveform_t::square; }
        else if (config.find("waveform").toString() == "constant") { m_waveform = waveform_t::constant; }
        else if (config.check("waveform")) { yError() << "Unsupported value for waveform parameter"; return false; }

        if      (m_waveform == waveform_t::sine)      { yCInfo(FAKEMICROPHONE) << "Using sine waveform, signal amplitude=" << m_wave_amplitude     << ", signal frequency=" << m_sig_freq; }
        else if (m_waveform == waveform_t::sawtooth)  { yCInfo(FAKEMICROPHONE) << "Using sawtooth waveform, signal amplitude=" << m_wave_amplitude << ", signal frequency=" << m_sig_freq; }
        else if (m_waveform == waveform_t::square)    { yCInfo(FAKEMICROPHONE) << "Using square waveform, signal amplitude=" << m_wave_amplitude   << ", signal frequency=" << m_sig_freq; }
        else if (m_waveform == waveform_t::constant)  { yCInfo(FAKEMICROPHONE) << "Using constant waveform, signal amplitude="<< m_wave_amplitude  << ", signal frequency=" << m_sig_freq; }
    }

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

bool fakeMicrophone::close()
{
    fakeMicrophone::stop();

    //wait until the thread is stopped...

    return true;
}

bool fakeMicrophone::setHWGain(double gain)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (gain > 0)
    {
        m_hw_gain = gain;
        return true;
    }
    return false;
}

bool fakeMicrophone::threadInit()
{
    return true;
}


void fakeMicrophone::run()
{
    // when not recording, do nothing
    if (!m_recording_enabled)
    {
        return;
    }

    //fill the buffer with a generated tone.
    //each iteration, which occurs every xxx ms (thread period), I copy a fixed amount of
    //samples (m_samples_to_be_copied) in the buffer. This operation cannot be interrupted by stopping the device
    //with m_recording_enabled=false.
    for (size_t i = 0; i < m_samples_to_be_copied; i++)
    {
        // Default values:
        // this signal has amplitude (-32000,32000)
        // the first channel has frequency 440Hz (A4 note)
        // the second channel has frequency 220Hz etc.
        // and so on..
        if (m_waveform == waveform_t::sine)
        {
            for (size_t i = 0; i < m_audiorecorder_cfg.numChannels; i++)
            {
                m_max_count[i] = double(m_audiorecorder_cfg.frequency) / m_sig_freq / double(i + 1);
                double elem1 = double(m_wave_amplitude * sin(double(m_counter[i]) / m_max_count[i] * 2 * M_PI));
                m_inputBuffer->write(elem1 * m_hw_gain);
                m_counter[i]++;
                if (m_counter[i] >= m_max_count[i]) m_counter[i] = 0;
            }
        }
        else if(m_waveform == waveform_t::sawtooth)
        {
            for (size_t i = 0; i < m_audiorecorder_cfg.numChannels; i++)
            {
                m_max_count[i] = double(m_audiorecorder_cfg.frequency) / m_sig_freq / double(i + 1);
                double elem1 = m_wave_amplitude * 2 * (double(m_counter[i])/ m_max_count[i]) - m_wave_amplitude;
                m_inputBuffer->write(elem1 * m_hw_gain);
                m_counter[i]++;
                if (m_counter[i] >= m_max_count[i]) m_counter[i] = 0;
            }
        }
        else if (m_waveform == waveform_t::square)
        {
            for (size_t i = 0; i < m_audiorecorder_cfg.numChannels; i++)
            {
                m_max_count[i] = double(m_audiorecorder_cfg.frequency) / m_sig_freq / double(i + 1);
                double elem1 = m_counter[i] < m_max_count[i]/2  ? m_wave_amplitude : 0;
                m_inputBuffer->write(elem1 * m_hw_gain);
                m_counter[i]++;
                if (m_counter[i] >= m_max_count[i]) m_counter[i] = 0;
            }
        }
        else if (m_waveform == waveform_t::constant)
        {
            for (size_t i = 0; i < m_audiorecorder_cfg.numChannels; i++)
            {
                m_inputBuffer->write(m_wave_amplitude * m_hw_gain / double(i + 1));
                m_counter[i]++;
                if (m_counter[i] >= m_max_count[i]) m_counter[i] = 0;
            }
        }
        else
        {
            yCInfo(FAKEMICROPHONE) << "Not implemented/unreachable code";
        }
    }
}
