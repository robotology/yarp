// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <PortAudioDeviceDriver.h>

#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

#include <yarp/os/Time.h>

#define SAMPLE_RATE  (44100)
#define NUM_CHANNELS    (2)
#define DITHER_FLAG     (0)

#define NUM_SECONDS	0.1

//#define	NUM_SAMPLES	((int)(SAMPLE_RATE*NUM_SECONDS))
#define	NUM_SAMPLES	((int)(512))

//char 		devname[] = "/dev/dsp";

using namespace yarp::os;
using namespace yarp::dev;


// portaudio version 1.8 doesn't have Pa_ReadStream :-(

/* Select sample format. */
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

//static SAMPLE	pa_pulsecode[ NUM_SAMPLES*NUM_CHANNELS];
static double pa_tap_test = 1e6;

PortAudioDeviceDriver::PortAudioDeviceDriver() {
    system_resource = NULL;
    num_samples = 0;
    num_channels = 0;
    canRead = canWrite = false;
    loopBack = false;
    set_freq = 0;
}

PortAudioDeviceDriver::~PortAudioDeviceDriver() {
    close();
}


bool PortAudioDeviceDriver::open(PortAudioDeviceDriverSettings& config) {
    int rate = config.rate;
    int samples = config.samples;
    int channels = config.channels;
    bool wantRead = config.wantRead;
    bool wantWrite = config.wantWrite;
    int deviceNumber = config.deviceNumber;
    if (rate==0)    rate = SAMPLE_RATE;
    if (samples==0) samples = NUM_SAMPLES;
    num_samples = samples;
    if (channels==0) channels = NUM_CHANNELS;
    num_channels = channels;
    set_freq = rate;

    buffer.allocate(num_samples*num_channels*sizeof(SAMPLE));

    // just for testing, ssssh it is a secret
    pa_tap_test = 1e6; //config.check("tap",Value(1e6)).asDouble();

    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    PaError    err;

    err = Pa_Initialize();
    if( err != paNoError ) {
        printf("portaudio system failed to initialize\n");
        return false;
    }

    inputParameters.device = (deviceNumber==-1)?Pa_GetDefaultInputDevice():deviceNumber;
    printf("Device number %d\n", inputParameters.device);
    inputParameters.channelCount = num_channels;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    if ((Pa_GetDeviceInfo( inputParameters.device ))!=0) {
        inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    }
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = (deviceNumber==-1)?Pa_GetDefaultInputDevice():deviceNumber;
    outputParameters.channelCount = num_channels;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    //outputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;


    err = Pa_OpenStream(
                        &stream,
                        wantRead?(&inputParameters):NULL,
                        wantWrite?(&outputParameters):NULL,
                        rate,
                        num_samples,
                        paClipOff,
                        NULL,
                        NULL
                        );
    if( err != paNoError ) {
        printf("portaudio stream failed to initialize, check settings\n");
        return false;
    }

    err = Pa_StartStream( stream );
    if( err != paNoError ) {
        printf("portaudio stream failed to start, check settings\n");
        return false;
    }
    printf("Reading/Writing audio data using portaudio...\n"); fflush(stdout);
    printf("   Audio parameters:\n");
    printf("     read %d, write %d loopback %d\n", wantRead, wantWrite,
           loopBack);
    printf("     (sampling) rate (in Hertz) %d\n", rate);
    printf("     samples (per block) %d\n", num_samples);
    printf("     channels %d\n", num_channels);
    config.rate = rate;
    config.samples = samples;
    config.channels = channels;
    system_resource = stream;
    canRead = wantRead;
    canWrite = wantWrite;

    return true;
}


bool PortAudioDeviceDriver::open(yarp::os::Searchable& config) {
    PortAudioDeviceDriverSettings config2;
    config2.rate = config.check("rate",Value(0),"audio sample rate (0=automatic)").asInt();
    config2.samples = config.check("samples",Value(0),"number of samples per network packet (0=automatic)").asInt();
    config2.channels = config.check("channels",Value(0),"number of audio channels (0=automatic, max is 2)").asInt();
    config2.wantRead = (bool)config.check("read","if present, just deal with reading audio (microphone)");
    config2.wantWrite = (bool)config.check("write","if present, just deal with writing audio (speaker)");
    config2.deviceNumber = config.check("id",Value(-1),"which portaudio index to use (-1=automatic)").asInt();
    if (!(config2.wantRead||config2.wantWrite)) {
        config2.wantRead = config2.wantWrite = true;
    }
    if (config.check("loopback","if present, send audio read from microphone immediately back to speaker")) {
        loopBack = true;
    }
    delayed = false;
    delayedConfig = config2;
    if (config.check("delay","if present, do not configure audio device until it needs to be used")) {
        printf("Delaying audio configuration\n");
        delayed = true;
        return true;
    } else {
        return open(delayedConfig);
    }
}

