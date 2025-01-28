/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/ISpeechTranscription.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

#include <yarp/sig/Sound.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::speechTranscription_nwc_yarp", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSpeechTranscription", "device");
    YARP_REQUIRE_PLUGIN("speechTranscription_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("speechTranscription_nwc_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Checking speechTranscription_nwc_yarp device")
    {
        PolyDriver ddnws;
        PolyDriver ddfake;
        PolyDriver ddnwc;
        yarp::dev::ISpeechTranscription* iSpeech = nullptr;

        ////////"Checking opening speechTranscription_nwc_yarp polydrivers"
        {
            Property pnws_cfg;
            pnws_cfg.put("device", "speechTranscription_nws_yarp");
            pnws_cfg.put("name",  "/speechTranscription_nws_yarp");
            REQUIRE(ddnws.open(pnws_cfg));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakeSpeechTranscription");
            REQUIRE(ddfake.open(pdev_cfg));

            {yarp::dev::WrapperSingle* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddfake);
            REQUIRE(result_att); }

            Property pclient_cfg;
            pclient_cfg.put("device", "speechTranscription_nwc_yarp");
            pclient_cfg.put("local",  "/speechTranscription_nwc_yarp");
            pclient_cfg.put("remote", "/speechTranscription_nws_yarp");
            REQUIRE(ddnwc.open(pclient_cfg));
            REQUIRE(ddnwc.view(iSpeech));
        }

        yarp::os::Time::delay(0.1);

        CHECK (iSpeech->setLanguage("ita"));
        std::string lang;
        CHECK( iSpeech->getLanguage(lang));
        CHECK(lang == "ita");

        yarp::sig::Sound s;
        std::string text;
        double score;
        s.resize(100, 2);
        CHECK(iSpeech->transcribe(s,text,score));
        CHECK(text == "hello world");
        CHECK(score == 1);

        yarp::sig::Sound s2;
        CHECK(!iSpeech->transcribe(s2,text,score));

        //"Close all polydrivers and check"
        {
            CHECK(ddnwc.close());
            CHECK(ddnws.close());
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
