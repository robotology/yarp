// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/PortAudioDeviceDriver.h>

#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

#define SAMPLE_RATE  (44100)
#define NUM_CHANNELS    (2)
#define DITHER_FLAG     (0)

#define NUM_SECONDS	0.1

//#define	NUM_SAMPLES	((int)(SAMPLE_RATE*NUM_SECONDS))
#define	NUM_SAMPLES	((int)(1024))

//char 		devname[] = "/dev/dsp";

using namespace yarp::os;
using namespace yarp::dev;


// portaudio version 1.8 doesn't have Pa_ReadStream :-(

/* Select sample format. */
#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#elif 0
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define SAMPLE_UNSIGNED
#endif

static SAMPLE	pa_pulsecode[ NUM_SAMPLES*NUM_CHANNELS];
static double pa_tap_test = 1e6;

PortAudioDeviceDriver::PortAudioDeviceDriver() {
    system_resource = NULL;
}

PortAudioDeviceDriver::~PortAudioDeviceDriver() {
    close();
}


bool PortAudioDeviceDriver::open(int rate,
                                 int samples) {
    if (rate==0)    rate = SAMPLE_RATE;
    if (samples!=0) {
        printf("WARNING: \"samples\" ignored right now\n");
    }
    if (samples==0) samples = NUM_SAMPLES;

    printf("WARNING: the PortAudioDeviceDriver is just a cartoon\n");
    printf("WARNING: it hasn't really been written yet\n");

    // just for testing, ssssh it is a secret
    pa_tap_test = 1e6; //config.check("tap",Value(1e6)).asDouble();

    PaStreamParameters inputParameters;
    PaStream *stream;
    PaError    err;
    int        totalFrames;
    int        numSamples;
    int        numBytes;
    printf("patest_record.c\n"); fflush(stdout);

    numSamples = totalFrames * NUM_CHANNELS;

    numBytes = numSamples * sizeof(SAMPLE);

    err = Pa_Initialize();
    if( err != paNoError ) {
        printf("Audio error\n");
        exit(1);
    }


    /* Record some audio. -------------------------------------------- */
    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,
              rate,
              1024,            /* frames per buffer */
              paClipOff,
              NULL, //recordCallback,
              NULL
              );
    if( err != paNoError ) {
        printf("Audio error\n");
        exit(1);
    }

    err = Pa_StartStream( stream );
    if( err != paNoError ) {
        printf("Audio error\n");
        exit(1);
    }
    printf("Now recording!!\n"); fflush(stdout);
    system_resource = stream;
    
    return true;
}


bool PortAudioDeviceDriver::open(yarp::os::Searchable& config) {
    int chosen_rate = config.check("rate",Value(0)).asInt();
    int chosen_samples = config.check("samples",Value(0)).asInt();
    return open(chosen_rate,chosen_samples);
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
	// main loop to capture the waveform audio data	
    sound.resize(NUM_SAMPLES);
    double total = 0;
    PaError err;
    err = Pa_ReadStream((PaStream*)system_resource,(void*)pa_pulsecode,1024);
    if( err != paNoError ) {
        printf("Audio error\n");
        exit(1);
    }    
    for (int i=0; i<NUM_SAMPLES; i++) {
        sound.set(pa_pulsecode[i],i);
        total += abs(pa_pulsecode[i]-128);
    }
    total /= NUM_SAMPLES;

    if (total >= pa_tap_test) {
        printf( "grabbing a chunk of %d samples... magnitude: %g\n", 
                NUM_SAMPLES, total);
    }
    return true;
}
