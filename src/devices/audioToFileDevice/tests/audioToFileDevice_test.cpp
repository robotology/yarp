/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IAudioRender.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::audioToFileDevice", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("audioToFileDevice", "device");

    Network::setLocalMode(true);

    SECTION("Checking audioToFileDevice device")
    {
        PolyDriver dd;
        yarp::dev::IAudioRender* iplay=nullptr;

        ////////"Checking opening audioToFileDevice polydriver"
        {
            Property p_cfg;
            p_cfg.put("device", "audioToFileDevice");
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
