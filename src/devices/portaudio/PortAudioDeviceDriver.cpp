/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "PortAudioDeviceDriver.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <portaudio.h>
#include <yarp/dev/DeviceDriver.h>
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
YARP_LOG_COMPONENT(PORTAUDIO, "yarp.devices.portaudio")
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
    auto* dataBuffers = static_cast<circularDataBuffers*>(userData);
    CircularAudioBuffer_16t *playdata = dataBuffers->playData;
    CircularAudioBuffer_16t *recdata  = dataBuffers->recData;
    int num_rec_channels     = dataBuffers->numRecChannels;
    int num_play_channels    = dataBuffers->numPlayChannels;
    int finished = paComplete;

    if (dataBuffers->canRec)
    {
        const auto* rptr = (const SAMPLE*)inputBuffer;
        unsigned int framesToCalc;
        unsigned int i;
        size_t framesLeft = (recdata->getMaxSize().getSamples()* recdata->getMaxSize().getChannels()) -
                            (recdata->size().getSamples()      * recdata->size().getChannels());

        YARP_UNUSED(outputBuffer);
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
            yCTrace(PORTAUDIO) << "Writing" << framesToCalc*2*2 << "bytes in the circular buffer";
            for( i=0; i<framesToCalc; i++ )
            {
                recdata->write(*rptr++);  // left
                if(num_rec_channels == 2 ) recdata->write(*rptr++);  // right
            }
        }
        //note: you can record or play but not simultaneously (for now)
        return finished;
    }

    if (dataBuffers->canPlay)
    {
        auto* wptr = (SAMPLE*)outputBuffer;
        unsigned int i;

        size_t framesLeft = playdata->size().getSamples()* playdata->size().getChannels();

        YARP_UNUSED(inputBuffer); // just to prevent unused variable warnings
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
                for (int chs=2; chs<num_play_channels; chs++) playdata->read(); //remove all additional channels > 2
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
#if 1
            yCDebug(PORTAUDIO) << "Reading" << framesPerBuffer*2 << "bytes from the circular buffer";
#endif
            for( i=0; i<framesPerBuffer; i++ )
            {
                *wptr++ = playdata->read();  // left
                if( num_play_channels == 2 ) *wptr++ = playdata->read();  // right
                for (int chs=2; chs<num_play_channels; chs++) playdata->read(); //remove all additional channels > 2
            }
            //if we return paContinue, then the callback will be invoked again later
            //method Pa_IsStreamActive() will return 0
            finished = paContinue;
        }
        //note: you can record or play but not simultaneously (for now)
        return finished;
    }

    yCError(PORTAUDIO, "No read/write operations requested, aborting");
    return paAbort;
}

PortAudioDeviceDriver::PortAudioDeviceDriver() :
    stream(nullptr),
    err(paNoError),
    numSamples(0),
    numBytes(0),
    m_system_resource(nullptr),
    m_numPlaybackChannels(0),
    m_numRecordChannels(0),
    m_frequency(0),
    m_loopBack(false),
    m_getSoundIsNotBlocking(true),
    renderMode(RENDER_APPEND)
{
    memset(&inputParameters, 0, sizeof(PaStreamParameters));
    memset(&outputParameters, 0, sizeof(PaStreamParameters));
    memset(&m_driverConfig, 0, sizeof(PortAudioDeviceDriverSettings));
}

PortAudioDeviceDriver::~PortAudioDeviceDriver()
{
    close();
}


