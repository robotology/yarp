/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/tests/IPositionControlTest.h>
#include <yarp/dev/tests/ITorqueControlTest.h>
#include <yarp/dev/tests/IEncodersTimedTest.h>
#include <yarp/dev/tests/IVelocityControlTest.h>
#include <yarp/dev/tests/IAxisInfoTest.h>
#include <yarp/dev/tests/IControlModeTest.h>
#include <yarp/dev/tests/IInteractionModeTest.h>
#include <yarp/dev/tests/ICurrentControlTest.h>
#include <yarp/dev/tests/IImpedanceControlTest.h>
#include <yarp/dev/tests/IPWMControlTest.h>
#include <yarp/dev/tests/IPidControlTest.h>
#include <yarp/dev/tests/IMotorTest.h>
#include <yarp/dev/tests/IMotorEncodersTest.h>
#include <yarp/dev/tests/IRemoteCalibratorTest.h>
#include <yarp/dev/tests/IJointFaultTest.h>
#include <yarp/dev/tests/IControlLimitsTest.h>
#include <yarp/dev/tests/IJointBrakeTest.h>
#include <yarp/dev/tests/IPositionDirectTest.h>
#include <yarp/dev/tests/IVelocityDirectTest.h>

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
        IInteractionMode* iint = nullptr;
        IMotor* imot = nullptr;
        IMotorEncoders* imotenc = nullptr;
        IPidControl* ipid = nullptr;
        IPWMControl* ipwm = nullptr;
        ICurrentControl* icurr = nullptr;
        IJointFault* ifault = nullptr;
        IControlLimits* ilims = nullptr;
        IImpedanceControl* iimp = nullptr;
        //IRemoteCalibrator* iremotecalib = nullptr;
        IJointBrake* ibrake = nullptr;
        IPositionDirect* iposdir = nullptr;
        IVelocityDirect* iveldir = nullptr;

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

        yarp::os::Time::delay(0.9);

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
        ddnwc.view(ipos);    REQUIRE(ipos);
        ddnwc.view(ivel);    REQUIRE(ivel);
        ddnwc.view(itrq);    REQUIRE(itrq);
        ddnwc.view(iinfo);   REQUIRE(iinfo);
        ddnwc.view(ienc);    REQUIRE(ienc);
        ddnwc.view(icmd);    REQUIRE(icmd);
        ddnwc.view(iint);    REQUIRE(iint);
        ddnwc.view(imot);    REQUIRE(imot);
        ddnwc.view(imotenc); REQUIRE(imotenc);
        ddnwc.view(ipid);    REQUIRE(ipid);
        ddnwc.view(ipwm);    REQUIRE(ipwm);
        ddnwc.view(icurr);   REQUIRE(icurr);
        ddnwc.view(ifault);  REQUIRE(ifault);
        ddnwc.view(ilims);   REQUIRE(ilims);
        ddnwc.view(ibrake);  REQUIRE(ibrake);
        ddnwc.view(iposdir);  REQUIRE(iposdir);
        ddnwc.view(iveldir);  REQUIRE(iveldir);
        //ddnwc.view(icalib);  REQUIRE(iremotecalib);

        yarp::dev::tests::exec_iPositionControl_test_1(ipos,icmd);
        yarp::dev::tests::exec_iVelocityControl_test_1(ivel,icmd);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq,icmd);
        yarp::dev::tests::exec_iAxisInfo_test_1(iinfo);
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);
        yarp::dev::tests::exec_iControlMode_test_1(icmd, iinfo);
        yarp::dev::tests::exec_iInteractionMode_test_1(iint, iinfo);
        yarp::dev::tests::exec_iMotor_test_1(imot);
        yarp::dev::tests::exec_iMotorEncoders_test_1(imotenc);
        yarp::dev::tests::exec_iPidControl_test_1(ipid, iinfo);
        yarp::dev::tests::exec_iPidControl_test_2(ipid, iinfo);
        yarp::dev::tests::exec_iPwmControl_test_1(ipwm,icmd);
        yarp::dev::tests::exec_iCurrentControl_test_1(icurr,icmd);
        //yarp::dev::tests::exec_iRemoteCalibrator_test_1(icalib);
        yarp::dev::tests::exec_iJointFault_test_1(ifault);
        yarp::dev::tests::exec_iControlLimits_test1(ilims, iinfo);
        yarp::dev::tests::exec_iJointBrake_test1(ibrake);
        yarp::dev::tests::exec_iPositionDirect_test_1(iposdir, icmd);
        yarp::dev::tests::exec_iVelocityDirect_test_1(iveldir, icmd);

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

        yarp::os::Time::delay(0.9);

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
