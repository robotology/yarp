/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/sig/Sound.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Log.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

#include <cstring>
#include <cstdio>

using namespace yarp::sig;
using namespace yarp::os;

#define HELPER(x) (*((FlexImage*)(x)))

Sound::Sound(int bytesPerSample) {
    init(bytesPerSample);
    frequency = 0;
}

Sound::Sound(const Sound& alt) : yarp::os::Portable() {
    init(alt.getBytesPerSample());
    FlexImage& img1 = HELPER(implementation);
    FlexImage& img2 = HELPER(alt.implementation);
    img1.copy(img2);
    frequency = alt.frequency;
    synchronize();
}

Sound& Sound::operator += (const Sound& alt) {
    if (alt.channels!= channels)
    {
        printf ("unable to concatenate sounds with different number of channels!");
        return *this;
    }
    if (alt.frequency!= frequency)
    {
        printf ("unable to concatenate sounds with different sample rate!");
        return *this;
    }

    Sound orig= *this;
    this->resize(this->samples+alt.samples,channels);

    unsigned char* p1    = orig.getRawData();
    unsigned char* p2    = alt.getRawData();
    unsigned char* pout  = this->getRawData();

    for (int ch=0; ch<channels; ch++)
    {
        int out1 = ch* this->getBytesPerSample() * this->samples;
        int out2 = ch* this->getBytesPerSample() * this->samples + this->getBytesPerSample() *  orig.samples;

        int ori1 = ch * orig.getBytesPerSample() * orig.samples;
        int s1   = orig.getBytesPerSample() * orig.samples;

        int alt1 = ch * orig.getBytesPerSample() *  alt.samples;
        uint s2  = alt.getBytesPerSample() *  alt.samples;

        memcpy((void *) &pout[out1], (void *) (p1+ori1), s1);
        memcpy((void *) &pout[out2], (void *) (p2+alt1), s2);
    }

    this->synchronize();
    return *this;
}

const Sound& Sound::operator = (const Sound& alt) {
    yAssert(getBytesPerSample()==alt.getBytesPerSample());
    FlexImage& img1 = HELPER(implementation);
    FlexImage& img2 = HELPER(alt.implementation);
    img1.copy(img2);
    frequency = alt.frequency;
    synchronize();
    return *this;
}

void Sound::synchronize() {
    FlexImage& img = HELPER(implementation);
    samples = img.width();
    channels = img.height();
}

Sound Sound::subSound(int first_sample, int last_sample)
{
    if (last_sample  > this->samples)
        last_sample = samples;
    if (first_sample > this->samples)
        first_sample = samples;
    if (last_sample  < 0)
        last_sample = 0;
    if (first_sample < 0)
        first_sample = 0;
    if (last_sample < first_sample)
        last_sample = first_sample;

    Sound s;

    s.resize(last_sample-first_sample, this->channels);
    s.setFrequency(this->frequency);

    /*
    //faster implementation but currently not working
    unsigned char* p1    = this->getRawData();
    unsigned char* p2    = s.getRawData();
    int j=0;
    for (int i=first_sample; i<last_sample*2; i++)
    {
        p2[j++]=p1[i];
    }
    */

    //safe implementation
    int j=0;
    for (int i=first_sample; i<last_sample; i++)
    {
        for (int c=0; c< this->channels; c++)
            s.set(this->get(i,c),j,c);
        j++;
    }

    s.synchronize();

    return s;
}

void Sound::init(int bytesPerSample) {
    implementation = new FlexImage();
    yAssert(implementation!=nullptr);

    yAssert(bytesPerSample==2); // that's all thats implemented right now
    HELPER(implementation).setPixelCode(VOCAB_PIXEL_MONO16);
    HELPER(implementation).setQuantum(2);

    samples = 0;
    channels = 0;
    this->bytesPerSample = bytesPerSample;
}

Sound::~Sound() {
    if (implementation!=nullptr) {
        delete &HELPER(implementation);
        implementation = nullptr;
    }
}

void Sound::resize(int samples, int channels) {
    FlexImage& img = HELPER(implementation);
    img.resize(samples,channels);
    synchronize();
}

int Sound::get(int location, int channel) const {
    FlexImage& img = HELPER(implementation);
    unsigned char *addr = img.getPixelAddress(location,channel);
    if (bytesPerSample==2) {
        return *(reinterpret_cast<NetUint16*>(addr));
    }
    yInfo("sound only implemented for 16 bit samples");
    return 0;
}

void Sound::clear()
{
    int size = this->getRawDataSize();
    unsigned char* p  = this->getRawData();
    memset(p,0,size);
}

void Sound::set(int value, int location, int channel) {
    FlexImage& img = HELPER(implementation);
    unsigned char *addr = img.getPixelAddress(location,channel);
    if (bytesPerSample==2) {
        *(reinterpret_cast<NetUint16*>(addr)) = value;
        return;
    }
    yInfo("sound only implemented for 16 bit samples");
}

int Sound::getFrequency() const {
    return frequency;
}

void Sound::setFrequency(int freq) {
    this->frequency = freq;
}

bool Sound::read(ConnectionReader& connection) {
    // lousy format - fix soon!
    FlexImage& img = HELPER(implementation);
    Bottle bot;
    bool ok = PortablePair<FlexImage,Bottle>::readPair(connection,img,bot);
    frequency = bot.get(0).asInt();
    synchronize();
    return ok;
}


bool Sound::write(ConnectionWriter& connection) {
    // lousy format - fix soon!
    FlexImage& img = HELPER(implementation);
    Bottle bot;
    bot.addInt(frequency);
    return PortablePair<FlexImage,Bottle>::writePair(connection,img,bot);
}

unsigned char *Sound::getRawData() const {
    FlexImage& img = HELPER(implementation);
    return img.getRawImage();
}

int Sound::getRawDataSize() const {
    FlexImage& img = HELPER(implementation);
    return img.getRawImageSize();
}
