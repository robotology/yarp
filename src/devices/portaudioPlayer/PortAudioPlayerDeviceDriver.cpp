/*
 * Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
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

#include <PortAudioPlayerDeviceDriver.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <portaudio.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>

#include <yarp/os/Time.h>
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
    auto* dataBuffers = static_cast<circularDataBuffers*>(userData);
    CircularAudioBuffer_16t *playdata = dataBuffers->playData;
    int num_play_channels    = dataBuffers->numPlayChannels;
    int finished = paComplete;

    if (dataBuffers->canPlay)
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
        //note: you can record or play but not simultaneously (for now)
        return finished;
    }

    printf("No read/write operations requested, aborting\n");
    return paAbort;
}

PortAudioPlayerDeviceDriver::PortAudioPlayerDeviceDriver() :
    stream(nullptr),
    err(paNoError),
    numSamples(0),
    numBytes(0),
    m_system_resource(nullptr),
    m_numPlaybackChannels(0),
    m_frequency(0),
    m_getSoundIsNotBlocking(true),
    renderMode(RENDER_APPEND)
{
    memset(&outputParameters, 0, sizeof(PaStreamParameters));
    memset(&dataBuffers, 0, sizeof(circularDataBuffers));
    memset(&m_driverConfig, 0, sizeof(PortAudioPlayerDeviceDriverSettings));
}

PortAudioPlayerDeviceDriver::~PortAudioPlayerDeviceDriver()
{
    close();
}


bool PortAudioPlayerDeviceDriver::open(yarp::os::Searchable& config)
{
    m_driverConfig.rate = config.check("rate",Value(0),"audio sample rate (0=automatic)").asInt32();
    m_driverConfig.samples = config.check("samples",Value(0),"number of samples per network packet (0=automatic). For chunks of 1 second of recording set samples=rate. Channels number is handled internally.").asInt32();
    m_driverConfig.playChannels = config.check("channels",Value(0),"number of audio channels (0=automatic, max is 2)").asInt32();
    m_driverConfig.deviceNumber = config.check("id",Value(-1),"which portaudio index to use (-1=automatic)").asInt32();

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
    int rate = config.rate;
    int samples = config.samples;
    int playChannels = config.playChannels;
    int deviceNumber = config.deviceNumber;

    if (playChannels==0) playChannels = DEFAULT_NUM_CHANNELS;
    m_numPlaybackChannels = playChannels;

    if (rate==0) rate = DEFAULT_SAMPLE_RATE;
    m_frequency = rate;

    if (samples==0)
        numSamples = m_frequency; //  by default let's stream chunks of 1 second
    else
        numSamples = samples;

    size_t numPlayBytes = numSamples * sizeof(SAMPLE) * m_numPlaybackChannels;
    int twiceTheBuffer = 2;
    AudioBufferSize playback_buffer_size(numSamples, m_numPlaybackChannels, sizeof(SAMPLE));
    dataBuffers.numPlayChannels = m_numPlaybackChannels;
    if (dataBuffers.playData==nullptr)
        dataBuffers.playData = new CircularAudioBuffer_16t("portatudio_play", playback_buffer_size);
    dataBuffers.canPlay = true;

    err = Pa_Initialize();
    if( err != paNoError )
    {
        yError("portaudio system failed to initialize");
        return false;
    }

    outputParameters.device = (deviceNumber==-1)?Pa_GetDefaultOutputDevice():deviceNumber;
    outputParameters.channelCount = m_numPlaybackChannels;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
              &stream,
              nullptr,
              &outputParameters,
              m_frequency,
              DEFAULT_FRAMES_PER_BUFFER,
              paClipOff,
              bufferIOCallback,
              &dataBuffers );

    if( err != paNoError )
    {
        fprintf( stderr, "An error occurred while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    }

    //start the thread
    pThread.stream = stream;
    pThread.start();

    return (err==paNoError);
}

void playStreamThread::handleError()
{
    Pa_Terminate();
    if( err != paNoError )
    {
        fprintf( stderr, "An error occurred while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    }
}

void PortAudioPlayerDeviceDriver::handleError()
{
    //Pa_Terminate();
    dataBuffers.playData->clear();

    if( err != paNoError )
    {
        fprintf( stderr, "An error occurred while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    }
}

bool PortAudioPlayerDeviceDriver::close()
{
    pThread.stop();
    if (stream != nullptr)
    {
        err = Pa_CloseStream( stream );
        if( err != paNoError )
        {
            fprintf( stderr, "An error occurred while closing the portaudio stream\n" );
            fprintf( stderr, "Error number: %d\n", err );
            fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        }
    }

    if (this->dataBuffers.playData != nullptr)
    {
        delete this->dataBuffers.playData;
        this->dataBuffers.playData = nullptr;
    }
    if (this->dataBuffers.recData != nullptr)
    {
        delete this->dataBuffers.recData;
        this->dataBuffers.recData = nullptr;
    }

    return (err==paNoError);
}

bool PortAudioPlayerDeviceDriver::abortSound()
{
    printf("\n=== Stopping and clearing stream.===\n"); fflush(stdout);
    err = Pa_StopStream( stream );
    if( err != paNoError )
    {
        fprintf( stderr, "abortSound: error occurred while stopping the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    }

    dataBuffers.playData->clear();

    return (err==paNoError);
}

void playStreamThread::threadRelease()
{
}

bool playStreamThread::threadInit()
{
    something_to_play=false;
    err = paNoError;
    return true;
}

void playStreamThread::run()
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

bool PortAudioPlayerDeviceDriver::immediateSound(const yarp::sig::Sound& sound)
{
    dataBuffers.playData->clear();

    size_t num_bytes = sound.getBytesPerSample();
    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i=0; i<num_samples; i++)
        for (size_t j=0; j<num_channels; j++)
            dataBuffers.playData->write (sound.get(i,j));

    pThread.something_to_play = true;
    return true;
}

bool PortAudioPlayerDeviceDriver::renderSound(const yarp::sig::Sound& sound)
{
    int freq  = sound.getFrequency();
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

    if (freq  != this->m_frequency ||
        chans != this->m_numPlaybackChannels)
    {
        //wait for current playback to finish
        while (Pa_IsStreamStopped(stream )==0)
        {
            yarp::os::Time::delay(SLEEP_TIME);
        }

        //reset the driver
        yInfo("***** audio driver configuration changed, resetting");
        yInfo() << "changing from: " << this->m_numPlaybackChannels << "channels, " <<  this->m_frequency << " Hz, ->" <<
                                                    chans << "channels, " <<  freq << " Hz";
        this->close();
        m_driverConfig.playChannels = (int)(chans);
        m_driverConfig.rate = (int)(freq);
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
    size_t num_bytes = sound.getBytesPerSample();
    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i=0; i<num_samples; i++)
        for (size_t j=0; j<num_channels; j++)
            dataBuffers.playData->write (sound.get(i,j));

    pThread.something_to_play = true;
    return true;
}

bool PortAudioPlayerDeviceDriver::getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    size = this->dataBuffers.playData->size();
    return true;
}

bool PortAudioPlayerDeviceDriver::getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    size = this->dataBuffers.playData->getMaxSize();
    return true;
}

bool PortAudioPlayerDeviceDriver::resetPlaybackAudioBuffer()
{
    this->dataBuffers.playData->clear();
    return true;
}

bool PortAudioPlayerDeviceDriver::startPlayback()
{
    pThread.something_to_play = true;
    return true;
}

bool PortAudioPlayerDeviceDriver::stopPlayback()
{
    pThread.something_to_play = false;
    return true;
}
