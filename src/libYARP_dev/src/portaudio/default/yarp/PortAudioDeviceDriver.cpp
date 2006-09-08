// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/PortAudioDeviceDriver.h>

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
    if (rate==0)    rate = SAMPLE_RATE;
    if (samples==0) samples = NUM_SAMPLES;
    num_samples = samples;
    if (channels==0) channels = NUM_CHANNELS;
    num_channels = channels;

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

    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = num_channels;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    if ((Pa_GetDeviceInfo( inputParameters.device ))!=0) {
        inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    }
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultInputDevice();
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
    system_resource = stream;
    canRead = wantRead;
    canWrite = wantWrite;

    return true;
}


bool PortAudioDeviceDriver::open(yarp::os::Searchable& config) {
    PortAudioDeviceDriverSettings config2;
    config2.rate = config.check("rate",Value(0)).asInt();
    config2.samples = config.check("samples",Value(0)).asInt();
    config2.channels = config.check("channels",Value(0)).asInt();
    config2.wantRead = (bool)config.check("read");
    config2.wantWrite = (bool)config.check("write");
    if (!(config2.wantRead||config2.wantWrite)) {
        config2.wantRead = config2.wantWrite = true;
    }
    if (config.check("loopback")) {
        loopBack = true;
    }
    delayed = false;
    if (config.check("delay")) {
        printf("Delaying audio configuration\n");
        delayedConfig = config2;
        delayed = true;
        return true;
    } else {
        return open(config2);
    }
}

bool PortAudioDeviceDriver::close(void) {
    PaError    err;
    if (system_resource!=NULL) {
        err = Pa_CloseStream( (PaStream*)system_resource );
        if( err != paNoError ) {
            printf("Audio error\n");
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
    PaError err;
    SAMPLE *pa_pulsecode = (SAMPLE *)buffer.get();
    err = Pa_ReadStream((PaStream*)system_resource,(void*)pa_pulsecode,
                        num_samples);
    if( err != paNoError ) {
        printf("Audio error\n");
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
    if (delayed) {
        delayedConfig.rate = sound.getFrequency();
        printf("rate from sound is %d\n", delayedConfig.rate);
        open(delayedConfig);
        delayed = false;
    }
}


bool PortAudioDeviceDriver::renderSound(yarp::sig::Sound& sound) {
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
        printf("Audio error\n");
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
