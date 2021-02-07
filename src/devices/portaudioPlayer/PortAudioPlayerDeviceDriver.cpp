/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

#include "PortAudioPlayerDeviceDriver.h"

#include <cstdlib>
#include <cstring>
#include <portaudio.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/api.h>

#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>

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

#define DEFAULT_FRAMES_PER_BUFFER (512)

namespace {
YARP_LOG_COMPONENT(PORTAUDIOPLAYER, "yarp.devices.portaudioPlayer")
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
    CircularAudioBuffer_16t *playdata = static_cast<CircularAudioBuffer_16t*>(userData);
    size_t num_play_channels = playdata->getMaxSize().getChannels();
    int finished = paComplete;

    if (1)
    {
        auto* wptr = (SAMPLE*)outputBuffer;
        unsigned int i;

        size_t framesLeft = playdata->size().getSamples()* playdata->size().getChannels();

        YARP_UNUSED(inputBuffer);
        YARP_UNUSED(timeInfo);
        YARP_UNUSED(statusFlags);
        YARP_UNUSED(userData);

        if( framesLeft/ num_play_channels < framesPerBuffer )
        {
            // final buffer
            for( i=0; i<framesLeft/ num_play_channels; i++ )
            {
                *wptr++ = playdata->read();  // left
                if( num_play_channels == 2 ) *wptr++ = playdata->read();  // right
                for (size_t chs=2; chs<num_play_channels; chs++) playdata->read(); //remove all additional channels > 2
            }
            for( ; i<framesPerBuffer; i++ )
            {
                *wptr++ = 0;  // left
                if(num_play_channels == 2 ) *wptr++ = 0;  // right
            }
#ifdef STOP_PLAY_ON_EMPTY_BUFFER
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
#if 0
            yCDebug(PORTAUDIOPLAYER) << "Reading" << framesPerBuffer*2 << "bytes from the circular buffer";
#endif
            for( i=0; i<framesPerBuffer; i++ )
            {
                *wptr++ = playdata->read();  // left
                if( num_play_channels == 2 ) *wptr++ = playdata->read();  // right
                for (size_t chs=2; chs<num_play_channels; chs++) playdata->read(); //remove all additional channels > 2
            }
            //if we return paContinue, then the callback will be invoked again later
            //method Pa_IsStreamActive() will return 0
            finished = paContinue;
        }
        return finished;
    }

    yCError(PORTAUDIOPLAYER, "No read operations requested, aborting");
    return paAbort;
}

/*
PlayStreamThread::PlayStreamThread() :
        something_to_play(false),
        stream(nullptr),
        err(paNoError)
{
}*/

void PortAudioPlayerDeviceDriver::threadRelease()
{
}

bool PortAudioPlayerDeviceDriver::threadInit()
{

    return true;
}

