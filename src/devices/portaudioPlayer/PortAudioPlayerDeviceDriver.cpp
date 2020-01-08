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

#include "PortAudioPlayerDeviceDriver.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <portaudio.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/api.h>

#include <yarp/os/Time.h>
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

        (void) inputBuffer; // just to prevent unused variable warnings
        (void) timeInfo;
        (void) statusFlags;
        (void) userData;

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
            yDebug() << "Reading" << framesPerBuffer*2 << "bytes from the circular buffer";
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

    printf("No read operations requested, aborting\n");
    return paAbort;
}

PlayStreamThread::PlayStreamThread() :
        something_to_play(false),
        stream(nullptr),
        err(paNoError)
{
}

void PlayStreamThread::threadRelease()
{
}

bool PlayStreamThread::threadInit()
{

    return true;
}

void PlayStreamThread::run()
{
    while(this->isStopping()==false)
    {
        if( something_to_play )
        {
            something_to_play = false;
            err = Pa_StartStream( stream );
            if( err != paNoError ) {handleError(); return;}

            while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
            {
                yarp::os::SystemClock::delaySystem(SLEEP_TIME);
            }
            if (err == 0)
            {
                yDebug() << "The playback stream has been stopped";
            }
            if( err < 0 )
            {
                handleError();
                return;
            }

            err = Pa_StopStream( stream );
            //err = Pa_AbortStream( stream );
            if( err < 0 )
            {
                handleError();
                return;
            }

        }

        yarp::os::Time::delay(SLEEP_TIME);
    }
    return;
}

void PlayStreamThread::handleError()
{
    Pa_Terminate();
    if( err != paNoError )
    {
        yError( "An error occurred while using the portaudio stream\n" );
        yError( "Error number: %d\n", err );
        yError( "Error message: %s\n", Pa_GetErrorText( err ) );
    }
}

PortAudioPlayerDeviceDriver::PortAudioPlayerDeviceDriver() :
    m_stream(nullptr),
    m_err(paNoError),
    m_playDataBuffer(nullptr),
    m_system_resource(nullptr),
    renderMode(RENDER_APPEND)
{
    memset(&m_outputParameters, 0, sizeof(PaStreamParameters));
}

PortAudioPlayerDeviceDriver::~PortAudioPlayerDeviceDriver()
{
    close();
}


bool PortAudioPlayerDeviceDriver::open(yarp::os::Searchable& config)
{
    m_driverConfig.cfg_rate = config.check("rate",Value(0),"audio sample rate (0=automatic)").asInt32();
    m_driverConfig.cfg_samples = config.check("samples",Value(0),"number of samples per network packet (0=automatic). For chunks of 1 second of recording set samples=rate. Channels number is handled internally.").asInt32();
    m_driverConfig.cfg_playChannels = config.check("channels",Value(0),"number of audio channels (0=automatic, max is 2)").asInt32();
    m_driverConfig.cfg_deviceNumber = config.check("id",Value(-1),"which portaudio index to use (-1=automatic)").asInt32();

    if (config.check("render_mode_append"))
    {
        renderMode = RENDER_APPEND;
    }
    if (config.check("render_mode_immediate"))
    {
        renderMode = RENDER_IMMEDIATE;
    }

    return open(m_driverConfig);
}

bool PortAudioPlayerDeviceDriver::open(PortAudioPlayerDeviceDriverSettings& config)
{
    m_config = config;

    if (m_config.cfg_playChannels==0)  m_config.cfg_playChannels = DEFAULT_NUM_CHANNELS;

    if (m_config.cfg_rate==0)  m_config.cfg_rate = DEFAULT_SAMPLE_RATE;

    if (m_config.cfg_samples==0) m_config.cfg_samples = m_config.cfg_rate; //  by default let's use chunks of 1 second

//     size_t debug_numPlayBytes = (m_config.cfg_samples * sizeof(SAMPLE) * m_config.cfg_playChannels);
    AudioBufferSize playback_buffer_size(m_config.cfg_samples, m_config.cfg_playChannels, sizeof(SAMPLE));
    if (m_playDataBuffer ==nullptr)
        m_playDataBuffer = new CircularAudioBuffer_16t("portatudio_play", playback_buffer_size);

    m_err = Pa_Initialize();
    if(m_err != paNoError )
    {
        yError("portaudio system failed to initialize");
        return false;
    }

    m_outputParameters.device = (config.cfg_deviceNumber ==-1)?Pa_GetDefaultOutputDevice(): config.cfg_deviceNumber;
    m_outputParameters.channelCount = m_config.cfg_playChannels;
    m_outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device )->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = nullptr;

    m_err = Pa_OpenStream(
              &m_stream,
              nullptr,
              &m_outputParameters,
              m_config.cfg_rate,
              DEFAULT_FRAMES_PER_BUFFER,
              paClipOff,
              bufferIOCallback,
              m_playDataBuffer);

    if(m_err != paNoError )
    {
        yError("An error occurred while using the portaudio stream\n" );
        yError("Error number: %d\n", m_err );
        yError("Error message: %s\n", Pa_GetErrorText(m_err ) );
    }

    //start the thread
    m_pThread.stream = m_stream;
    m_pThread.start();

    return (m_err==paNoError);
}

