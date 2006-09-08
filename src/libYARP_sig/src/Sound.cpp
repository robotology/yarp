// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/Logger.h>

#include <yarp/sig/Sound.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/PortablePair.h>

using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp;

#define HELPER(x) (*((FlexImage*)(x)))

Sound::Sound(int bytesPerSample) {
    init(bytesPerSample);
}

Sound::Sound(const Sound& alt) {
    init(alt.getBytesPerSample());
    FlexImage& img1 = HELPER(implementation);
    FlexImage& img2 = HELPER(alt.implementation);
    img1.copy(img2);
    synchronize();
}

const Sound& Sound::operator = (const Sound& alt) {
    YARP_ASSERT(getBytesPerSample()==alt.getBytesPerSample());
    FlexImage& img1 = HELPER(implementation);
    FlexImage& img2 = HELPER(alt.implementation);
    img1.copy(img2);
    synchronize();
    return *this;
}

void Sound::synchronize() {
    FlexImage& img = HELPER(implementation);
    samples = img.width();
    channels = img.height();
}

void Sound::init(int bytesPerSample) {
    implementation = new FlexImage();
    YARP_ASSERT(implementation!=NULL);

    YARP_ASSERT(bytesPerSample==1); // that's all thats implemented right now
    HELPER(implementation).setPixelSize(bytesPerSample);
    HELPER(implementation).setPixelCode(VOCAB_PIXEL_MONO);

    samples = 0;
    channels = 0;
    this->bytesPerSample = bytesPerSample;    
}

Sound::~Sound() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}

void Sound::resize(int samples, int channels) {
    FlexImage& img = HELPER(implementation);
    img.resize(samples,channels);
    synchronize();
}

int Sound::get(int location, int channel) {
    FlexImage& img = HELPER(implementation);
    unsigned char *addr = img.getPixelAddress(location,channel);
    if (bytesPerSample==1) {
        return *((char *)addr);
    }
    YARP_INFO(Logger::get(),"sound only implemented for 8 bit samples");
    return 0;
}

void Sound::set(int value, int location, int channel) {
    FlexImage& img = HELPER(implementation);
    unsigned char *addr = img.getPixelAddress(location,channel);
    if (bytesPerSample==1) {
        *((char *)addr) = (char)value;
        return;
    }
    YARP_INFO(Logger::get(),"sound only implemented for 8 bit samples");
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

