/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <MicrophoneDeviceDriver.h>

#include <cstdio>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

#define SAMPLE_RATE 48000
#define NUM_SECONDS 0.1
#define NUM_SAMPLES ((int)(SAMPLE_RATE*NUM_SECONDS))

char devname[] = "/dev/dsp";
unsigned char pulsecode[ NUM_SAMPLES ];

using namespace yarp::os;
using namespace yarp::dev;


MicrophoneDeviceDriver::MicrophoneDeviceDriver() :
    system_resources(nullptr),
    dsp(-1)
{}

MicrophoneDeviceDriver::~MicrophoneDeviceDriver() {
    close();
}

bool MicrophoneDeviceDriver::open(yarp::os::Searchable& config) {

    printf("WARNING: the MicrophoneDeviceDriver is just a cartoon\n");
    printf("WARNING: it hasn't really been written yet\n");

    short nChannels = 1;
    int samplesPerSec = SAMPLE_RATE;
    short bitsPerSample = 8;

    // open the audio device-file and set its capture parameters
    int bits = ( bitsPerSample == 8 ) ? AFMT_U8 : AFMT_S16_BE;
    int chns = ( nChannels == 1 ) ? 1 : 2;
    int rate = samplesPerSec;
    dsp = ::open( devname, O_RDONLY );
    if ( dsp < 0 ) { perror( devname ); std::exit(1); }
    if ( ioctl( dsp, SNDCTL_DSP_RESET, NULL ) < 0 )
        { perror( "ioctl" ); std::exit(1); }
    if (      ( ioctl( dsp, SNDCTL_DSP_SETFMT,   &bits ) < 0 )
              ||( ioctl( dsp, SNDCTL_DSP_CHANNELS, &chns ) < 0 )
              ||( ioctl( dsp, SNDCTL_DSP_SPEED,    &rate ) < 0 )    )
        { perror( "audio format not not supported\n" ); std::exit(1); }

    printf("Ok, opened microphone...\n");

    return true;
}

bool MicrophoneDeviceDriver::close() {
    if (dsp!=-1) {
        ::close( dsp );
        dsp = -1;
    }
    return true;
}

bool MicrophoneDeviceDriver::getSound(yarp::sig::Sound& sound) {
    // main loop to capture the waveform audio data
    unsigned char *data = pulsecode;
    int begun = 1;
    int bytes_to_grab = NUM_SAMPLES / 16;
    int bytes_to_save = NUM_SAMPLES;
    while ( bytes_to_save > 0 )
        {
            int nbytes = read( dsp, data, bytes_to_grab );
            if ( !begun )
                {
                    int max = 0;
                    for (int i = 0; i < nbytes; i++)
                        if ( data[i] > max ) max = data[i];
                    if ( max > 0x99 ) begun = 1;
                    else continue;
                }
            bytes_to_save -= nbytes;
            data += nbytes;
        }
    sound.resize(NUM_SAMPLES);
    double total = 0;
    for (int i=0; i<NUM_SAMPLES; i++) {
        sound.set(pulsecode[i],i);
        total += abs(pulsecode[i]-128);
    }
    printf( "grabbing a chunk of %d samples... magnitude: %g\n",
            NUM_SAMPLES, total);
    return true;
}

bool MicrophoneDeviceDriver::startRecording()
{
    return false;
}

bool MicrophoneDeviceDriver::stopRecording()
{
    return false;
}