void PortAudioPlayerDeviceDriver::run()
{
    while(this->isStopping()==false)
    {
        if(m_something_to_play)
        {
            m_something_to_play = false;
            m_err = Pa_StartStream(m_stream);
            if(m_err != paNoError ) {handleError(); return;}

            while( (m_err = Pa_IsStreamActive(m_stream) ) == 1 )
            {
                yarp::os::SystemClock::delaySystem(SLEEP_TIME);
            }
            if (m_err == 0)
            {
                yCDebug(PORTAUDIOPLAYER) << "The playback stream has been stopped";
            }
            if(m_err < 0 )
            {
                handleError();
                return;
            }

            m_err = Pa_StopStream(m_stream);
            //err = Pa_AbortStream( stream );
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

PortAudioPlayerDeviceDriver::PortAudioPlayerDeviceDriver() :
    m_stream(nullptr),
    m_err(paNoError),
    m_system_resource(nullptr)
{
    memset(&m_outputParameters, 0, sizeof(PaStreamParameters));
}

PortAudioPlayerDeviceDriver::~PortAudioPlayerDeviceDriver()
{
    close();
}

bool PortAudioPlayerDeviceDriver::interruptDeviceAndClose()
{
    this->stop();
    if (m_stream != nullptr)
    {
        m_err = Pa_CloseStream(m_stream);
        if (m_err != paNoError)
        {
            yCError(PORTAUDIOPLAYER, "An error occurred while closing the portaudio stream");
            yCError(PORTAUDIOPLAYER, "Error number: %d", m_err);
            yCError(PORTAUDIOPLAYER, "Error message: %s", Pa_GetErrorText(m_err));
        }
    }

    if (this->m_outputBuffer != nullptr)
    {
        delete this->m_outputBuffer;
        this->m_outputBuffer = nullptr;
    }

    return (m_err == paNoError);
}

bool PortAudioPlayerDeviceDriver::configureDeviceAndStart()
{
//     size_t debug_numPlayBytes = (m_config.cfg_samples * sizeof(SAMPLE) * m_config.cfg_playChannels);
    AudioBufferSize playback_buffer_size(m_audioplayer_cfg.numSamples, m_audioplayer_cfg.numChannels, sizeof(SAMPLE));
    if (m_outputBuffer == nullptr)
        m_outputBuffer = new CircularAudioBuffer_16t("portatudio_play", playback_buffer_size);

    m_err = Pa_Initialize();
    if (m_err != paNoError)
    {
        yCError(PORTAUDIOPLAYER, "portaudio system failed to initialize");
        return false;
    }

    m_outputParameters.device = (m_device_id == -1) ? Pa_GetDefaultOutputDevice() : m_device_id;
    m_outputParameters.channelCount = m_audioplayer_cfg.numChannels;
    m_outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = nullptr;

    m_err = Pa_OpenStream(
        &m_stream,
        nullptr,
        &m_outputParameters,
        m_audioplayer_cfg.frequency,
        DEFAULT_FRAMES_PER_BUFFER,
        paClipOff,
        bufferIOCallback,
        m_outputBuffer);

    if (m_err != paNoError)
    {
        yCError(PORTAUDIOPLAYER, "An error occurred while using the portaudio stream");
        yCError(PORTAUDIOPLAYER, "Error number: %d", m_err);
        yCError(PORTAUDIOPLAYER, "Error message: %s", Pa_GetErrorText(m_err));
    }

    //start the thread
    this->start();

    return true;
}

bool PortAudioPlayerDeviceDriver::open(yarp::os::Searchable& config)
{
    bool b = configurePlayerAudioDevice(config);
    if (!b) { return false; }

    m_device_id = config.check("id", Value(-1), "which portaudio index to use (-1=automatic)").asInt32();

    b = configureDeviceAndStart();
    return (m_err==paNoError);
}

void PortAudioPlayerDeviceDriver::handleError()
{
    //Pa_Terminate();
    m_outputBuffer->clear();

    if(m_err != paNoError )
    {
        yCError(PORTAUDIOPLAYER,  "An error occurred while using the portaudio stream" );
        yCError(PORTAUDIOPLAYER,  "Error number: %d", m_err );
        yCError(PORTAUDIOPLAYER,  "Error message: %s", Pa_GetErrorText(m_err ) );
    }
}

bool PortAudioPlayerDeviceDriver::close()
{
    return interruptDeviceAndClose();
}

bool PortAudioPlayerDeviceDriver::abortSound()
{
    yCInfo(PORTAUDIOPLAYER, "=== Stopping and clearing stream.==="); fflush(stdout);
    m_err = Pa_StopStream(m_stream );
    if(m_err != paNoError )
    {
        yCError(PORTAUDIOPLAYER, "abortSound: error occurred while stopping the portaudio stream" );
        yCError(PORTAUDIOPLAYER, "Error number: %d", m_err );
        yCError(PORTAUDIOPLAYER, "Error message: %s", Pa_GetErrorText(m_err ) );
    }

    m_outputBuffer->clear();

    return (m_err==paNoError);
}

bool PortAudioPlayerDeviceDriver::setHWGain(double gain)
{
    yCError(PORTAUDIOPLAYER, "Not yet implemented");
    return false;
}

void PortAudioPlayerDeviceDriver::waitUntilPlaybackStreamIsComplete()
{
    while (Pa_IsStreamStopped(m_stream) == 0)
    {
        yarp::os::Time::delay(SLEEP_TIME);
    }
}
