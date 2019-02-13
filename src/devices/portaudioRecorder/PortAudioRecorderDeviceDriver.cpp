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

#include <PortAudioRecorderDeviceDriver.h>

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
    CircularAudioBuffer_16t *recdata  = dataBuffers->recData;
    int num_rec_channels     = dataBuffers->numRecChannels;
    int num_play_channels    = dataBuffers->numPlayChannels;
    int finished = paComplete;

    if (dataBuffers->canRec)
    {
        const auto* rptr = (const SAMPLE*)inputBuffer;
        unsigned int framesToCalc;
        unsigned int i;
        size_t framesLeft = recdata->size().getSamples()* recdata->size().getChannels();

        (void) outputBuffer; // just to prevent unused variable warnings
        (void) timeInfo;
        (void) statusFlags;
        (void) userData;

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
            yDebug() << "Writing" << framesToCalc*2*2 << "bytes in the circular buffer";
#endif 
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
#if 1
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

PortAudioRecorderDeviceDriver::PortAudioRecorderDeviceDriver() :
    stream(nullptr),
    err(paNoError),
    numSamples(0),
    numBytes(0),
    m_system_resource(nullptr),
    m_numPlaybackChannels(0),
    m_numRecordChannels(0),
    m_frequency(0),
    m_loopBack(false),
    m_getSoundIsNotBlocking(true)
{
    memset(&inputParameters, 0, sizeof(PaStreamParameters));
    memset(&dataBuffers, 0, sizeof(circularDataBuffers));
    memset(&m_driverConfig, 0, sizeof(PortAudioRecorderDeviceDriverSettings));
}

PortAudioRecorderDeviceDriver::~PortAudioRecorderDeviceDriver()
{
    close();
}


bool PortAudioRecorderDeviceDriver::open(yarp::os::Searchable& config)
{
    m_driverConfig.rate = config.check("rate",Value(0),"audio sample rate (0=automatic)").asInt32();
    m_driverConfig.samples = config.check("samples",Value(0),"number of samples per network packet (0=automatic). For chunks of 1 second of recording set samples=rate. Channels number is handled internally.").asInt32();
    m_driverConfig.recChannels = config.check("channels", Value(0), "number of audio channels (0=automatic, max is 2)").asInt32();
    m_driverConfig.deviceNumber = config.check("id",Value(-1),"which portaudio index to use (-1=automatic)").asInt32();

    return open(m_driverConfig);
}

bool PortAudioRecorderDeviceDriver::open(PortAudioRecorderDeviceDriverSettings& config)
{
    int rate = config.rate;
    int samples = config.samples;
    int recChannels = config.recChannels;
    int deviceNumber = config.deviceNumber;

    if (recChannels == 0) recChannels = DEFAULT_NUM_CHANNELS;
    m_numRecordChannels = recChannels;

    if (rate==0) rate = DEFAULT_SAMPLE_RATE;
    m_frequency = rate;

    if (samples==0)
        numSamples = m_frequency; //  by default let's stream chunks of 1 second
    else
        numSamples = samples;

    size_t numPlayBytes = numSamples * sizeof(SAMPLE) * m_numPlaybackChannels;
    size_t numRecBytes = numSamples * sizeof(SAMPLE) * m_numRecordChannels;
    int twiceTheBuffer = 2;
    AudioBufferSize playback_buffer_size(numSamples, m_numPlaybackChannels, sizeof(SAMPLE));
    AudioBufferSize rec_buffer_size (numSamples, m_numRecordChannels, sizeof(SAMPLE));
    dataBuffers.numPlayChannels = m_numPlaybackChannels;
    dataBuffers.numRecChannels = m_numRecordChannels;
    if (dataBuffers.playData==nullptr)
        dataBuffers.playData = new CircularAudioBuffer_16t("portatudio_play", playback_buffer_size);
    if (dataBuffers.recData==nullptr)
        dataBuffers.recData = new CircularAudioBuffer_16t("portatudio_rec", rec_buffer_size);
    dataBuffers.canRec = true;

    err = Pa_Initialize();
    if( err != paNoError )
    {
        yError("portaudio system failed to initialize");
        return false;
    }

    inputParameters.device = (deviceNumber==-1)?Pa_GetDefaultInputDevice():deviceNumber;
    yInfo("Device number %d", inputParameters.device);
    inputParameters.channelCount = m_numRecordChannels;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    if ((Pa_GetDeviceInfo( inputParameters.device ))!=nullptr) {
        inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    }
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              nullptr,
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

void recStreamThread::handleError()
{
    Pa_Terminate();
    if( err != paNoError )
    {
        fprintf( stderr, "An error occurred while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    }
}

void PortAudioRecorderDeviceDriver::handleError()
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

bool PortAudioRecorderDeviceDriver::close()
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

bool PortAudioRecorderDeviceDriver::startRecording()
{
    pThread.something_to_record = true;
    err = Pa_StartStream( stream );
    if( err < 0 ) {handleError(); return false;}
    return true;
}

bool PortAudioRecorderDeviceDriver::stopRecording()
{
    pThread.something_to_record = false;
    err = Pa_StopStream( stream );
    if( err < 0 ) {handleError(); return false;}
    return true;
}

bool PortAudioRecorderDeviceDriver::getSound(yarp::sig::Sound& sound)
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
                 yError() << "PortAudioRecorderDeviceDriver::getSound() Buffer size is still zero after 100 iterations, returning";
                 return false;
             }
             else
             {
                 yDebug() << "PortAudioRecorderDeviceDriver::getSound() Buffer size is " << buff_size << "/" << this->numSamples <<" after 100 iterations";
                 if (m_getSoundIsNotBlocking)
                 {
                     yError() << "PortAudioRecorderDeviceDriver::getSound() is in not-blocking mode, returning";
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
    sound.setFrequency(this->m_driverConfig.rate);

    for (size_t i=0; i<this->numSamples; i++)
        for (size_t j=0; j<this->m_numRecordChannels; j++)
            {
                SAMPLE s = dataBuffers.recData->read();
                sound.set(s,i,j);
            }
    return true;
}

void recStreamThread::threadRelease()
{
}

bool recStreamThread::threadInit()
{
    something_to_record=false;
    err = paNoError;
    return true;
}

void recStreamThread::run()
{
    while(this->isStopping()==false)
    {
        if (something_to_record)
        {
            while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
            {
                yarp::os::SystemClock::delaySystem(SLEEP_TIME);
            }
            if (err == 0)
            {
                Pa_StopStream(stream);
                yDebug() << "The recording stream has been stopped";
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

bool PortAudioRecorderDeviceDriver::getRecordingAudioBufferCurrentSize(yarp::dev::AudioBufferSize& size)
{
    size = this->dataBuffers.recData->size();
    return true;
}

bool PortAudioRecorderDeviceDriver::getRecordingAudioBufferMaxSize(yarp::dev::AudioBufferSize& size)
{
    size = this->dataBuffers.recData->getMaxSize();
    return true;
}

bool PortAudioRecorderDeviceDriver::resetRecordingAudioBuffer()
{
    this->dataBuffers.recData->clear();
    return true;
}

