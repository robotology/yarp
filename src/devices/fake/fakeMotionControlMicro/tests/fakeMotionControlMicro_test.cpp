/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IAxisInfoTest.h>
#include <yarp/dev/tests/IEncodersTimedTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeMotionControlMicro", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControlMicro", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeMotionControlMicro device")
    {
        PolyDriver ddmc;
        IAxisInfo* iaxis = nullptr;
        IEncodersTimed* ienc = nullptr;

        ////////"Checking opening polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControlMicro");
            REQUIRE(ddmc.open(p_cfg));
        }

        ddmc.view(iaxis); REQUIRE(iaxis);
        ddmc.view(ienc);  REQUIRE(ienc);
        yarp::dev::tests::exec_iAxisInfo_test_1(iaxis);
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);

        //"Close all polydrivers and check"
        {
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
