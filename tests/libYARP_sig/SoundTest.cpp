/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    virtual ConstString getName() override { return "SoundTest"; }

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

    void checkSum() {
        report(0,"check set/get sample...");
        Sound snd1, snd2, sndSum;
        snd1.resize(5, 8);
        snd2.resize(3, 8);

        for(int ch=0; ch<snd1.getChannels(); ch++)
        {
            for(int s=0; s<snd1.getSamples(); s++)
            {
                snd1.set(ch+s, s, ch);
            }
        }

        for(int ch=0; ch<snd2.getChannels(); ch++)
        {
            for(int s=0; s<snd2.getSamples(); s++)
            {
                snd2.set(1000+ch+s, s, ch);
            }
        }

        sndSum  = snd1;
        sndSum += snd2;

        // Checking sum is correct
        bool ok = true;
        int s1Samples = snd1.getSamples();
        int s2Samples = snd2.getSamples();

        for(int ch=0; ch<snd1.getChannels(); ch++)
        {
            for(int s=0; s<s1Samples; s++)
            {
                ok &= (sndSum.get(s, ch) == snd1.get(s, ch));
            }

            for(int s=0; s<s2Samples; s++)
            {
                ok &= (sndSum.get(s1Samples +s , ch) == snd2.get(s, ch));
            }
        }
        checkTrue(ok,"operator '+=' test performed ");
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

        checkTrue(result!=nullptr,"got something check");
        if (result!=nullptr) {
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
    
    virtual void runTests() override {
        Network::setLocalMode(true);
        checkSetGet();
        checkSum();
        checkTransmit();
        Network::setLocalMode(false);
    }
};

static SoundTest theSoundTest;

UnitTest& getSoundTest() {
    return theSoundTest;
}
