/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeFrameWriterTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeFrameWriter", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeFrameWriter device")
    {
        PolyDriver dd;

        yarp::os::ResourceFinder res;

        ////////"Checking opening polydriver"
        {
            Property cfg;
            cfg.put("device", "fakeFrameWriter");
            REQUIRE(dd.open(cfg));
        }

        yarp::os::Time::delay(1.0);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
