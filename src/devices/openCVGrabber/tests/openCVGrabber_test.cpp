/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

TEST_CASE("dev::openCVGrabberTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("openCVGrabber", "device");

    Network::setLocalMode(true);

    SECTION("Checking openCVGrabber device, opening an avi file")
    {
        PolyDriver dd;

        yarp::os::ResourceFinder res;
        res.setDefaultContext("tests/openCVGrabber");
        std::string filepath = res.findFileByName("test.avi");

        ////////"Checking opening polydriver"
        {
            Property cfg;
            cfg.put("device", "openCVGrabber");
            cfg.put("movie", filepath);
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
