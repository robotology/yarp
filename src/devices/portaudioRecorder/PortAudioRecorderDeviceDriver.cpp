/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "PortAudioRecorderDeviceDriver.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <yarp/dev/api.h>

#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;

#define SLEEP_TIME 0.005f

#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#elif 1
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define SAMPLE_UNSIGNED
#endif


namespace {
YARP_LOG_COMPONENT(PORTAUDIORECORDER, "yarp.devices.portaudioRecorder")
}


/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int bufferIOCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    CircularAudioBuffer_16t *recdata = (CircularAudioBuffer_16t*)(userData);
    size_t num_rec_channels     = recdata->getMaxSize().getChannels();
    int finished = paComplete;

    if (1)
    {
        const auto* rptr = (const SAMPLE*)inputBuffer;
        size_t framesToCalc;
        unsigned int i;
        size_t framesLeft = (recdata->getMaxSize().getSamples()* recdata->getMaxSize().getChannels()) -
                            (recdata->size().getSamples()      * recdata->size().getChannels());

        YARP_UNUSED(outputBuffer); // just to prevent unused variable warnings
        YARP_UNUSED(timeInfo);
        YARP_UNUSED(statusFlags);
        YARP_UNUSED(userData);

        if( framesLeft/ num_rec_channels < framesPerBuffer )
        {
            framesToCalc = framesLeft/ num_rec_channels;
#ifdef STOP_REC_ON_EMPTY_BUFFER
            //if we return paComplete, then the callback is not called anymore.
            //method Pa_IsStreamActive() will return 1.
            //user needs to call Pa_StopStream() before starting a new recording session
            finished = paComplete;
#else
            finished = paContinue;
#endif
        }
        else
        {
            framesToCalc = framesPerBuffer;
            //if we return paContinue, then the callback will be invoked again later
            //method Pa_IsStreamActive() will return 0
            finished = paContinue;
        }

        if( inputBuffer == nullptr )
        {
            for( i=0; i<framesToCalc; i++ )
            {
                recdata->write(0); // left
                if(num_rec_channels == 2 ) recdata->write(0);  // right
            }
        }
        else
        {
#if 0
            yCDebug(PORTAUDIORECORDER) << "Writing" << framesToCalc*2*2 << "bytes in the circular buffer";
#endif
            for( i=0; i<framesToCalc; i++ )
            {
                recdata->write(*rptr++);  // left
                if(num_rec_channels == 2 ) recdata->write(*rptr++);  // right
            }
        }
        return finished;
    }

    yCError(PORTAUDIORECORDER, "No write operations requested, aborting");
    return paAbort;
}

PortAudioRecorderDeviceDriver::PortAudioRecorderDeviceDriver() :
    m_stream(nullptr),
    m_err(paNoError),
    m_system_resource(nullptr)
{
    memset(&m_inputParameters, 0, sizeof(PaStreamParameters));
}

PortAudioRecorderDeviceDriver::~PortAudioRecorderDeviceDriver()
{
    close();
}


