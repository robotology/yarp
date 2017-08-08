/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <PortAudioDeviceDriver.h>

#include <cstdio>
#include <cstdlib>
#include <portaudio.h>

#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace yarp::dev;

#define SLEEP_TIME 0.005f

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
    circularDataBuffers* dataBuffers = static_cast<circularDataBuffers*>(userData);
    circularBuffer *playdata = dataBuffers->playData;
    circularBuffer *recdata  = dataBuffers->recData;
    int num_channels         = dataBuffers->numChannels;
    int finished = paComplete;

    if (dataBuffers->canRec)
    {
        const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
        unsigned int framesToCalc;
        unsigned int i;
        unsigned long framesLeft = recdata->getMaxSize()-recdata->size();

        (void) outputBuffer; // just to prevent unused variable warnings
        (void) timeInfo;
        (void) statusFlags;
        (void) userData;

        if( framesLeft/num_channels < framesPerBuffer )
        {
            framesToCalc = framesLeft/num_channels;
            finished = paComplete;
        }
        else
        {
            framesToCalc = framesPerBuffer;
            finished = paContinue;
        }

        if( inputBuffer == NULL )
        {
            for( i=0; i<framesToCalc; i++ )
            {
                recdata->write(0); // left
                if( num_channels == 2 ) recdata->write(0);  // right
            }
        }
        else
        {
            for( i=0; i<framesToCalc; i++ )
            {
                recdata->write(*rptr++);  // left
                if( num_channels == 2 ) recdata->write(*rptr++);  // right
            }
        }
        //note: you can record or play but not simultaneously (for now)
        return finished;
    }

    if (dataBuffers->canPlay)
    {
        SAMPLE *wptr = (SAMPLE*)outputBuffer;
        unsigned int i;

        unsigned int framesLeft = playdata->size();

        (void) inputBuffer; // just to prevent unused variable warnings
        (void) timeInfo;
        (void) statusFlags;
        (void) userData;

        if( framesLeft/num_channels < framesPerBuffer )
        {
            // final buffer
            for( i=0; i<framesLeft/num_channels; i++ )
            {
                *wptr++ = playdata->read();  // left
                if( num_channels == 2 ) *wptr++ = playdata->read();  // right
            }
            for( ; i<framesPerBuffer; i++ )
            {
                *wptr++ = 0;  // left
                if( num_channels == 2 ) *wptr++ = 0;  // right
            }
            finished = paComplete;
        }
        else
        {
            for( i=0; i<framesPerBuffer; i++ )
            {
                *wptr++ = playdata->read();  // left
                if( num_channels == 2 ) *wptr++ = playdata->read();  // right
            }
            finished = paContinue;
        }
        //note: you can record or play but not simultaneously (for now)
        return finished;
    }

    printf("No read/write operations requested, aborting\n");
    return paAbort;
}

PortAudioDeviceDriver::PortAudioDeviceDriver()
{
    system_resource = NULL;
    numSamples = 0;
    numChannels = 0;
    loopBack = false;
    frequency = 0;
    err = paNoError;
    dataBuffers.playData = 0;
    dataBuffers.recData = 0;
    renderMode = RENDER_APPEND;
    stream = 0;
}

PortAudioDeviceDriver::~PortAudioDeviceDriver()
{
    close();
}


bool PortAudioDeviceDriver::open(yarp::os::Searchable& config)
{
    driverConfig.rate = config.check("rate",Value(0),"audio sample rate (0=automatic)").asInt();
    driverConfig.samples = config.check("samples",Value(0),"number of samples per network packet (0=automatic). For chunks of 1 second of recording set samples=rate. Channels number is handled internally.").asInt();
    driverConfig.channels = config.check("channels",Value(0),"number of audio channels (0=automatic, max is 2)").asInt();
    driverConfig.wantRead = (bool)config.check("read","if present, just deal with reading audio (microphone)");
    driverConfig.wantWrite = (bool)config.check("write","if present, just deal with writing audio (speaker)");
    driverConfig.deviceNumber = config.check("id",Value(-1),"which portaudio index to use (-1=automatic)").asInt();

    if (!(driverConfig.wantRead||driverConfig.wantWrite))
    {
        driverConfig.wantRead = driverConfig.wantWrite = true;
    }

    if (config.check("loopback","if present, send audio read from microphone immediately back to speaker"))
    {
        printf ("WARN: loopback not yet implemented\n");
        loopBack = true;
    }

    if (config.check("render_mode_append"))
    {
        renderMode = RENDER_APPEND;
    }
    if (config.check("render_mode_immediate"))
    {
        renderMode = RENDER_IMMEDIATE;
    }

    return open(driverConfig);
}

