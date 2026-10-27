/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ISpeechSynthesizer.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/ParametersTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeSpeechSynthesizer", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSpeechSynthesizer", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeSpeechSynthesizer device")
    {
        yarp::dev::ISpeechSynthesizer* istr=nullptr;
        PolyDriver ddfake;

        //open the device
        {
            Property pdev_cfg;
            pdev_cfg.put("device", "fakeSpeechSynthesizer");
            REQUIRE(ddfake.open(pdev_cfg));
            yarp::dev::tests::exec_params_test(&ddfake);

            REQUIRE(ddfake.view(istr));
        }

        std::string lang = "eng";
        CHECK(istr->getLanguage(lang));
        CHECK(lang=="auto");

        CHECK(istr->setLanguage("eng"));
        CHECK(istr->getLanguage(lang));
        CHECK(lang == "eng");

        yarp::sig::Sound snd;
        CHECK(istr->synthesize("hello world",snd));
        CHECK(snd.getChannels()==2);
        CHECK(snd.getSamples()==100);

        yarp::sig::VectorOfString available_voices;
        CHECK(istr->getAvailableVoices(available_voices));
        CHECK(available_voices.size() == 3);
        CHECK(available_voices[0] == "voice1");
        CHECK(available_voices[1] == "voice2");
        CHECK(available_voices[2] == "voice3");

        //"Close all polydrivers and check"
        {
            yarp::os::Time::delay(0.1);
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
