/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IRangefinder2DTest.h>
#include <yarp/dev/tests/IPositionControlTest.h>
#include <yarp/dev/tests/IControlModeTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

TEST_CASE("dev::FakeLaserWithMotorTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeLaserWithMotor", "device");

    Network::setLocalMode(true);

    SECTION("Checking FakeLaserTest device")
    {
        PolyDriver fakelaserdev;
        IRangefinder2D* irng = nullptr;
        IPositionControl* ipos = nullptr;
        IControlMode* icmd = nullptr;
        IAxisInfo* iinfo = nullptr;

        ////////"Checking opening polydriver"
        {
            Property las_cfg;
            las_cfg.put("device", "fakeLaserWithMotor");
            las_cfg.put("test", "use_constant");
            Property& cm_cfg = las_cfg.addGroup("CONSTANT_MODE");
            cm_cfg.put("const_distance", 0.5);
            REQUIRE(fakelaserdev.open(las_cfg));
            REQUIRE(fakelaserdev.view(irng));
            REQUIRE(fakelaserdev.view(ipos));
            REQUIRE(fakelaserdev.view(icmd));
            REQUIRE(fakelaserdev.view(iinfo));
        }

        //execute tests
        yarp::dev::tests::exec_iRangefinder2D_test_1(irng);
        yarp::dev::tests::exec_iPositionControl_test_1(ipos, icmd);
        //yarp::dev::tests::exec_iControlMode_test_1(icmd,iinfo);

        //"Close all polydrivers and check"
        CHECK(fakelaserdev.close());
    }

    Network::setLocalMode(false);
}