bool PortAudioDeviceDriver::open(PortAudioDeviceDriverSettings& config)
{
    int rate = config.rate;
    int samples = config.samples;
    int channels = config.channels;
    bool wantRead = config.wantRead;
    bool wantWrite = config.wantWrite;
    int deviceNumber = config.deviceNumber;

    if (channels==0) channels = DEFAULT_NUM_CHANNELS;
    numChannels = channels;
    if (rate==0) rate = DEFAULT_SAMPLE_RATE;
    frequency = rate;

    if (samples==0)
        numSamples = frequency; //  by default let's stream chunks of 1 second
    else
        numSamples = samples;

    //buffer.allocate(num_samples*num_channels*sizeof(SAMPLE));
    numBytes = numSamples * sizeof(SAMPLE) * numChannels;
    int twiceTheBuffer = numBytes * 2;
    dataBuffers.numChannels=numChannels;
    if (dataBuffers.playData==0)
        dataBuffers.playData = new circularBuffer(twiceTheBuffer);
    if (dataBuffers.recData==0)
        dataBuffers.recData = new circularBuffer(twiceTheBuffer);
    if (wantRead) dataBuffers.canRec = true;
    if (wantWrite) dataBuffers.canPlay = true;

    err = Pa_Initialize();
    if( err != paNoError ) {
        printf("portaudio system failed to initialize\n");
        return false;
    }

    inputParameters.device = (deviceNumber==-1)?Pa_GetDefaultInputDevice():deviceNumber;
    printf("Device number %d\n", inputParameters.device);
    inputParameters.channelCount = numChannels;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    if ((Pa_GetDeviceInfo( inputParameters.device ))!=0) {
        inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    }
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = (deviceNumber==-1)?Pa_GetDefaultOutputDevice():deviceNumber;
    outputParameters.channelCount = numChannels;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              wantRead?(&inputParameters):NULL,
              wantWrite?(&outputParameters):NULL,
              frequency,
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

void streamThread::handleError()
{
    Pa_Terminate();
    if( err != paNoError )
    {
        fprintf( stderr, "An error occurred while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    }
}

void PortAudioDeviceDriver::handleError()
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

bool PortAudioDeviceDriver::close(void)
{
    pThread.stop();
    if (stream != 0)
    {
        err = Pa_CloseStream( stream );
        if( err != paNoError )
        {
            fprintf( stderr, "An error occurred while closing the portaudio stream\n" );
            fprintf( stderr, "Error number: %d\n", err );
            fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        }
    }

    if (this->dataBuffers.playData != 0)
    {
        delete this->dataBuffers.playData;
        this->dataBuffers.playData = 0;
    }
    if (this->dataBuffers.recData != 0)
    {
        delete this->dataBuffers.recData;
        this->dataBuffers.recData = 0;
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

bool PortAudioDeviceDriver::getSound(yarp::sig::Sound& sound)
{
    if (pThread.something_to_record == false)
    {
        this->startRecording();
    }

    int buff_size = 0;
    static int buff_size_wdt = 0;
    while (buff_size<this->numSamples*this->numChannels)
    {
         buff_size = dataBuffers.recData->size();
         if (buff_size == 0 && buff_size_wdt++ == 100) break;
         yarp::os::SystemClock::delaySystem(SLEEP_TIME);
    }
    buff_size_wdt = 0;

    if (sound.getChannels()!=this->numChannels && sound.getSamples() != this->numSamples)
    {
        sound.resize(this->numSamples,this->numChannels);
    }
    sound.setFrequency(this->driverConfig.rate);

    for (int i=0; i<this->numSamples; i++)
        for (int j=0; j<this->numChannels; j++)
            {
                SAMPLE s = dataBuffers.recData->read();
                sound.set(s,i,j);
            }
    return true;
}

bool PortAudioDeviceDriver::abortSound(void)
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
                {yarp::os::SystemClock::delaySystem(SLEEP_TIME);}
            if( err < 0 ) {handleError(); return;}

            err = Pa_StopStream( stream );
            //err = Pa_AbortStream( stream );
            if( err < 0 ) {handleError(); return;}

        }

        if (something_to_record)
        {
            while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
                {yarp::os::SystemClock::delaySystem(SLEEP_TIME);}
            if( err < 0 ) {handleError(); return;}
        }

        yarp::os::Time::delay(SLEEP_TIME);
    }
    return;
}

bool PortAudioDeviceDriver::immediateSound(yarp::sig::Sound& sound)
{
    dataBuffers.playData->clear();

    //unsigned char* dataP= sound.getRawData();
    int num_bytes = sound.getBytesPerSample();
    int num_channels = sound.getChannels();
    int num_samples = sound.getRawDataSize()/num_channels/num_bytes;
    // memcpy(data.samplesBuffer,dataP,num_samples/**num_bytes*num_channels*/);

    for (int i=0; i<num_samples; i++)
        for (int j=0; j<num_channels; j++)
            dataBuffers.playData->write (sound.get(i,j));

    pThread.something_to_play = true;
    return true;
}

bool PortAudioDeviceDriver::renderSound(yarp::sig::Sound& sound)
{
    int freq  = sound.getFrequency();
    int chans = sound.getChannels();
    if (freq  != this->frequency ||
        chans != this->numChannels)
    {
        //wait for current playback to finish
        while (Pa_IsStreamStopped(stream )==0)
        {
            yarp::os::Time::delay(SLEEP_TIME);
        }

        //reset the driver
        printf("***** driver configuration changed, resetting *****\n");
        this->close();
        driverConfig.channels = chans;
        driverConfig.rate = freq;
        bool ok = open(driverConfig);
        if (ok == false)
        {
            printf("error occurred during driver reconfiguration, aborting\n");
            return false;
        }
    }

    if (renderMode == RENDER_IMMEDIATE)
        return immediateSound(sound);
    else if (renderMode == RENDER_APPEND)
        return appendSound(sound);

    return false;
}

bool PortAudioDeviceDriver::appendSound(yarp::sig::Sound& sound)
{
    //unsigned char* dataP= sound.getRawData();
    int num_bytes = sound.getBytesPerSample();
    int num_channels = sound.getChannels();
    int num_samples = sound.getRawDataSize()/num_channels/num_bytes;
    // memcpy(data.samplesBuffer,dataP,num_samples/**num_bytes*num_channels*/);

    for (int i=0; i<num_samples; i++)
        for (int j=0; j<num_channels; j++)
            dataBuffers.playData->write (sound.get(i,j));

    pThread.something_to_play = true;
    return true;
}


