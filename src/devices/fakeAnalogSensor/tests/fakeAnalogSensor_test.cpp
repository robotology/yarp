/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IAnalogSensor.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeAnalogSensor", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeAnalogSensor", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeAnalogSensor device")
    {
        PolyDriver ddmc;
        yarp::dev::IAnalogSensor* ianalog=nullptr;

        ////////"Checking opening polydriver"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeAnalogSensor");
            p_cfg.put("period", "0.010");
            REQUIRE(ddmc.open(p_cfg));
        }

        ddmc.view(ianalog);
        CHECK(ianalog->getChannels()==1);

        //"Close all polydrivers and check"
        {
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
