/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IJointCoupling.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeJointCoupling", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeJointCoupling", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeJointCoupling device")
    {
        PolyDriver ddjc;
        IJointCoupling* ijc=nullptr;
        {
            Property p_cfg;
            p_cfg.put("device", "fakeJointCoupling");
            REQUIRE(ddjc.open(p_cfg));
        }

        REQUIRE(ddjc.view(ijc));

        //"Close all polydrivers and check"
        {
            CHECK(ddjc.close());
        }
    }

    Network::setLocalMode(false);
}
