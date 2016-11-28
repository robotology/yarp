/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/sig/Sound.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>

#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::os;

class SoundTest : public UnitTest {
public:
    virtual ConstString getName() { return "SoundTest"; }

    void checkSetGet() {
        report(0,"check set/get sample...");
        Sound snd;
        snd.resize(100,2);
        checkEqual(100,snd.getSamples(),"sample count");
        checkEqual(2,snd.getChannels(),"channel count");
        for (int i=0; i<snd.getSamples(); i++) {
            for (int j=0; j<2; j++) {
                snd.set(42,i,j);
            }
        }
        snd.set(99,50);
        checkEqual(99,snd.get(50),"set/get");
        Sound snd2(snd);
        checkEqual(100,snd2.getSamples(),"sample count");
        checkEqual(2,snd2.getChannels(),"channel count");
        checkEqual(99,snd2.get(50),"copy works");
    }



    void checkTransmit() {
        report(0,"checking sound transmission...");

        Sound snd1;
        snd1.resize(128);
        snd1.setFrequency(50);
        for (int i=0; i<snd1.getSamples(); i++) {
            snd1.set(i-snd1.getSamples()/2,i);
        }

        PortReaderBuffer<Sound> buf;
        Port input, output;
        input.open("/in");
        output.open("/out");
        buf.setStrict();
        buf.attach(input);
        Network::connect("/out","/in");

        report(0,"writing...");
        output.write(snd1);
        report(0,"reading...");
        Sound *result = buf.read();

        checkTrue(result!=NULL,"got something check");
        if (result!=NULL) {
            checkEqual(snd1.getSamples(),result->getSamples(),
                       "sample ct check");
            checkEqual(snd1.getFrequency(),result->getFrequency(),
                       "freq check");

            bool ok = true;
            for (int i=0; i<result->getSamples(); i++) {
                ok = ok && (result->get(i) == snd1.get(i));
            }
            checkTrue(ok,"sample values match");

        }

        output.close();
        input.close();
    }
    
    virtual void runTests() {
        Network::setLocalMode(true);
        checkSetGet();
        checkTransmit();
        Network::setLocalMode(false);
    }
};

static SoundTest theSoundTest;

UnitTest& getSoundTest() {
    return theSoundTest;
}
