/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IAudioGrabberSound.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeMicrophone", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMicrophone", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeMicrophone device")
    {
        PolyDriver dd;
        yarp::dev::IAudioGrabberSound* igrb = nullptr;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMicrophone");
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
