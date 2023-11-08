/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IAxisInfo.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IPositionControlTest.h>
#include <yarp/dev/tests/ITorqueControlTest.h>
#include <yarp/dev/tests/IAxisInfoTest.h>
#include <yarp/dev/tests/IEncodersTimedTest.h>
#include <yarp/dev/tests/IVelocityControlTest.h>

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
        IVelocityControl* ivel = nullptr;
        ITorqueControl* itrq=nullptr;
        IAxisInfo* iaxis = nullptr;
        IEncodersTimed* ienc = nullptr;
        IControlMode* icmd = nullptr;

        ////////"Checking opening polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            REQUIRE(ddmc.open(p_cfg));
        }

        ddmc.view(ipos);  REQUIRE(ipos);
        ddmc.view(ivel);  REQUIRE(ivel);
        ddmc.view(itrq);  REQUIRE(itrq);
        ddmc.view(iaxis); REQUIRE(iaxis);
        ddmc.view(ienc);  REQUIRE(ienc);
        ddmc.view(icmd);  REQUIRE(icmd);
        yarp::dev::tests::exec_iPositionControl_test_1(ipos, icmd);
        yarp::dev::tests::exec_iVelocityControl_test_1(ivel, icmd);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq, icmd);
        yarp::dev::tests::exec_iAxisInfo_test_1(iaxis);
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);

        //"Close all polydrivers and check"
        {
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
