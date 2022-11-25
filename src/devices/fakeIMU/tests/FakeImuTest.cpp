/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IOrientationSensorsTest.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeImu", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeImu", "device");

    Network::setLocalMode(true);

    SECTION("Checking map2D_nws_yarp device")
    {
        PolyDriver ddmc;
        yarp::dev::IOrientationSensors* iimu=nullptr;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            p_cfg.put("constantValue", 1);
            REQUIRE(ddmc.open(p_cfg));
        }

        ddmc.view(iimu);
        yarp::dev::tests::exec_IOrientationSensors_test_1(iimu);

        //"Close all polydrivers and check"
        {
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
