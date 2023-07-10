/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ISpeechSynthesizer.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

#include <yarp/sig/Sound.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::speechSynthesizer_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSpeechSynthesizer", "device");
    YARP_REQUIRE_PLUGIN("speechSynthesizer_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("speechSynthesizer_nwc_yarp", "device");

    Network::setLocalMode(true);


    SECTION("Checking speechSynthesizer_nwc_yarp device")
    {
        PolyDriver ddnws;
        PolyDriver ddfake;
        PolyDriver ddnwc;
        yarp::dev::ISpeechSynthesizer* iSpeech = nullptr;

        ////////"Checking opening speechSynthesizer_nwc_yarp polydrivers"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "speechSynthesizer_nws_yarp");
            pnws_cfg.put("name",  "/speechSynthesizer_nws_yarp");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeSpeechSynthesizer");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws; ddnws.view(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }

            Property pclient_cfg;
            pclient_cfg.put("device", "speechSynthesizer_nwc_yarp");
            pclient_cfg.put("local",  "/speechSynthesizer_nwc_yarp");
            pclient_cfg.put("remote", "/speechSynthesizer_nws_yarp");
            REQUIRE(ddnwc.open(pclient_cfg));
            REQUIRE(ddnwc.view(iSpeech));
        }

        yarp::os::Time::delay(0.1);


        std::string text = "hello world";
        yarp::sig::Sound ss;
        CHECK(iSpeech->synthesize(text,ss));
        CHECK(ss.getChannels() == 2);
        CHECK(ss.getSamples() == 100);

        std::string voice = "voice_place_holder";
        std::string voiceReturned;
        double pitch = 100;
        double pitchReturned;
        double speed = 100;
        double speedReturned;

        CHECK(iSpeech->setVoice(voice));
        CHECK(iSpeech->getVoice(voiceReturned));
        CHECK(voice == voiceReturned);
        CHECK(iSpeech->setPitch(pitch));
        CHECK(iSpeech->getPitch(pitchReturned));
        CHECK(pitch == pitchReturned);
        CHECK(iSpeech->setSpeed(speed));
        CHECK(iSpeech->getSpeed(speedReturned));
        CHECK(voice == voiceReturned);

        //"Close all polydrivers and check"
        {
            CHECK(ddnwc.close());
            CHECK(ddnws.close());
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