bool PortAudioRecorderDeviceDriver::open(yarp::os::Searchable& config)
{
    m_audiorecorder_cfg.frequency = config.check("rate",Value(0),"audio sample rate (0=automatic)").asInt32();
    m_audiorecorder_cfg.numSamples = config.check("samples",Value(0),"number of samples per network packet (0=automatic). For chunks of 1 second of recording set samples=rate. Channels number is handled internally.").asInt32();
    m_audiorecorder_cfg.numChannels = config.check("channels", Value(0), "number of audio channels (0=automatic, max is 2)").asInt32();
    m_device_id = config.check("id",Value(-1),"which portaudio index to use (-1=automatic)").asInt32();

    if (m_audiorecorder_cfg.numChannels == 0)  m_audiorecorder_cfg.numChannels = DEFAULT_NUM_CHANNELS;

    if (m_audiorecorder_cfg.frequency == 0)  m_audiorecorder_cfg.frequency = DEFAULT_SAMPLE_RATE;

    if (m_audiorecorder_cfg.numSamples == 0) m_audiorecorder_cfg.numSamples = m_audiorecorder_cfg.frequency; //  by default let's use chunks of 1 second

//     size_t debug_numRecBytes = m_config.cfg_samples * sizeof(SAMPLE) * m_config.cfg_recChannels;
    AudioBufferSize rec_buffer_size (m_audiorecorder_cfg.numSamples, m_audiorecorder_cfg.numChannels, sizeof(SAMPLE));
    if (m_inputBuffer ==nullptr)
        m_inputBuffer = new CircularAudioBuffer_16t("portatudio_rec", rec_buffer_size);

    m_err = Pa_Initialize();
    if(m_err != paNoError )
    {
        yCError(PORTAUDIORECORDER, "portaudio system failed to initialize");
        return false;
    }

    m_inputParameters.device = (m_device_id ==-1)?Pa_GetDefaultInputDevice(): m_device_id;
    yCInfo(PORTAUDIORECORDER, "Device number %d", m_inputParameters.device);
    m_inputParameters.channelCount = (int)m_audiorecorder_cfg.numChannels;
    m_inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    if ((Pa_GetDeviceInfo(m_inputParameters.device ))!=nullptr) {
        m_inputParameters.suggestedLatency = Pa_GetDeviceInfo(m_inputParameters.device )->defaultLowInputLatency;
    }
    m_inputParameters.hostApiSpecificStreamInfo = nullptr;

    m_err = Pa_OpenStream(
              &m_stream,
              &m_inputParameters,
              nullptr,
              m_audiorecorder_cfg.frequency,
              DEFAULT_FRAMES_PER_BUFFER,
              paClipOff,
              bufferIOCallback,
              m_inputBuffer);

    if(m_err != paNoError )
    {
        yCError(PORTAUDIORECORDER, "An error occurred while using the portaudio stream" );
        yCError(PORTAUDIORECORDER, "Error number: %d", m_err );
        yCError(PORTAUDIORECORDER, "Error message: %s", Pa_GetErrorText(m_err ) );
    }

    //start the thread
    bool ret = this->start();
    YARP_UNUSED(ret);

    return (m_err==paNoError);
}

/*
void recStreamThread::handleError(const PaError& err)
{
    Pa_Terminate();
    if( err != paNoError )
    {
        yCError(PORTAUDIORECORDER, "An error occurred while using the portaudio stream" );
        yCError(PORTAUDIORECORDER, "Error number: %d", err );
        yCError(PORTAUDIORECORDER, "Error message: %s", Pa_GetErrorText( err ) );
    }
}
*/

void PortAudioRecorderDeviceDriver::handleError()
{
    //Pa_Terminate();
    m_inputBuffer->clear();

    if(m_err != paNoError )
    {
        yCError(PORTAUDIORECORDER, "An error occurred while using the portaudio stream" );
        yCError(PORTAUDIORECORDER, "Error number: %d", m_err );
        yCError(PORTAUDIORECORDER, "Error message: %s", Pa_GetErrorText(m_err ) );
    }
}

bool PortAudioRecorderDeviceDriver::close()
{
    this->stop();
    if (m_stream != nullptr)
    {
        m_err = Pa_CloseStream(m_stream );
        if(m_err != paNoError )
        {
            yCError(PORTAUDIORECORDER,  "An error occurred while closing the portaudio stream" );
            yCError(PORTAUDIORECORDER,  "Error number: %d", m_err );
            yCError(PORTAUDIORECORDER,  "Error message: %s", Pa_GetErrorText(m_err ) );
        }
    }

    if (this->m_inputBuffer != nullptr)
    {
        delete this->m_inputBuffer;
        this->m_inputBuffer = nullptr;
    }

    return (m_err==paNoError);
}

bool PortAudioRecorderDeviceDriver::startRecording()
{
    AudioRecorderDeviceBase::startRecording();
    m_err = Pa_StartStream(m_stream );
    if(m_err < 0 ) {handleError(); return false;}
    yCInfo(PORTAUDIORECORDER) << "PortAudioRecorderDeviceDriver started recording";
    return true;
}

