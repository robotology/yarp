/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/AudioRecorderDeviceBase.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <limits>
#include <functional>

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

constexpr double c_sleep_time=0.005;

YARP_LOG_COMPONENT(AUDIORECORDER_BASE, "yarp.devices.AudioRecorderDeviceBase")

//the following macros should never be modified and are used only for development purposes
#define AUTOMATIC_REC_START 0
#define DEBUG_TIME_SPENT 0

//Default device parameters
#define DEFAULT_SAMPLE_RATE  (44100)
#define DEFAULT_NUM_CHANNELS    (2)
#define DEFAULT_SAMPLE_SIZE     (2)

bool AudioRecorderDeviceBase::getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s)
{
    //check for something_to_record
    {
    #if AUTOMATIC_REC_START
    if (m_isRecording == false)
    {
    this->startRecording();
    }
    #else
    double debug_time = yarp::os::Time::now();
    while (m_recording_enabled == false)
    {
        if (yarp::os::Time::now() - debug_time > 5.0)
        {
            yCInfo(AUDIORECORDER_BASE) << "getSound() is currently waiting. Use startRecording() to start the audio stream";
            debug_time = yarp::os::Time::now();
        }
        yarp::os::SystemClock::delaySystem(c_sleep_time);
        }
    #endif
    }

    //prevents simultaneous start/stop/reset etc.
    //std::lock_guard<std::mutex> lock(m_mutex); //This must be used carefully

    //check on input parameters
    if (max_number_of_samples < min_number_of_samples)
    {
        yCError(AUDIORECORDER_BASE) << "max_number_of_samples must be greater than min_number_of_samples!";
        return false;
    }
    if (max_number_of_samples > this->m_audiorecorder_cfg.numSamples)
    {
        yCWarning(AUDIORECORDER_BASE) << "max_number_of_samples bigger than the internal audio buffer! It will be truncated to:" << this->m_audiorecorder_cfg.numSamples;
        max_number_of_samples = this->m_audiorecorder_cfg.numSamples;
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
        if (m_recording_enabled == false) { break; }

        if (yarp::os::Time::now() - debug_time > 1.0)
        {
            debug_time = yarp::os::Time::now();
            yCDebug(AUDIORECORDER_BASE) << "getSound() Buffer size is " << buff_size << "/" << max_number_of_samples << " after 1s";
        }
        yarp::os::SystemClock::delaySystem(c_sleep_time);
    } while (true);

    //prepare the sound data struct
    size_t samples_to_be_copied = buff_size;
    if (samples_to_be_copied > max_number_of_samples) samples_to_be_copied = max_number_of_samples;
    if (sound.getChannels() != this->m_audiorecorder_cfg.numChannels && sound.getSamples() != samples_to_be_copied)
    {
        sound.resize(samples_to_be_copied, this->m_audiorecorder_cfg.numChannels);
    }
    sound.setFrequency(this->m_audiorecorder_cfg.frequency);

    //fill the sound data struct, reading samples from the circular buffer
    #if DEBUG_TIME_SPENT
    double ct1 = yarp::os::Time::now();
    #endif
    for (size_t i = 0; i < samples_to_be_copied; i++)
    {
        for (size_t j = 0; j < this->m_audiorecorder_cfg.numChannels; j++)
        {
            int16_t s = (int16_t)(m_inputBuffer->read());
            if (s > (std::numeric_limits<int16_t>::max() - m_cliptol) ||
                s < (std::numeric_limits<int16_t>::min() + m_cliptol))
            {
                yCWarningThrottle(AUDIORECORDER_BASE, 0.1) << "Sound clipped!";
            }
            sound.set(s, i, j);
        }
    }

    //amplify if required
    if (m_sw_gain!=1.0) {sound.amplify(m_sw_gain);}

    auto debug_p = sound.getInterleavedAudioRawData();
    #if DEBUG_TIME_SPENT
    double ct2 = yarp::os::Time::now();
    yCDebug(AUDIORECORDER_BASE) << ct2 - ct1;
    #endif
    return true;
}

bool AudioRecorderDeviceBase::getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    if (m_inputBuffer == nullptr)
    {
        yCError(AUDIORECORDER_BASE) << "getRecordingAudioBufferMaxSize() called, but no audio buffer is allocated yet";
        return false;
    }
    //no lock guard is needed here
    size = this->m_inputBuffer->getMaxSize();
    return true;
}


