// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/Logger.h>

#include <yarp/sig/Sound.h>
#include <yarp/sig/Image.h>

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
    samples = img1.width();
    channels = img1.height();
}

const Sound& Sound::operator = (const Sound& alt) {
    YARP_ASSERT(getBytesPerSample()==alt.getBytesPerSample());
    FlexImage& img1 = HELPER(implementation);
    FlexImage& img2 = HELPER(alt.implementation);
    img1.copy(img2);
    samples = img1.width();
    channels = img1.height();
    return *this;
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
    this->samples = img.width();
    this->channels = img.height();
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

/*
  int Sound::getFrequency() {
  return frequency;
  }

  void Sound::setFrequency(int freq) {
  this->frequency = freq;
  }
*/

bool Sound::read(ConnectionReader& connection) {
    FlexImage& img = HELPER(implementation);
    frequency = 0; // no frequency transmitted yet
    return img.read(connection);
}


bool Sound::write(ConnectionWriter& connection) {
    // we just ship the image -- this is sleazy because we don't send
    // frequency
    FlexImage& img = HELPER(implementation);
    return img.write(connection);
}