void PortAudioPlayerDeviceDriver::handleError()
{
    //Pa_Terminate();
    m_playDataBuffer->clear();

    if(m_err != paNoError )
    {
        yError( "An error occurred while using the portaudio stream\n" );
        yError( "Error number: %d\n", m_err );
        yError( "Error message: %s\n", Pa_GetErrorText(m_err ) );
    }
}

bool PortAudioPlayerDeviceDriver::close()
{
    m_pThread.stop();
    if (m_stream != nullptr)
    {
        m_err = Pa_CloseStream(m_stream );
        if(m_err != paNoError )
        {
            yError( "An error occurred while closing the portaudio stream\n" );
            yError( "Error number: %d\n", m_err );
            yError( "Error message: %s\n", Pa_GetErrorText(m_err ) );
        }
    }

    if (this->m_playDataBuffer != nullptr)
    {
        delete this->m_playDataBuffer;
        this->m_playDataBuffer = nullptr;
    }

    return (m_err==paNoError);
}

bool PortAudioPlayerDeviceDriver::abortSound()
{
    yInfo("=== Stopping and clearing stream.===\n"); fflush(stdout);
    m_err = Pa_StopStream(m_stream );
    if(m_err != paNoError )
    {
        yError("abortSound: error occurred while stopping the portaudio stream\n" );
        yError("Error number: %d\n", m_err );
        yError("Error message: %s\n", Pa_GetErrorText(m_err ) );
    }

    m_playDataBuffer->clear();

    return (m_err==paNoError);
}

bool PortAudioPlayerDeviceDriver::immediateSound(const yarp::sig::Sound& sound)
{
    m_playDataBuffer->clear();

//     size_t num_bytes = sound.getBytesPerSample();
    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i=0; i<num_samples; i++)
        for (size_t j=0; j<num_channels; j++)
            m_playDataBuffer->write (sound.get(i,j));

    m_pThread.something_to_play = true;
    return true;
}

bool PortAudioPlayerDeviceDriver::renderSound(const yarp::sig::Sound& sound)
{
    //prevents simultaneous start/stop/reset etc.
    std::lock_guard<std::mutex> lock(m_mutex);

    size_t freq  = sound.getFrequency();
    size_t chans = sound.getChannels();
    if (freq == 0)
    {
        yError() << "received a bad audio sample of frequency 0";
        return false;
    }
    if (chans == 0)
    {
        yError() << "received a bad audio sample with 0 channels";
        return false;
    }

    if (freq  != this->m_config.cfg_rate ||
        chans != this->m_config.cfg_playChannels)
    {
        //wait for current playback to finish
        while (Pa_IsStreamStopped(m_stream )==0)
        {
            yarp::os::Time::delay(SLEEP_TIME);
        }

        //reset the driver
        yInfo("***** audio driver configuration changed, resetting");
        yInfo() << "changing from: " << this->m_config.cfg_playChannels << "channels, " <<  this->m_config.cfg_rate << " Hz, ->" <<
                                                    chans << "channels, " <<  freq << " Hz";
        this->close();
        m_driverConfig.cfg_playChannels = (int)(chans);
        m_driverConfig.cfg_rate = (int)(freq);
        bool ok = open(m_driverConfig);
        if (ok == false)
        {
            yError("error occurred during audio driver reconfiguration, aborting");
            return false;
        }
    }

    if (renderMode == RENDER_IMMEDIATE)
        return immediateSound(sound);
    else if (renderMode == RENDER_APPEND)
        return appendSound(sound);

    return false;
}

bool PortAudioPlayerDeviceDriver::appendSound(const yarp::sig::Sound& sound)
{
//     size_t num_bytes = sound.getBytesPerSample();
    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i=0; i<num_samples; i++)
        for (size_t j=0; j<num_channels; j++)
            m_playDataBuffer->write (sound.get(i,j));

    m_pThread.something_to_play = true;
    return true;
}

bool PortAudioPlayerDeviceDriver::getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    size = this->m_playDataBuffer->size();
    return true;
}

bool PortAudioPlayerDeviceDriver::getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    //no lock guard is needed here
    size = this->m_playDataBuffer->getMaxSize();
    return true;
}

bool PortAudioPlayerDeviceDriver::resetPlaybackAudioBuffer()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    this->m_playDataBuffer->clear();
    return true;
}

bool PortAudioPlayerDeviceDriver::startPlayback()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pThread.something_to_play = true;
    return true;
}

bool PortAudioPlayerDeviceDriver::stopPlayback()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pThread.something_to_play = false;
    return true;
}
