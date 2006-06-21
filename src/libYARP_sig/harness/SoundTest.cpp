// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/sig/Sound.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::sig;

class SoundTest : public UnitTest {
public:
    virtual String getName() { return "SoundTest"; }

    void checkSetGet() {
        report(0,"check set/get sample");
        Sound snd;
        snd.resize(100,2);
        checkEqual(100,snd.getSamples(),"sample count");
        checkEqual(2,snd.getChannels(),"channel count");
        snd.set(99,50);
        checkEqual(99,snd.get(50),"set/get");
        Sound snd2(snd);
        checkEqual(100,snd2.getSamples(),"sample count");
        checkEqual(2,snd2.getChannels(),"channel count");
        checkEqual(99,snd2.get(50),"copy works");
    }
    
    virtual void runTests() {
        checkSetGet();
    }
};

static SoundTest theSoundTest;

UnitTest& getSoundTest() {
  return theSoundTest;
}
