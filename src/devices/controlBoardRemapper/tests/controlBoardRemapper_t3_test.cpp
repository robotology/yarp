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
#include <yarp/dev/IControlLimits.h>
#include <yarp/dev/IImpedanceControl.h>
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
#include <yarp/dev/tests/IJointFaultTest.h>
#include <yarp/dev/tests/IControlLimitsTest.h>
#include <yarp/dev/tests/IImpedanceControlTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::ControlBoardRemapperTest3", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlboardremapper", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");
    //YARP_REQUIRE_PLUGIN("controlBoard_nwc_yarp", "device");
    YARP_REQUIRE_PLUGIN("remote_controlboard", "device");

    Network::setLocalMode(true);

    //remapping from one single hardware device
    SECTION("Checking controlboardremapper device attached to a single controlBoard_nwc_yarp device")
    {
        PolyDriver ddfakemc;
        PolyDriver ddnws;
        PolyDriver ddnwc;
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
        IJointFault* ifault = nullptr;
        IControlLimits* ilims = nullptr;
        IImpedanceControl* iimp = nullptr;
        //IRemoteCalibrator* iremotecalib = nullptr;

        ////////"Test a controlboardremapper attached to a controlBoard_nwc_yarp"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 4);
            REQUIRE(ddfakemc.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlBoard_nws_yarp");
            p_cfg.put("name", "/alljoints");
            REQUIRE(ddnws.open(p_cfg));
        }
        {
            yarp::dev::IMultipleWrapper* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddfakemc,"fakeboard1");
            bool result_att = ww_nws->attachAll(pdlist);
            REQUIRE(result_att);
        }

        yarp::os::Time::delay(0.1);

        //client side
        {
            Property p_cfg;
            //p_cfg.put("device", "controlBoard_nwc_yarp");
            p_cfg.put("device", "remote_controlboard");
            p_cfg.put("local", "/localalljoints");
            p_cfg.put("remote", "/alljoints");
            REQUIRE(ddnwc.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlboardremapper");
            yarp::os::Value* jlist = yarp::os::Value::makeList("joint3 joint2");
            p_cfg.put("axesNames", jlist);
            REQUIRE(ddremapper.open(p_cfg));
        }
        {
            yarp::dev::IMultipleWrapper* ww_rem=nullptr; ddremapper.view(ww_rem);
            REQUIRE(ww_rem);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddnwc,"nwcboard1");
            bool result_att = ww_rem->attachAll(pdlist);
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
        ddremapper.view(ifault);  REQUIRE(ifault);
        ddremapper.view(ilims);   REQUIRE(ilims);
        ddremapper.view(iimp);   REQUIRE(iimp);
        //ddremapper.view(iremotecalib);  REQUIRE(iremotecalib);

        yarp::dev::tests::exec_iPositionControl_test_1(ipos, icmd);
        yarp::dev::tests::exec_iVelocityControl_test_1(ivel, icmd);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq, icmd);
        yarp::dev::tests::exec_iAxisInfo_test_1(iinfo, "joint3");
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);
        yarp::dev::tests::exec_iControlMode_test_1(icmd, iinfo);
        yarp::dev::tests::exec_iInteractionMode_test_1(iint,iinfo);
        yarp::dev::tests::exec_iMotor_test_1(imot);
        yarp::dev::tests::exec_iMotorEncoders_test_1(imotenc);
        yarp::dev::tests::exec_iPidControl_test_1(ipid, iinfo);
        yarp::dev::tests::exec_iPidControl_test_2(ipid, iinfo);
        yarp::dev::tests::exec_iPwmControl_test_1(ipwm, icmd);
        yarp::dev::tests::exec_iCurrentControl_test_1(icurr, icmd);
        //yarp::dev::tests::exec_iRemoteCalibrator_test_1(iremotecalib);
        yarp::dev::tests::exec_iJointFault_test_1(ifault);
        yarp::dev::tests::exec_iControlLimits_test1(ilims, iinfo);
        yarp::dev::tests::exec_iImpedanceControl_test_1(iimp);

        //"Close all polydrivers and check"
        {
            CHECK(ddremapper.close());
            CHECK(ddnwc.close());
            CHECK(ddnws.close());
            CHECK(ddfakemc.close());
        }
    }

    // remapping from two hardware devices
    SECTION("Checking controlboardremapper device attached to two controlBoard_nwc_yarp device")
    {
        PolyDriver ddfakemc1;
        PolyDriver ddnws1;
        PolyDriver ddnwc1;
        PolyDriver ddfakemc2;
        PolyDriver ddnws2;
        PolyDriver ddnwc2;
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
        IJointFault* ifault = nullptr;
        IControlLimits* ilims = nullptr;
        IImpedanceControl* iimp = nullptr;
        //IRemoteCalibrator* iremotecalib = nullptr;

        ////////"Test a controlboardremapper attached to a controlBoard_nwc_yarp"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 4);
            yarp::os::Value* jlist = yarp::os::Value::makeList("a1 a2 a3 a4");
            grp.put("AxisName", jlist);
            REQUIRE(ddfakemc1.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 3);
            yarp::os::Value* jlist = yarp::os::Value::makeList("b11 b12 b13");
            grp.put("AxisName", jlist);
            REQUIRE(ddfakemc2.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlBoard_nws_yarp");
            p_cfg.put("name", "/alljoints1");
            REQUIRE(ddnws1.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlBoard_nws_yarp");
            p_cfg.put("name", "/alljoints2");
            REQUIRE(ddnws2.open(p_cfg));
        }
        {
            yarp::dev::IMultipleWrapper* ww_nws=nullptr; ddnws1.view(ww_nws);
            REQUIRE(ww_nws);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddfakemc1,"fakeboard1");
            bool result_att = ww_nws->attachAll(pdlist);
            REQUIRE(result_att);
        }
        {
            yarp::dev::IMultipleWrapper* ww_nws=nullptr; ddnws2.view(ww_nws);
            REQUIRE(ww_nws);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddfakemc2,"fakeboard2");
            bool result_att = ww_nws->attachAll(pdlist);
            REQUIRE(result_att);
        }

        yarp::os::Time::delay(0.1);

        //client side
        {
            Property p_cfg;
            //p_cfg.put("device", "controlBoard_nwc_yarp");
            p_cfg.put("device", "remote_controlboard");
            p_cfg.put("local", "/localalljoints1");
            p_cfg.put("remote", "/alljoints1");
            REQUIRE(ddnwc1.open(p_cfg));
        }
        {
            Property p_cfg;
            //p_cfg.put("device", "controlBoard_nwc_yarp");
            p_cfg.put("device", "remote_controlboard");
            p_cfg.put("local", "/localalljoints2");
            p_cfg.put("remote", "/alljoints2");
            REQUIRE(ddnwc2.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlboardremapper");
            yarp::os::Value* jlist = yarp::os::Value::makeList("a3 b12");
            p_cfg.put("axesNames", jlist);
            REQUIRE(ddremapper.open(p_cfg));
        }
        {
            yarp::dev::IMultipleWrapper* ww_rem=nullptr; ddremapper.view(ww_rem);
            REQUIRE(ww_rem);
            yarp::dev::PolyDriverList pdlist;
            pdlist.push(&ddnwc1, "nwcboard1");
            pdlist.push(&ddnwc2, "nwcboard2");
            bool result_att = ww_rem->attachAll(pdlist);
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
        ddremapper.view(ifault);  REQUIRE(ifault);
        ddremapper.view(ilims);   REQUIRE(ilims);
        ddremapper.view(iimp);   REQUIRE(iimp);
        //ddremapper.view(iremotecalib);  REQUIRE(iremotecalib);

        yarp::dev::tests::exec_iPositionControl_test_1(ipos, icmd);
        yarp::dev::tests::exec_iVelocityControl_test_1(ivel, icmd);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq, icmd);
        yarp::dev::tests::exec_iAxisInfo_test_1(iinfo, "a3");
        yarp::dev::tests::exec_iEncodersTimed_test_1(ienc);
        yarp::dev::tests::exec_iControlMode_test_1(icmd, iinfo);
        yarp::dev::tests::exec_iInteractionMode_test_1(iint,iinfo);
        yarp::dev::tests::exec_iMotor_test_1(imot);
        yarp::dev::tests::exec_iMotorEncoders_test_1(imotenc);
        yarp::dev::tests::exec_iPidControl_test_1(ipid, iinfo);
        yarp::dev::tests::exec_iPidControl_test_2(ipid, iinfo);
        yarp::dev::tests::exec_iPwmControl_test_1(ipwm, icmd);
        yarp::dev::tests::exec_iCurrentControl_test_1(icurr, icmd);
        //yarp::dev::tests::exec_iRemoteCalibrator_test_1(iremotecalib);
        yarp::dev::tests::exec_iJointFault_test_1(ifault);
        yarp::dev::tests::exec_iControlLimits_test1(ilims, iinfo);
        yarp::dev::tests::exec_iImpedanceControl_test_1(iimp);

        //"Close all polydrivers and check"
        {
            CHECK(ddremapper.close());
            CHECK(ddnwc1.close());
            CHECK(ddnwc2.close());
            CHECK(ddnws1.close());
            CHECK(ddnws2.close());
            CHECK(ddfakemc1.close());
            CHECK(ddfakemc2.close());
        }
    }

    Network::setLocalMode(false);
}
