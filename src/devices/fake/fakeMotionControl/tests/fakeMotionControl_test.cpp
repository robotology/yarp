/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

#include <yarp/dev/tests/IPositionControlTest.h>
#include <yarp/dev/tests/ITorqueControlTest.h>
#include <yarp/dev/tests/IAxisInfoTest.h>
#include <yarp/dev/tests/IEncodersTimedTest.h>
#include <yarp/dev/tests/IVelocityControlTest.h>
#include <yarp/dev/tests/IAxisInfoTest.h>
#include <yarp/dev/tests/IControlModeTest.h>
#include <yarp/dev/tests/IInteractionModeTest.h>
#include <yarp/dev/tests/ICurrentControlTest.h>
#include <yarp/dev/tests/IPWMControlTest.h>
#include <yarp/dev/tests/IPidControlTest.h>
#include <yarp/dev/tests/IMotorTest.h>
#include <yarp/dev/tests/IMotorEncodersTest.h>
#include <yarp/dev/tests/IRemoteCalibratorTest.h>
#include <yarp/dev/tests/IJointFaultTest.h>
#include <yarp/dev/tests/IJointBrakeTest.h>
#include <yarp/dev/tests/IControlLimitsTest.h>
#include <yarp/dev/tests/IVelocityDirectTest.h>

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
        IJointBrake* ibrake = nullptr;
        IVelocityDirect* iveldir = nullptr;

        //IRemoteCalibrator* icalib = nullptr;

        ////////"Checking opening polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 2);
            REQUIRE(ddmc.open(p_cfg));
        }

        ddmc.view(ipos);    REQUIRE(ipos);
        ddmc.view(ivel);    REQUIRE(ivel);
        ddmc.view(itrq);    REQUIRE(itrq);
        ddmc.view(iinfo);   REQUIRE(iinfo);
        ddmc.view(ienc);    REQUIRE(ienc);
        ddmc.view(icmd);    REQUIRE(icmd);
        ddmc.view(iint);    REQUIRE(iint);
        ddmc.view(imot);    REQUIRE(imot);
        ddmc.view(imotenc); REQUIRE(imotenc);
        ddmc.view(ipid);    REQUIRE(ipid);
        ddmc.view(ipwm);    REQUIRE(ipwm);
        ddmc.view(icurr);   REQUIRE(icurr);
        ddmc.view(ifault);  REQUIRE(ifault);
        ddmc.view(ilims);   REQUIRE(ilims);
        ddmc.view(ibrake);  REQUIRE(ibrake);
        ddmc.view(iveldir); REQUIRE(iveldir);
        //ddmc.view(iremotecalib);  REQUIRE(iremotecalib);

        yarp::dev::tests::exec_iPositionControl_test_1(ipos, icmd);
        yarp::dev::tests::exec_iVelocityControl_test_1(ivel, icmd);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq, icmd);
        yarp::dev::tests::exec_iAxisInfo_test_1(iinfo);
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);
        yarp::dev::tests::exec_iControlMode_test_1(icmd, iinfo);
        yarp::dev::tests::exec_iInteractionMode_test_1(iint, iinfo);
        yarp::dev::tests::exec_iMotor_test_1(imot);
        yarp::dev::tests::exec_iMotorEncoders_test_1(imotenc);
        yarp::dev::tests::exec_iPidControl_test_1(ipid, iinfo);
        yarp::dev::tests::exec_iPidControl_test_2(ipid);
        yarp::dev::tests::exec_iPwmControl_test_1(ipwm, icmd);
        yarp::dev::tests::exec_iCurrentControl_test_1(icurr, icmd);
        //yarp::dev::tests::exec_iRemoteCalibrator_test_1(iremotecalib);
        yarp::dev::tests::exec_iJointFault_test_1(ifault);
        yarp::dev::tests::exec_iControlLimits_test1(ilims, iinfo);
        yarp::dev::tests::exec_iJointBrake_test1(ibrake);
        yarp::dev::tests::exec_iVelocityDirect_test_1(iveldir, icmd);

        //"Close all polydrivers and check"
        {
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
