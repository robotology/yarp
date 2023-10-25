/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IAxisInfo.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IPositionControlTest.h>
#include <yarp/dev/tests/ITorqueControlTest.h>
#include <yarp/dev/tests/IAxisInfoTest.h>
#include <yarp/dev/tests/IEncodersTimedTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeMotionControl", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeMotionControl device")
    {
        PolyDriver ddmc;
        IPositionControl* ipos=nullptr;
        ITorqueControl* itrq=nullptr;
        IAxisInfo* iaxis = nullptr;
        IEncodersTimed* ienc = nullptr;

        ////////"Checking opening polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            REQUIRE(ddmc.open(p_cfg));
        }

        ddmc.view(ipos);
        ddmc.view(itrq);
        ddmc.view(iaxis);
        ddmc.view(ienc);
        yarp::dev::tests::exec_iPositionControl_test_1(ipos);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq);
        yarp::dev::tests::exec_iAxisInfo_test_1(iaxis);
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);

        //"Close all polydrivers and check"
        {
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
