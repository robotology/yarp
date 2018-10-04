/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif

#include <yarp/sig/Sound.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>


using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("sig::SoundTest", "[yarp::sig]") {
    SECTION("check set/get sample.") {
        Sound snd;
        snd.resize(100,2);
        CHECK((size_t) 100 == snd.getSamples()); // "sample count"
        CHECK((size_t) 2 == snd.getChannels()); // "channel count"
        for (size_t i=0; i<snd.getSamples(); i++) {
            for (size_t j=0; j<2; j++) {
                snd.set(42,i,j);
            }
        }
        snd.set(99,50);
        CHECK(99 == snd.get(50)); // set/get
        Sound snd2(snd);
        CHECK((size_t) 100 == snd2.getSamples()); // sample count
        CHECK((size_t) 2 == snd2.getChannels()); // channel count
        CHECK(99 == snd2.get(50)); // copy works
    }

    SECTION("check set/get sample.") {
        Sound snd1, snd2, sndSum;
        snd1.resize(5, 8);
        snd2.resize(3, 8);

        for(size_t ch=0; ch<snd1.getChannels(); ch++)
        {
            for(size_t s=0; s<snd1.getSamples(); s++)
            {
                snd1.set(ch+s, s, ch);
            }
        }

        for(size_t ch=0; ch<snd2.getChannels(); ch++)
        {
            for(size_t s=0; s<snd2.getSamples(); s++)
            {
                snd2.set(1000+ch+s, s, ch);
            }
        }

        sndSum  = snd1;
        sndSum += snd2;

        // Checking sum is correct
        bool ok = true;
        size_t s1Samples = snd1.getSamples();
        size_t s2Samples = snd2.getSamples();

        for(size_t ch=0; ch<snd1.getChannels(); ch++)
        {
            for(size_t s=0; s<s1Samples; s++)
            {
                ok &= (sndSum.get(s, ch) == snd1.get(s, ch));
            }

            for(size_t s=0; s<s2Samples; s++)
            {
                ok &= (sndSum.get(s1Samples +s , ch) == snd2.get(s, ch));
            }
        }
        CHECK(ok); // "operator '+=' test performed "
    }

    SECTION("check sound transmission.") {

        Sound snd1;
        snd1.resize(128);
        snd1.setFrequency(50);
        for (size_t i=0; i<snd1.getSamples(); i++) {
            snd1.set(i-snd1.getSamples()/2,i);
        }

        PortReaderBuffer<Sound> buf;
        Port input, output;
        input.open("/in");
        output.open("/out");
        buf.setStrict();
        buf.attach(input);
        Network::connect("/out","/in");

        INFO("writing...");
        output.write(snd1);
        INFO("reading...");
        Sound *result = buf.read();

        CHECK(result!=nullptr); // "got something check")
        if (result!=nullptr) {
            CHECK(snd1.getSamples() == result->getSamples()); // "sample ct check
            CHECK(snd1.getFrequency() == result->getFrequency()); // "freq check"

            bool ok = true;
            for (size_t i=0; i<result->getSamples(); i++) {
                ok = ok && (result->get(i) == snd1.get(i));
            }
            CHECK(ok); // "sample values match"

        }

        output.close();
        input.close();
    }
    
}

