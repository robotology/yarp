/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ISpeechTranscription.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/ResourceFinder.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakePythonSpeechTranscription", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakePythonSpeechTranscription", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakePythonSpeechTranscription device")
    {
        yarp::dev::ISpeechTranscription* istr=nullptr;
        PolyDriver ddfake;

        //open the device
        {
            yarp::os::ResourceFinder res;
            res.setDefaultContext("tests/fakePythonSpeechTranscription");
            std::string filepath = res.findFileByName("Module.py");
            std::string parentPath = filepath.substr(0, filepath.find_last_of("\\/"));

            Property pdev_cfg;
            pdev_cfg.put("device", "fakePythonSpeechTranscription");
            pdev_cfg.put("moduleName", "Module");
            pdev_cfg.put("modulePath", parentPath);
            pdev_cfg.put("className", "SpeechTranscriptor");
            REQUIRE(ddfake.open(pdev_cfg));
            REQUIRE(ddfake.view(istr));
        }

        std::string lang = "eng";
        CHECK(istr->getLanguage(lang));     // the default value initialized by the class is auto
        CHECK(lang=="auto");

        std::string new_lang = "eng";
        CHECK(istr->setLanguage(new_lang));
        CHECK(istr->getLanguage(lang));
        CHECK(lang == "eng");

        std::string transcript;
        double score;
        yarp::sig::Sound snd;
        snd.resize(100, 2);
        CHECK(istr->transcribe(snd,transcript, score));
        CHECK(transcript=="hello world");
        CHECK(score > 0.99);

        //"Close all polydrivers and check"
        {
            yarp::os::Time::delay(0.1);
            CHECK(ddfake.close());
        }
    }

    Network::setLocalMode(false);
}
