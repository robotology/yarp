/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    for (size_t ch=0; ch<channels; ch++)
    {
        size_t out1 = ch* this->getBytesPerSample() * this->samples;
        size_t out2 = ch* this->getBytesPerSample() * this->samples + this->getBytesPerSample() * orig.samples;

        size_t ori1 = ch * orig.getBytesPerSample() * orig.samples;
        size_t s1   = orig.getBytesPerSample() * orig.samples;

        size_t alt1 = ch * orig.getBytesPerSample() * alt.samples;
        unsigned int s2 = alt.getBytesPerSample() * alt.samples;

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

Sound Sound::subSound(size_t first_sample, size_t last_sample)
{
    if (last_sample  > this->samples)
        last_sample = samples;
    if (first_sample > this->samples)
        first_sample = samples;
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
    size_t j=0;
    for (size_t i=first_sample; i<last_sample; i++)
    {
        for (size_t c=0; c< this->channels; c++)
            s.set(this->get(i,c),j,c);
        j++;
    }

    s.synchronize();

    return s;
}

void Sound::init(size_t bytesPerSample) {
    implementation = new FlexImage();
    yAssert(implementation!=nullptr);

    yAssert(bytesPerSample==2); // that's all that's implemented right now
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

void Sound::resize(size_t samples, size_t channels) {
    FlexImage& img = HELPER(implementation);
    img.resize(samples,channels);
    synchronize();
}

int Sound::get(size_t location, size_t channel) const {
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

void Sound::set(int value, size_t location, size_t channel) {
    FlexImage& img = HELPER(implementation);
    unsigned char *addr = img.getPixelAddress(location,channel);
    if (bytesPerSample==2) {
        *(reinterpret_cast<NetUint16*>(addr)) = value;
        return;
    }
    yInfo("sound only implemented for 16 bit samples");
}

size_t Sound::getFrequency() const {
    return frequency;
}

void Sound::setFrequency(size_t freq) {
    this->frequency = freq;
}

bool Sound::read(ConnectionReader& connection) {
    // lousy format - fix soon!
    FlexImage& img = HELPER(implementation);
    Bottle bot;
    bool ok = PortablePair<FlexImage,Bottle>::readPair(connection,img,bot);
    frequency = bot.get(0).asInt32();
    synchronize();
    return ok;
}


bool Sound::write(ConnectionWriter& connection) const {
    // lousy format - fix soon!
    FlexImage& img = HELPER(implementation);
    Bottle bot;
    bot.addInt32(frequency);
    return PortablePair<FlexImage,Bottle>::writePair(connection,img,bot);
}

unsigned char *Sound::getRawData() const {
    FlexImage& img = HELPER(implementation);
    return img.getRawImage();
}

size_t Sound::getRawDataSize() const {
    FlexImage& img = HELPER(implementation);
    return img.getRawImageSize();
}
