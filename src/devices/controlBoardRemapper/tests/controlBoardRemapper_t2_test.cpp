/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IEncodersTimed.h>
#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IInteractionMode.h>
#include <yarp/dev/IMotorEncoders.h>
#include <yarp/dev/IMotor.h>
#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/IRemoteCalibrator.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperMultiple.h>
#include <yarp/dev/tests/IPositionControlTest.h>
#include <yarp/dev/tests/IVelocityControlTest.h>
#include <yarp/dev/tests/ITorqueControlTest.h>
#include <yarp/dev/tests/IEncodersTimedTest.h>
#include <yarp/dev/tests/IAxisInfoTest.h>
#include <yarp/dev/tests/IControlModeTest.h>
#include <yarp/dev/tests/IInteractionModeTest.h>
#include <yarp/dev/tests/ICurrentControlTest.h>
#include <yarp/dev/tests/IPWMControlTest.h>
#include <yarp/dev/tests/IPidControlTest.h>
#include <yarp/dev/tests/IMotorTest.h>
#include <yarp/dev/tests/IMotorEncodersTest.h>
#include <yarp/dev/tests/IRemoteCalibratorTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::ControlBoardRemapperTest2", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlboardremapper", "device");

    Network::setLocalMode(true);

    SECTION("Checking controlboardremapper device")
    {
        PolyDriver ddmc;
        PolyDriver ddremapper;

        IPositionControl* ipos = nullptr;
        IVelocityControl* ivel = nullptr;
        ITorqueControl* itrq = nullptr;
        IAxisInfo* iinfo = nullptr;
        IEncodersTimed* ienc = nullptr;
        IControlMode* icmd = nullptr;
        IInteractionMode* iint = nullptr;
        IMotor* imot = nullptr;
        IMotorEncoders* imotenc = nullptr;
        IPidControl* ipid = nullptr;
        IPWMControl* ipwm = nullptr;
        ICurrentControl* icurr = nullptr;
        //IRemoteCalibrator* iremotecalib = nullptr;

        ////////"Checking opening map2DServer and map2DClient polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 2);
            REQUIRE(ddmc.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlboardremapper");
            yarp::os::Value* jlist = yarp::os::Value::makeList("joint0 joint1");
            p_cfg.put("axesNames", jlist);
            REQUIRE(ddremapper.open(p_cfg));
        }
        {
            yarp::dev::IMultipleWrapper* ww_nws=nullptr; ddremapper.view(ww_nws);
            REQUIRE(ww_nws);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddmc,"fakeboard1");
            bool result_att = ww_nws->attachAll(pdlist);
            REQUIRE(result_att);
        }

        ddremapper.view(ipos);    REQUIRE(ipos);
        ddremapper.view(ivel);    REQUIRE(ivel);
        ddremapper.view(itrq);    REQUIRE(itrq);
        ddremapper.view(iinfo);   REQUIRE(iinfo);
        ddremapper.view(ienc);    REQUIRE(ienc);
        ddremapper.view(icmd);    REQUIRE(icmd);
        ddremapper.view(iint);    REQUIRE(iint);
        ddremapper.view(imot);    REQUIRE(imot);
        ddremapper.view(imotenc); REQUIRE(imotenc);
        ddremapper.view(ipid);    REQUIRE(ipid);
        ddremapper.view(ipwm);    REQUIRE(ipwm);
        ddremapper.view(icurr);   REQUIRE(icurr);
        //ddremapper.view(iremotecalib);  REQUIRE(iremotecalib);

        yarp::dev::tests::exec_iPositionControl_test_1(ipos, icmd);
        yarp::dev::tests::exec_iVelocityControl_test_1(ivel, icmd);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq, icmd);
        yarp::dev::tests::exec_iAxisInfo_test_1(iinfo);
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);
        yarp::dev::tests::exec_iControlMode_test_1(icmd, iinfo);
        yarp::dev::tests::exec_iInteractionMode_test_1(iint);
        yarp::dev::tests::exec_iMotor_test_1(imot);
        yarp::dev::tests::exec_iMotorEncoders_test_1(imotenc);
        yarp::dev::tests::exec_iPidControl_test_1(ipid);
        yarp::dev::tests::exec_iPwmControl_test_1(ipwm, icmd);
        yarp::dev::tests::exec_iCurrentControl_test_1(icurr, icmd);
        //yarp::dev::tests::exec_iRemoteCalibrator_test_1(iremotecalib);

        //"Close all polydrivers and check"
        {
            CHECK(ddremapper.close());
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
