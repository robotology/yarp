/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IAudioRender.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeSpeaker", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeSpeaker", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeSpeaker device")
    {
        PolyDriver dd;
        yarp::dev::IAudioRender* iplay = nullptr;

        ////////"Checking opening device polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeSpeaker");
            REQUIRE(dd.open(p_cfg));
        }

        dd.view(iplay);
        CHECK(iplay->startPlayback());
        CHECK(iplay->stopPlayback());

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