bool AudioRecorderDeviceBase::getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    if (m_inputBuffer == nullptr)
    {
        yCError(AUDIORECORDER_BASE) << "getRecordingAudioBufferCurrentSize() called, but no audio buffer is allocated yet";
        return false;
    }
    //no lock guard is needed here
    size = this->m_inputBuffer->size();
    return true;
}

bool AudioRecorderDeviceBase::setSWGain(double gain)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (gain > 0)
    {
        m_sw_gain = gain;
        return true;
    }
    return false;
}

bool AudioRecorderDeviceBase::resetRecordingAudioBuffer()
{
    if (m_inputBuffer == nullptr)
    {
        yCError(AUDIORECORDER_BASE) << "resetRecordingAudioBuffer() called, but no audio buffer is allocated yet";
        return false;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    m_inputBuffer->clear();
    yCDebug(AUDIORECORDER_BASE) << "resetRecordingAudioBuffer";
    return true;
}

bool AudioRecorderDeviceBase::startRecording()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_recording_enabled = true;
    if (m_enable_buffer_autoclear && this->m_inputBuffer)
    {
        this->m_inputBuffer->clear();
    }
    yCInfo(AUDIORECORDER_BASE) << "Recording started";
    return true;
}

bool AudioRecorderDeviceBase::stopRecording()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_recording_enabled = false;
    if (m_enable_buffer_autoclear && this->m_inputBuffer)
    {
        //In this case we do not want to clear the because we want to transmit the last sound frame
        //which has been partially captured until the stopRecording has been called.
        //this->m_inputBuffer->clear();
    }
    yCInfo(AUDIORECORDER_BASE) << "Recording stopped";
    return true;
}

bool AudioRecorderDeviceBase::isRecording(bool& recording_enabled)
{
    recording_enabled = m_recording_enabled;
    return true;
}

AudioRecorderDeviceBase::~AudioRecorderDeviceBase()
{
    delete m_inputBuffer;
}

bool AudioRecorderDeviceBase::configureRecorderAudioDevice(yarp::os::Searchable& config, string device_name)
{
    m_audiorecorder_cfg.frequency = config.check("rate", Value(0), "audio sample rate (0=automatic)").asInt32();
    m_audiorecorder_cfg.numSamples = config.check("samples", Value(0), "number of samples per network packet (0=automatic). For chunks of 1 second of recording set samples=rate. Channels number is handled internally.").asInt32();
    m_audiorecorder_cfg.numChannels = config.check("channels", Value(0), "number of audio channels (0=automatic, max is 2)").asInt32();

    if (m_audiorecorder_cfg.frequency == 0)  m_audiorecorder_cfg.frequency = DEFAULT_SAMPLE_RATE;
    if (m_audiorecorder_cfg.numChannels == 0)  m_audiorecorder_cfg.numChannels = DEFAULT_NUM_CHANNELS;
    if (m_audiorecorder_cfg.numSamples == 0) m_audiorecorder_cfg.numSamples = m_audiorecorder_cfg.frequency; //  by default let's use chunks of 1 second

    yCInfo(AUDIORECORDER_BASE) << "Device configured with the following options:";
    yCInfo(AUDIORECORDER_BASE) << "Frequency:"<< m_audiorecorder_cfg.frequency;
    yCInfo(AUDIORECORDER_BASE) << "Samples (buffer size):"<< m_audiorecorder_cfg.numSamples;
    yCInfo(AUDIORECORDER_BASE) << "Channels:"<< m_audiorecorder_cfg.numChannels;
    yCInfo(AUDIORECORDER_BASE) << "BytesForSample:"<< m_audiorecorder_cfg.bytesPerSample;
    yCInfo(AUDIORECORDER_BASE) << "HW gain:" << m_hw_gain;
    yCInfo(AUDIORECORDER_BASE) << "SW gain:" << m_sw_gain;

    AudioBufferSize rec_buffer_size(m_audiorecorder_cfg.numSamples, m_audiorecorder_cfg.numChannels, m_audiorecorder_cfg.bytesPerSample);
    if (m_inputBuffer == nullptr)
    {
         m_inputBuffer = new CircularAudioBuffer_16t(device_name, rec_buffer_size);
    }

    //additional options
    m_enable_buffer_autoclear = config.check("buffer_autoclear", Value(true), "Automatically clear the buffer every time the devices is started/stopped").asBool();
    m_audiobase_debug         = config.check("debug", Value(false), "Enable debug mode").asBool();

    return true;
}
