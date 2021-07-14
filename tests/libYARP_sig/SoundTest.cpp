/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Log.h>
#include <fstream>
#include <iostream>

#include <catch.hpp>
#include <harness.h>
#include <sstream>
#include <vector>

using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::os;

void generate_test_sound(Sound& snd, size_t samples, size_t size_channels)
{
    for (size_t ch = 0; ch<snd.getChannels(); ch++)
    {
        for (size_t s = 0; s<snd.getSamples(); s++)
        {
            snd.set((Sound::audio_sample)(ch * 10 + s), s, ch);
        }
    }
}

TEST_CASE("sig::SoundTest", "[yarp::sig]")
{
    NetworkBase::setLocalMode(true);

    SECTION("check getRawDataSize and padding")
    {
        Sound snd1;
        size_t siz_chans;
        size_t siz_samps;
        snd1.resize(10, 3);
        siz_chans = snd1.getChannels();
        siz_samps = snd1.getSamples();
        auto v1 = snd1.getInterleavedAudioRawData();
        auto v2 = snd1.getNonInterleavedAudioRawData();
        CHECK(siz_chans == 3);
        CHECK(siz_samps == 10);
        CHECK(v1.size() == 30);
        CHECK(v2.size() == 30);
    }

    SECTION("check set/get sample.")
    {
        Sound snd;
        snd.resize(100,2);
        CHECK((size_t) 100 == snd.getSamples()); // "sample count"
        CHECK((size_t)2 == snd.getChannels()); // "channel count"
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

    SECTION("check operator ==")
    {
        bool ok = true;
        Sound snd1, snd2;
        snd1.resize(10, 3);
        generate_test_sound(snd1, 10, 3);

        for (auto i = 0; i < 10; i++) {
            for (auto j = 0; j < 3; j++)
            {
                ok = true;
                snd2 = snd1;
                ok &= (snd1 == snd2);
                CHECK(ok);
                snd2.setSafe(1000, i, j);
                ok &= (snd1 == snd2);
                CHECK(!ok);
            }
        }
    }

    SECTION("check replace channel.")
    {
        Sound snd1;
        snd1.resize(10, 3);
        generate_test_sound(snd1, 10, 3);

        Sound snd_chan;
        snd_chan.resize (10, 1);
        generate_test_sound(snd_chan, 10, 1);

        Sound snd3;
        snd3.resize(10, 3);
        generate_test_sound(snd3, 10, 3);
        for (size_t s = 0; s<snd3.getSamples(); s++)
        {
            snd3.setSafe((Sound::audio_sample)(s), s, 2);
        }

        snd1.replaceChannel(2, snd_chan);

        bool ok = (snd1 == snd3);
        CHECK(ok);
    }

    SECTION("check subSound.")
    {
        Sound snd1;
        snd1.resize(10, 3);
        generate_test_sound(snd1, 10, 3);
        Sound subsnd1 = snd1.subSound(3, 7); //this includes samples 3 4 5 6

        Sound snd2;
        snd2.resize(4, 3);
        for (size_t ch = 0; ch<snd2.getChannels(); ch++)
        {
            for (size_t s = 0; s<snd2.getSamples(); s++)
            {
                snd2.set((Sound::audio_sample)(ch * 10 + s + 3), s, ch);
            }
        }
        bool ok = (subsnd1 == snd2);
        CHECK(ok);
    }

    SECTION("check extractChannel.")
    {
        Sound snd1;
        snd1.resize(10, 3);
        generate_test_sound(snd1, 10, 3);

        for (size_t chan=0; chan<3; chan++)
        {
            bool ok = true;
            Sound sndext = snd1.extractChannelAsSound(chan);
            Sound snd3;
            snd3.resize(10, 1);
            for (size_t s = 0; s < snd3.getSamples(); s++)
            {
                snd3.set((Sound::audio_sample)(chan * 10 + s), s, 0);
            }
            ok = (sndext == snd3);
            CHECK(ok);
        }
    }

    SECTION("check amplify.")
    {
        double gain = 2;
        Sound snd1;
        snd1.resize(10, 3);
        generate_test_sound(snd1, 10, 3);

        Sound snd2 = snd1;
        snd2.amplify(gain);

        Sound snd3 = snd1;
        snd3.amplifyChannel(1, gain);

        bool ok=true;
        for (size_t s = 0; s < snd2.getSamples(); s++)
        {
            ok &= ((snd2.get(s, 0) == snd1.get(s, 0) * gain));
            ok &= ((snd2.get(s, 1) == snd1.get(s, 1) * gain));
            ok &= ((snd2.get(s, 2) == snd1.get(s, 2) * gain));
        }
        CHECK(ok);
        for (size_t s = 0; s < snd3.getSamples(); s++)
        {
            ok &= ((snd3.get(s, 0) == snd1.get(s, 0) * 1.0));
            ok &= ((snd3.get(s, 1) == snd1.get(s, 1) * gain));
            ok &= ((snd3.get(s, 2) == snd1.get(s, 2) * 1.0));
        }
        CHECK(ok);
    }

    SECTION("check findPeak.")
    {
        bool ok;
        Sound snd1;
        snd1.resize(4, 2);
        snd1.set(10,0,0);  snd1.set(20, 1, 0); snd1.set(30, 2, 0); snd1.set(-10, 3, 0);
        snd1.set(15,0,1);  snd1.set(35, 1, 1); snd1.set(25, 2, 1); snd1.set(-15, 3, 1);
        size_t cid=0;
        size_t sid=0;
        short v=0;
        snd1.findPeak(cid,sid,v);
        ok = (cid == 1) && (sid == 1) && (v == 35);
        CHECK(ok);
        snd1.findPeakInChannel(0, sid, v);
        ok = (sid == 2) && (v == 30);
        CHECK(ok);
        snd1.findPeakInChannel(1, sid, v);
        ok = (sid == 1) && (v == 35);
        CHECK(ok);
    }

    SECTION("check normalize.")
    {
        Sound snd1;
        Sound snt2;
        Sound snt3;
        snd1.resize(4, 2);
        snt2.resize(4, 2);
        snt3.resize(4, 2);
        snd1.set(10, 0, 0);     snd1.set(20, 1, 0);    snd1.set(30, 2, 0);    snd1.set(-10, 3, 0);
        snd1.set(15, 0, 1);     snd1.set(35, 1, 1);    snd1.set(25, 2, 1);    snd1.set(-15, 3, 1);

        snt2.set(9362,  0, 0);  snt2.set(18724, 1, 0); snt2.set(28086, 2, 0); snt2.set(-9362, 3, 0);
        snt2.set(14043, 0, 1);  snt2.set(32767, 1, 1); snt2.set(23405, 2, 1); snt2.set(-14043, 3, 1);

        snt3.set(10922, 0, 0);  snt3.set(21844, 1, 0); snt3.set(32767, 2, 0); snt3.set(-10922, 3, 0);
        snt3.set(15, 0, 1);     snt3.set(35, 1, 1);    snt3.set(25, 2, 1);    snt3.set(-15, 3, 1);
        Sound snd2 = snd1;
        snd2.normalize();
        CHECK (snd2==snt2);
        Sound snd3 = snd1;
        snd3.normalizeChannel(0);
        CHECK(snd3 == snt3);
    }

    SECTION("check set/get sample.")
    {
        Sound snd1, snd2, sndSum;
        snd1.resize(5, 8);
        snd2.resize(3, 8);

        for(size_t ch=0; ch<snd1.getChannels(); ch++)
        {
            for(size_t s=0; s<snd1.getSamples(); s++)
            {
                snd1.set((Sound::audio_sample)(ch+s), s, ch);
            }
        }

        for(size_t ch=0; ch<snd2.getChannels(); ch++)
        {
            for(size_t s=0; s<snd2.getSamples(); s++)
            {
                snd2.set((Sound::audio_sample)(1000+ch+s), s, ch);
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

    SECTION("check serialization methods")
    {
        Sound snd1;
        snd1.resize(5, 2);
        generate_test_sound(snd1, 5, 2);

        std::vector<int> test_vec_i = { 0, 10, 1, 11, 2, 12, 3, 13, 4, 14 };
        std::vector<int> test_vec_ni = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14 };
        auto vec_i = snd1.getInterleavedAudioRawData();
        auto vec_ni = snd1.getNonInterleavedAudioRawData();

        bool ok;
        ok = (vec_i.size() == 5 * 2);
        CHECK(ok);
        ok = (vec_ni.size() == 5 * 2);
        CHECK(ok);
        for (size_t i = 0; i < vec_i.size(); i++)
        {
            ok = (vec_i.at(i).get() == test_vec_i.at(i));
            CHECK(ok);
        }
        for (size_t i = 0; i < vec_ni.size(); i++)
        {
            ok = (vec_ni.at(i).get() == test_vec_ni.at(i));
            CHECK(ok);
        }
        auto s1 = std::vector<short>(vec_i.begin(), vec_i.end());
        short* samples1 = s1.data();
        for (size_t i = 0; i < vec_i.size(); i++)
        {
            ok = (samples1[i] == test_vec_i.at(i));
            CHECK(ok);
        }
        auto s2 = std::vector<short>(vec_ni.begin(), vec_ni.end());
        short* samples2 = s2.data();
        for (size_t i = 0; i < vec_ni.size(); i++)
        {
            ok = (samples2[i] == test_vec_ni.at(i));
            CHECK(ok);
        }

        std::string str = snd1.toString();
        yDebug("sound snd1.toSring():");
        yDebug("%s", str.c_str());
    }

    SECTION("check sound transmission.")
    {

        Sound snd1;
        snd1.resize(128);
        snd1.setFrequency(50);
        for (size_t i=0; i<snd1.getSamples(); i++) {
            snd1.set((Sound::audio_sample)(i-snd1.getSamples()/2),i);
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

    SECTION("check write file.")
    {
        Sound snd1;
        snd1.resize(30000, 2);
        snd1.setFrequency(44100);
        generate_test_sound(snd1, 30000, 2);
        bool b1 = yarp::sig::file::write(snd1, "testmp3.mp3");
        #ifdef YARP_MP3_SUPPORTED
            CHECK (b1);
        #endif
        if (!b1)
        {
            WARN("Failed to write file, test skipped");
        }
        bool b2 = yarp::sig::file::write(snd1, "testwav.wav");
        if (!b2)
        {
            WARN("Failed to write file, test skipped");
        }
    }

    SECTION("check read file.")
    {
        Sound snd1;
        bool b1 = yarp::sig::file::read(snd1, "testmp3.mp3");
        #ifdef YARP_MP3_SUPPORTED
            CHECK(b1);
            //CHECK(snd1.getSamples() == 30000);
            CHECK(snd1.getFrequency() == 44100);
            CHECK(snd1.getBytesPerSample() == 2);
        #endif
        if (!b1)
        {
            WARN("Failed to read file, test skipped");
        }

        Sound snd2;
        bool b2 =yarp::sig::file::read(snd2, "testwav.wav");
        if (b2)
        {
            CHECK(snd2.getSamples() == 30000);
            CHECK(snd2.getFrequency() == 44100);
            CHECK(snd2.getBytesPerSample() == 2);
        }
        else
        {
            WARN("Failed to read file, test skipped");
        }
    }

    SECTION("check conversions")
    {
        #ifdef YARP_MP3_SUPPORTED
            Sound snd1;
            yarp::sig::file::read (snd1, "testmp3.mp3");
            yarp::sig::file::write(snd1, "testmp3b.mp3");
            yarp::sig::file::write(snd1, "testmp3towav.wav");
            Sound snd2;
            yarp::sig::file::read (snd2, "testwav.wav");
            yarp::sig::file::write(snd2, "testwavb.wav");
            yarp::sig::file::write(snd2, "testwavtomp3.mp3");
        #endif
    }

    SECTION("check read bytestream.")
    {
        std::ifstream is;
        is.open("testmp3.mp3", std::ios::binary);
        #ifdef YARP_MP3_SUPPORTED
            CHECK (is.is_open());
        #endif
        if (is.is_open())
        {
            // get length of file and allocate space
            is.seekg(0, std::ios::end);
            size_t length = is.tellg();
            is.seekg(0, std::ios::beg);
            char* buffer = new char[length];
            // read data as a block:
            is.read(buffer, length);
            is.close();

            Sound snd1;
            yarp::sig::file::read_bytestream(snd1, buffer, length, ".mp3");
            yarp::sig::file::write(snd1, "testmp3c.mp3");
        }
        else
        {
            WARN("Failed to open file, test skipped");
        }
    }

    NetworkBase::setLocalMode(false);
}
