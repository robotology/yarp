/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

#define SLEEP_TIME 0.010f

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
        //The status of the buffer (i.e. the return value of Pa_IsStreamActive() depends on the returned value
        //of the callback function bufferIOCallback() which may return paContinue or paComplete.
        if(m_playback_enabled)
        {
            m_err = Pa_IsStreamActive(m_stream);
            if (m_err < 0)
            {
                handleError();
                yCError(PORTAUDIOPLAYER) << "Unhandled error. Calling abortSound()";
                abortSound();
                continue;
            }
            if (m_err == 1)
            {
                //already playing something
            }
            else if (m_err == 0)
            {
                //the playback is stopped
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
    AudioBufferSize playback_buffer_size(m_audioplayer_cfg.numSamples, m_audioplayer_cfg.numChannels, m_audioplayer_cfg.bytesPerSample);
    if (m_outputBuffer == nullptr)
        m_outputBuffer = new CircularAudioBuffer_16t("portatudio_play", playback_buffer_size);

    m_err = Pa_Initialize();
    if (m_err != paNoError)
    {
        yCError(PORTAUDIOPLAYER, "portaudio system failed to initialize");
        return false;
    }

    m_outputParameters.device = (m_device_id == -1) ? Pa_GetDefaultOutputDevice() : m_device_id;
    m_outputParameters.channelCount = static_cast<int>(m_audioplayer_cfg.numChannels);
    m_outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = nullptr;

    m_err = Pa_OpenStream(
        &m_stream,
        nullptr,
        &m_outputParameters,
        (double)(m_audioplayer_cfg.frequency),
        m_driver_frame_size,
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
    if (config.check("help"))
    {
        yCInfo(PORTAUDIOPLAYER, "Some examples:");
        yCInfo(PORTAUDIOPLAYER, "yarpdev --device portaudioPlayer --help");
        yCInfo(PORTAUDIOPLAYER, "yarpdev --device AudioPlayerWrapper --subdevice portaudioPlayer --start");
        return false;
    }

    bool b = configurePlayerAudioDevice(config.findGroup("AUDIO_BASE"), "portaudioPlayer");
    if (!b) { return false; }

    m_device_id = config.check("id", Value(-1), "which portaudio index to use (-1=automatic)").asInt32();
    m_driver_frame_size = config.check("driver_frame_size", Value(0), "").asInt32();
    if (m_driver_frame_size == 0)  m_driver_frame_size = DEFAULT_FRAMES_PER_BUFFER;

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
        size_t tmp = m_outputBuffer->size().getSamples();
        if (tmp == 0) break;
    }
}

bool PortAudioPlayerDeviceDriver::startPlayback()
{
    AudioPlayerDeviceBase::startPlayback();
    m_err = Pa_StartStream(m_stream);
    if (m_err < 0) { handleError(); return false; }
    yCInfo(PORTAUDIOPLAYER) << "started playback";
    return true;
}

bool PortAudioPlayerDeviceDriver::stopPlayback()
{
    AudioPlayerDeviceBase::stopPlayback();
    m_err = Pa_StopStream(m_stream);
    if (m_err < 0) { handleError(); return false; }
    yCInfo(PORTAUDIOPLAYER) << "stopped playback";
    return true;
}
