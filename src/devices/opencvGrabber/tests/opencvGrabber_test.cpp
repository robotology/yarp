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

TEST_CASE("dev::opencvGrabberTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("opencv_grabber", "device");

    Network::setLocalMode(true);

    SECTION("Checking opencv_grabber device, opening an avi file")
    {
        PolyDriver dd;

        yarp::os::ResourceFinder res;
        res.setDefaultContext("tests/opencvGrabber");
        std::string filepath = res.findFileByName("test.avi");

        ////////"Checking opening polydriver"
        {
            Property cfg;
            cfg.put("device", "opencv_grabber");
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
