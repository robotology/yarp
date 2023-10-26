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

TEST_CASE("dev::RemoteControlBoardTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("remote_controlboard", "device");

    Network::setLocalMode(true);

    SECTION("Checking remote_controlboard device (using fakeMotionControl)")
    {
        PolyDriver ddmc;
        PolyDriver ddnws;
        PolyDriver ddnwc;

        IPositionControl* ipos = nullptr;
        IVelocityControl* ivel = nullptr;
        ITorqueControl* itrq = nullptr;
        IAxisInfo* iinfo = nullptr;
        IEncodersTimed* ienc = nullptr;
        IControlMode* icmd = nullptr;

        ////////"Checking opening fakeMotionControl and controlBoard_nws_yarp polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 2);
            REQUIRE(ddmc.open(p_cfg));
        }
        yarp::os::Time::delay(0.1);
        {
            Property p_cfg;
            p_cfg.put("device", "controlBoard_nws_yarp");
            p_cfg.put("name", "/controlboardserver");
            REQUIRE(ddnws.open(p_cfg));
        }

        //attach nws and fake
        {
            yarp::dev::WrapperSingle* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddmc);
            REQUIRE(result_att);
        }

        yarp::os::Time::delay(0.1);

        //open the nwc
        {
            Property p_cfg;
            p_cfg.put("device", "remote_controlboard");
            p_cfg.put("local", "/local_controlboard");
            p_cfg.put("remote", "/controlboardserver");
            REQUIRE(ddnwc.open(p_cfg));
        }

        yarp::os::Time::delay(0.1);

        //test
        ddnwc.view(ipos);  REQUIRE(ipos);
        ddnwc.view(ivel);  REQUIRE(ivel);
        ddnwc.view(itrq);  REQUIRE(itrq);
        ddnwc.view(iinfo); REQUIRE(iinfo);
        ddnwc.view(ienc);  REQUIRE(ienc);
        ddnwc.view(icmd);  REQUIRE(icmd);
        yarp::dev::tests::exec_iPositionControl_test_1(ipos,icmd);
        yarp::dev::tests::exec_iVelocityControl_test_1(ivel,icmd);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq,icmd);
        yarp::dev::tests::exec_iAxisInfo_test_1(iinfo);
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);

        //"Close all polydrivers and check"
        {
            CHECK(ddnwc.close());
            CHECK(ddnws.close());
            CHECK(ddmc.close());
        }
    }

    SECTION("Checking remote_controlboard device (using fakeMotionControlMicro)")
    {
        PolyDriver ddmc;
        PolyDriver ddnws;
        PolyDriver ddnwc;

        IPositionControl* ipos = nullptr;
        IVelocityControl* ivel = nullptr;
        ITorqueControl* itrq = nullptr;
        IAxisInfo* iinfo = nullptr;
        IEncodersTimed* ienc = nullptr;
        IControlMode* icmd = nullptr;

        ////////"Checking opening fakeMotionControl and controlBoard_nws_yarp polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControlMicro");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 2);
            REQUIRE(ddmc.open(p_cfg));
        }
        yarp::os::Time::delay(0.1);
        {
            Property p_cfg;
            p_cfg.put("device", "controlBoard_nws_yarp");
            p_cfg.put("name", "/controlboardserver");
            REQUIRE(ddnws.open(p_cfg));
        }

        //attach nws and fake
        {
            yarp::dev::WrapperSingle* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddmc);
            REQUIRE(result_att);
        }

        yarp::os::Time::delay(0.1);

        //open the nwc
        {
            Property p_cfg;
            p_cfg.put("device", "remote_controlboard");
            p_cfg.put("local", "/local_controlboard");
            p_cfg.put("remote", "/controlboardserver");
            REQUIRE(ddnwc.open(p_cfg));
        }

        yarp::os::Time::delay(0.1);

        //test
        ddnwc.view(ipos);  REQUIRE(ipos);
        ddnwc.view(ivel);  REQUIRE(ivel);
        ddnwc.view(itrq);  REQUIRE(itrq);
        ddnwc.view(iinfo); REQUIRE(iinfo);
        ddnwc.view(ienc);  REQUIRE(ienc);
        ddnwc.view(icmd);  REQUIRE(icmd);
        yarp::dev::tests::exec_iPositionControl_test_unimplemented_interface(ipos, icmd);
        yarp::dev::tests::exec_iVelocityControl_test_unimplemented_interface(ivel, icmd);
        yarp::dev::tests::exec_iTorqueControl_test_unimplemented_interface(itrq, icmd);
        yarp::dev::tests::exec_iAxisInfo_test_1(iinfo);
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);

        //"Close all polydrivers and check"
        {
            CHECK(ddnwc.close());
            CHECK(ddnws.close());
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
