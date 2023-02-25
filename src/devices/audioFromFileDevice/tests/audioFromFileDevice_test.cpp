/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IAudioGrabberSound.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/ResourceFinder.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::audioFromFileDevice", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("audioFromFileDevice", "device");

    Network::setLocalMode(true);

    SECTION("Checking audioFromFileDevice device")
    {
        PolyDriver dd;
        yarp::dev::IAudioGrabberSound* igrb=nullptr;

        ////////"Checking opening audioFromFileDevice polydriver"
        {
            yarp::os::ResourceFinder res;
            res.setDefaultContext("tests/audioFromFileDevice");
            std::string filepath = res.findFileByName("440.wav");
#if 0
            std::string cpp_path = __FILE__;
            std::size_t pos = cpp_path.find("audioFromFileDevice_test.cpp");
            if (pos != std::string::npos)
            {
                cpp_path.erase(pos, 28);
            }
#endif
            Property p_cfg;
            p_cfg.put("device", "audioFromFileDevice");
            Property& p_cfg_audio_base = p_cfg.addGroup("AUDIO_BASE");
            p_cfg_audio_base.put("channels", 1);
            p_cfg.put("file_name", filepath);
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(igrb);
        CHECK(igrb->startRecording());
        CHECK(igrb->stopRecording());

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