bool PortAudioDeviceDriver::open(yarp::os::Searchable& config)
{
    m_driverConfig.rate = config.check("rate",Value(0),"audio sample rate (0=automatic)").asInt32();
    m_driverConfig.samples = config.check("samples",Value(0),"number of samples per network packet (0=automatic). For chunks of 1 second of recording set samples=rate. Channels number is handled internally.").asInt32();
    m_driverConfig.playChannels = config.check("channels",Value(0),"number of audio channels (0=automatic, max is 2)").asInt32();
    m_driverConfig.recChannels = config.check("channels", Value(0), "number of audio channels (0=automatic, max is 2)").asInt32();
    m_driverConfig.wantRead = (bool)config.check("read","if present, just deal with reading audio (microphone)");
    m_driverConfig.wantWrite = (bool)config.check("write","if present, just deal with writing audio (speaker)");
    m_driverConfig.deviceNumber = config.check("id",Value(-1),"which portaudio index to use (-1=automatic)").asInt32();

    if (!(m_driverConfig.wantRead|| m_driverConfig.wantWrite))
    {
        m_driverConfig.wantRead = m_driverConfig.wantWrite = true;
    }

    if (config.check("loopback","if present, send audio read from microphone immediately back to speaker"))
    {
        yCError(PORTAUDIO, "loopback not yet implemented");
        m_loopBack = true;
    }

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

bool PortAudioDeviceDriver::open(PortAudioDeviceDriverSettings& config)
{
    int rate = config.rate;
    int samples = config.samples;
    int playChannels = config.playChannels;
    int recChannels = config.recChannels;
    bool wantRead = config.wantRead;
    bool wantWrite = config.wantWrite;
    int deviceNumber = config.deviceNumber;

    if (playChannels==0) playChannels = DEFAULT_NUM_CHANNELS;
    if (recChannels == 0) recChannels = DEFAULT_NUM_CHANNELS;
    m_numPlaybackChannels = playChannels;
    m_numRecordChannels = recChannels;

    if (rate==0) rate = DEFAULT_SAMPLE_RATE;
    m_frequency = rate;

    if (samples==0)
        numSamples = m_frequency; //  by default let's stream chunks of 1 second
    else
        numSamples = samples;

//     size_t numPlayBytes = numSamples * sizeof(SAMPLE) * m_numPlaybackChannels;
//     size_t numRecBytes = numSamples * sizeof(SAMPLE) * m_numRecordChannels;
//     int twiceTheBuffer = 2;
    AudioBufferSize playback_buffer_size(numSamples, m_numPlaybackChannels, sizeof(SAMPLE));
    AudioBufferSize rec_buffer_size (numSamples, m_numRecordChannels, sizeof(SAMPLE));
    dataBuffers.numPlayChannels = m_numPlaybackChannels;
    dataBuffers.numRecChannels = m_numRecordChannels;
    if (dataBuffers.playData==nullptr)
        dataBuffers.playData = new CircularAudioBuffer_16t("portatudio_play", playback_buffer_size);
    if (dataBuffers.recData==nullptr)
        dataBuffers.recData = new CircularAudioBuffer_16t("portatudio_rec", rec_buffer_size);
    if (wantRead) dataBuffers.canRec = true;
    if (wantWrite) dataBuffers.canPlay = true;

    err = Pa_Initialize();
    if( err != paNoError )
    {
        yCError(PORTAUDIO, "portaudio system failed to initialize");
        return false;
    }

    inputParameters.device = (deviceNumber==-1)?Pa_GetDefaultInputDevice():deviceNumber;
    yCInfo(PORTAUDIO, "Device number %d", inputParameters.device);
    inputParameters.channelCount = m_numRecordChannels;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    if ((Pa_GetDeviceInfo( inputParameters.device ))!=nullptr) {
        inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    }
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    outputParameters.device = (deviceNumber==-1)?Pa_GetDefaultOutputDevice():deviceNumber;
    outputParameters.channelCount = m_numPlaybackChannels;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
              &stream,
              wantRead?(&inputParameters):nullptr,
              wantWrite?(&outputParameters):nullptr,
              m_frequency,
              DEFAULT_FRAMES_PER_BUFFER,
              paClipOff,
              bufferIOCallback,
              &dataBuffers );

    if( err != paNoError )
    {
        yCError(PORTAUDIO, "An error occurred while using the portaudio stream");
        yCError(PORTAUDIO, "Error number: %d", err);
        yCError(PORTAUDIO, "Error message: %s", Pa_GetErrorText(err));
    }

    //start the thread
    pThread.stream = stream;
    pThread.start();

    return (err==paNoError);
}