bool PortAudioDeviceDriver::close(void) {
    PaError    err;
    if (system_resource!=NULL) {
        err = Pa_CloseStream( (PaStream*)system_resource );
        if( err != paNoError ) {
            printf("Audio error -- portaudio close failed (%s)\n",
                   Pa_GetErrorText(err));
            exit(1);
        }
        system_resource = NULL;
    }

    return true;
}

bool PortAudioDeviceDriver::getSound(yarp::sig::Sound& sound) {

    checkDelay(sound);
    
    if (!canRead) {
        Time::delay(0.25);
        return false;
    }

	// main loop to capture the waveform audio data	
    sound.resize(num_samples,num_channels);
    sound.setFrequency(set_freq);
    PaError err;
    SAMPLE *pa_pulsecode = (SAMPLE *)buffer.get();
    err = Pa_ReadStream((PaStream*)system_resource,(void*)pa_pulsecode,
                        num_samples);
    if (err == paInputOverflowed) {
        printf("Audio warning -- there was an input overflow (%s)\n",
               Pa_GetErrorText(err));
    } else if( err != paNoError ) {
        printf("Audio error -- portaudio read failed (%s)\n",
               Pa_GetErrorText(err));
        exit(1);
    }

    int idx = 0;
    for (int i=0; i<num_samples; i++) {
        for (int j=0; j<num_channels; j++) {
            sound.set(pa_pulsecode[idx],i,j);
            idx++;
        }
    }

    if (canWrite&&loopBack) {
        renderSound(sound);
    }
    return true;
}

void PortAudioDeviceDriver::checkDelay(yarp::sig::Sound& sound) {
    int _rate = sound.getFrequency();
    int _samples = sound.getSamples();    
    int _channels = sound.getChannels();
    if (!delayed) {
        if ((_rate!=0 && delayedConfig.rate!=_rate)||
            (_samples!=0 && delayedConfig.samples!=_samples)||
            (_channels!=0 && delayedConfig.channels!=_channels)) {
            printf("audio configuration mismatch, resetting\n");
            if (delayedConfig.rate!=_rate) {
                printf("  (sample rate of %d versus %d)\n",
                       delayedConfig.rate,_rate);
            }
            if (delayedConfig.samples!=_samples) {
                printf("  (sample count of %d versus %d)\n",
                       delayedConfig.samples,_samples);
            }
            if (delayedConfig.channels!=_channels) {
                printf("  (channel count of %d versus %d)\n",
                       delayedConfig.channels,_channels);
            }
            close();
            delayed = true;
        }
    }
    if (delayed) {
        delayedConfig.rate = _rate;
        delayedConfig.samples = _samples;
        delayedConfig.channels = _channels;
        printf("rate from sound is %d\n", delayedConfig.rate);
        printf("samples from sound is %d\n", delayedConfig.samples);
        printf("channels from sound is %d\n", delayedConfig.channels);
        open(delayedConfig);
        delayed = false;
    }
}


bool PortAudioDeviceDriver::renderSound(yarp::sig::Sound& sound) {
    if (sound.getSamples()==0) {
        return canWrite;
    }

    checkDelay(sound);

    if (!canWrite) {
        return false;
    }

    PaError err;
    SAMPLE *pa_pulsecode = (SAMPLE *)buffer.get();

    int idx = 0;
    for (int i=0; i<num_samples; i++) {
        for (int j=0; j<num_channels; j++) {
            pa_pulsecode[idx] = sound.get(i,j);
            idx++;
        }
    }

    err = Pa_WriteStream((PaStream*)system_resource,(void*)pa_pulsecode,
                         num_samples);
    if( err != paNoError ) {
        printf("Audio error -- portaudio write failed (%s)\n",
               Pa_GetErrorText(err));
        exit(1);
    }
    return true;
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*
  PortAudioRender::PortAudioRender() {
  }

  PortAudioRender::~PortAudioRender() {
  close();
  }

  bool PortAudioRender::open(yarp::os::Searchable& config) {
  }

  bool PortAudioRender::close() {
  }


  bool PortAudioRender::renderSound(yarp::sig::Sound& sound) {
  }

*/