bool PortAudioRecorderDeviceDriver::stopRecording()
{
    AudioRecorderDeviceBase::stopRecording();
    m_err = Pa_StopStream(m_stream );
    if(m_err < 0 ) {handleError(); return false;}
    yCInfo(PORTAUDIORECORDER) << "PortAudioRecorderDeviceDriver stopped recording";
    return true;
}

#if 0
bool PortAudioRecorderDeviceDriver::getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s)
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
                yCInfo(PORTAUDIORECORDER) << "getSound() is currently waiting. Use ::startRecording() to start the audio stream";
                debug_time = yarp::os::Time::now();
            }
            yarp::os::SystemClock::delaySystem(SLEEP_TIME);
        }
    #endif
    }

    //prevents simultaneous start/stop/reset etc.
    //std::lock_guard<std::mutex> lock(m_mutex);  //This must be used carefully

    //check on input parameters
    if (max_number_of_samples < min_number_of_samples)
    {
        yCError(PORTAUDIORECORDER) << "max_number_of_samples must be greater than min_number_of_samples!";
        return false;
    }
    if (max_number_of_samples > this->m_config.cfg_samples)
    {
        yCWarning(PORTAUDIORECORDER) << "max_number_of_samples bigger than the internal audio buffer! It will be truncated to:" << this->m_config.cfg_samples;
        max_number_of_samples = this->m_config.cfg_samples;
    }

    //wait until the desired number of samples are obtained
    size_t buff_size = 0;
    double start_time = yarp::os::Time::now();
    double debug_time = yarp::os::Time::now();
    do
    {
         buff_size = m_recDataBuffer->size().getSamples();
         if (buff_size >= max_number_of_samples) { break; }
         if (buff_size >= min_number_of_samples && yarp::os::Time::now() - start_time > max_samples_timeout_s) { break; }
         if (m_isRecording == false) { break; }

         if (yarp::os::Time::now() - debug_time > 1.0)
         {
             debug_time = yarp::os::Time::now();
             yCDebug(PORTAUDIORECORDER) << "PortAudioRecorderDeviceDriver::getSound() Buffer size is " << buff_size << "/" << max_number_of_samples << " after 1s";
         }

         yarp::os::SystemClock::delaySystem(SLEEP_TIME);
    }
    while (true);

    //prepare the sound data struct
    size_t samples_to_be_copied = buff_size;
    if (samples_to_be_copied > max_number_of_samples) samples_to_be_copied = max_number_of_samples;
    if (sound.getChannels()!=this->m_config.cfg_recChannels && sound.getSamples() != samples_to_be_copied)
    {
        sound.resize(samples_to_be_copied, this->m_config.cfg_recChannels);
    }
    sound.setFrequency(this->m_config.cfg_rate);

    //fill the sound data struct, reading samples from the circular buffer
    for (size_t i=0; i< samples_to_be_copied; i++)
        for (size_t j=0; j<this->m_config.cfg_recChannels; j++)
            {
                SAMPLE s = m_recDataBuffer->read();
                sound.set(s,i,j);
            }
    return true;
}
#endif

void PortAudioRecorderDeviceDriver::threadRelease()
{
}

bool PortAudioRecorderDeviceDriver::threadInit()
{
    m_isRecording=false;
    return true;
}

void PortAudioRecorderDeviceDriver::run()
{
    while(this->isStopping()==false)
    {
        if (m_isRecording)
        {
            while( ( m_err = Pa_IsStreamActive(m_stream) ) == 1 )
            {
                yarp::os::SystemClock::delaySystem(SLEEP_TIME);
            }

            if (m_err == 0)
            {
                Pa_StopStream(m_stream);
                yCDebug(PORTAUDIORECORDER) << "The recording stream has been stopped";
                m_isRecording = false;
            }
            if(m_err < 0 )
            {
                handleError();
                return;
            }
        }

        yarp::os::Time::delay(SLEEP_TIME);
    }
    return;
}