void streamThread::handleError()
{
    Pa_Terminate();
    if( err != paNoError )
    {
        yCError(PORTAUDIO, "An error occurred while using the portaudio stream");
        yCError(PORTAUDIO, "Error number: %d", err);
        yCError(PORTAUDIO, "Error message: %s\n", Pa_GetErrorText(err));
    }
}

void PortAudioDeviceDriver::handleError()
{
    //Pa_Terminate();
    dataBuffers.playData->clear();

    if( err != paNoError )
    {
        yCError(PORTAUDIO, "An error occurred while using the portaudio stream");
        yCError(PORTAUDIO, "Error number: %d", err);
        yCError(PORTAUDIO, "Error message: %s", Pa_GetErrorText(err));
    }
}

bool PortAudioDeviceDriver::close()
{
    pThread.stop();
    if (stream != nullptr)
    {
        err = Pa_CloseStream( stream );
        if( err != paNoError )
        {
            yCError(PORTAUDIO, "An error occurred while closing the portaudio stream");
            yCError(PORTAUDIO, "Error number: %d", err);
            yCError(PORTAUDIO, "Error message: %s", Pa_GetErrorText(err));
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

bool PortAudioDeviceDriver::startRecording()
{
    pThread.something_to_record = true;
    err = Pa_StartStream( stream );
    if( err < 0 ) {handleError(); return false;}
    return true;
}

bool PortAudioDeviceDriver::stopRecording()
{
    pThread.something_to_record = false;
    err = Pa_StopStream( stream );
    if( err < 0 ) {handleError(); return false;}
    return true;
}

bool PortAudioDeviceDriver::getSound(yarp::sig::Sound& sound, size_t min_number_of_samples, size_t max_number_of_samples, double max_samples_timeout_s)
{
    if (pThread.something_to_record == false)
    {
        this->startRecording();
    }

    size_t buff_size = 0;
    int buff_size_wdt = 0;
    do
    {
        buff_size = dataBuffers.recData->size().getSamples();

        if (buff_size_wdt > 100)
        {
            if (buff_size == 0)
            {
                yCError(PORTAUDIO) << "PortAudioDeviceDriver::getSound() Buffer size is still zero after 100 iterations, returning";
                return false;
            }
            else
            {
                yCDebug(PORTAUDIO) << "PortAudioDeviceDriver::getSound() Buffer size is " << buff_size << "/" << this->numSamples <<" after 100 iterations";
                if (m_getSoundIsNotBlocking)
                {
                    yCError(PORTAUDIO) << "PortAudioDeviceDriver::getSound() is in not-blocking mode, returning";
                    return false;
                }
            }
        }
        buff_size_wdt++;
        yarp::os::SystemClock::delaySystem(SLEEP_TIME);
    }
    while (buff_size < this->numSamples);

    buff_size_wdt = 0;

    if (sound.getChannels()!=this->m_numRecordChannels && sound.getSamples() != this->numSamples)
    {
        sound.resize(this->numSamples,this->m_numRecordChannels);
    }
    sound.setFrequency(this->m_frequency);

    for (size_t i=0; i<this->numSamples; i++)
        for (size_t j=0; j<this->m_numRecordChannels; j++)
        {
            SAMPLE s = dataBuffers.recData->read();
            sound.set(s,i,j);
        }
    return true;
}

bool PortAudioDeviceDriver::abortSound()
{
    yCInfo(PORTAUDIO, "=== Stopping and clearing stream.==="); fflush(stdout);
    err = Pa_StopStream( stream );
    if( err != paNoError )
    {
        yCError(PORTAUDIO, "abortSound: error occurred while stopping the portaudio stream" );
        yCError(PORTAUDIO, "Error number: %d", err );
        yCError(PORTAUDIO, "Error message: %s", Pa_GetErrorText( err ) );
    }

    dataBuffers.playData->clear();

    return (err==paNoError);
}

void streamThread::threadRelease()
{
}

bool streamThread::threadInit()
{
    something_to_play=false;
    something_to_record=false;
    err = paNoError;
    return true;
}

void streamThread::run()
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
                yCDebug(PORTAUDIO) << "The playback stream has been stopped";
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

        if (something_to_record)
        {
            while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
            {
                yarp::os::SystemClock::delaySystem(SLEEP_TIME);
            }
            if (err == 0)
            {
                Pa_StopStream(stream);
                yCDebug(PORTAUDIO) << "The recording stream has been stopped";
                something_to_record = false;
            }
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

bool PortAudioDeviceDriver::immediateSound(const yarp::sig::Sound& sound)
{
    dataBuffers.playData->clear();

//     size_t num_bytes = sound.getBytesPerSample();
    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i=0; i<num_samples; i++)
        for (size_t j=0; j<num_channels; j++)
            dataBuffers.playData->write (sound.get(i,j));

    pThread.something_to_play = true;
    return true;
}

bool PortAudioDeviceDriver::renderSound(const yarp::sig::Sound& sound)
{
    int freq  = sound.getFrequency();
    size_t chans = sound.getChannels();
    if (freq  != this->m_frequency ||
        chans != this->m_numPlaybackChannels)
    {
        //wait for current playback to finish
        while (Pa_IsStreamStopped(stream )==0)
        {
            yarp::os::Time::delay(SLEEP_TIME);
        }

        //reset the driver
        yCInfo(PORTAUDIO, "***** audio driver configuration changed, resetting");
        yCInfo(PORTAUDIO) << "changing from: " << this->m_numPlaybackChannels << "channels, " <<  this->m_frequency << " Hz, ->" <<
                                                    chans << "channels, " <<  freq << " Hz";
        this->close();
        m_driverConfig.playChannels = (int)(chans);
        m_driverConfig.rate = (int)(freq);
        bool ok = open(m_driverConfig);
        if (ok == false)
        {
            yCError(PORTAUDIO, "error occurred during audio driver reconfiguration, aborting");
            return false;
        }
    }

    if (renderMode == RENDER_IMMEDIATE)
        return immediateSound(sound);
    else if (renderMode == RENDER_APPEND)
        return appendSound(sound);

    return false;
}

bool PortAudioDeviceDriver::appendSound(const yarp::sig::Sound& sound)
{
//     size_t num_bytes = sound.getBytesPerSample();
    size_t num_channels = sound.getChannels();
    size_t num_samples = sound.getSamples();

    for (size_t i=0; i<num_samples; i++)
        for (size_t j=0; j<num_channels; j++)
            dataBuffers.playData->write (sound.get(i,j));

    pThread.something_to_play = true;
    return true;
}

bool PortAudioDeviceDriver::getPlaybackAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    size = this->dataBuffers.playData->size();
    return true;
}

bool PortAudioDeviceDriver::getPlaybackAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    size = this->dataBuffers.playData->getMaxSize();
    return true;
}

bool PortAudioDeviceDriver::resetPlaybackAudioBuffer()
{
    this->dataBuffers.playData->clear();
    return true;
}

bool PortAudioDeviceDriver::getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    size = this->dataBuffers.recData->size();
    return true;
}

bool PortAudioDeviceDriver::getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    size = this->dataBuffers.recData->getMaxSize();
    return true;
}

bool PortAudioDeviceDriver::resetRecordingAudioBuffer()
{
    this->dataBuffers.recData->clear();
    return true;
}

bool PortAudioDeviceDriver::startPlayback()
{
    pThread.something_to_play = true;
    return true;
}

bool PortAudioDeviceDriver::stopPlayback()
{
    pThread.something_to_play = false;
    return true;
}

bool PortAudioDeviceDriver::setSWGain( double gain)
{
    yCError(PORTAUDIO,"Not yet implemented");
    return false;
}

bool PortAudioDeviceDriver::setHWGain(double gain)
{
    yCError(PORTAUDIO, "Not yet implemented");
    return false;
}

bool PortAudioDeviceDriver::isPlaying(bool& playback_enabled)
{
    playback_enabled = true;
    return true;
}

bool PortAudioDeviceDriver::isRecording(bool& recording_enabled)
{
    recording_enabled = true;
    return true;
}
