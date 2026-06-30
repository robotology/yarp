/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
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

TEST_CASE("dev::ControlBoardRemapperTest4", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlboardremapper", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");

    Network::setLocalMode(true);

    SECTION("Testing an invalid configuration (subdevice not found)")
    {
        PolyDriver ddfakemc;
        PolyDriver ddnws;
        PolyDriver ddremapper;

        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 2);
            REQUIRE(ddfakemc.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlboardremapper");
            p_cfg.put("joints", 2);
            Value* jlist = Value::makeList("net0");
            p_cfg.put("networks", jlist);
            p_cfg.put("net0", "0 1 0 1");
            REQUIRE(ddremapper.open(p_cfg));
        }
        {
            yarp::dev::IMultipleWrapper* ww_rem=nullptr; ddremapper.view(ww_rem);
            REQUIRE(ww_rem);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddfakemc,"fakeboard1");
            bool result_att = ww_rem->attachAll(pdlist);
            REQUIRE(!result_att);
        }

        //"Close all polydrivers and check"
        {
            CHECK(ddremapper.close());
            CHECK(ddfakemc.close());
        }
    }

    SECTION("Testing an invalid configuration")
    {
        PolyDriver ddfakemc;
        PolyDriver ddnws;
        PolyDriver ddremapper;

        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            p_cfg.put("name", "fakeboard1");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 2);
            REQUIRE(ddfakemc.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlboardremapper");
            p_cfg.put("joints", 4);
            Value* jlist = Value::makeList("fakeboard1");
            p_cfg.put("networks", jlist);
            p_cfg.put("fakeboard1", "0 3 0 3");
            REQUIRE(ddremapper.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlBoard_nws_yarp");
            p_cfg.put("name", "/alljoints");
            REQUIRE(ddnws.open(p_cfg));
        }
        {
            yarp::dev::IMultipleWrapper* ww_rem=nullptr; ddremapper.view(ww_rem);
            REQUIRE(ww_rem);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddfakemc,"fakeboard1");
            bool result_att = ww_rem->attachAll(pdlist);
            REQUIRE(result_att);
        }
        {
            yarp::dev::IMultipleWrapper* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddremapper,"remapper1");
            bool result_att = ww_nws->attachAll(pdlist);
            REQUIRE(result_att);
        }

        yarp::os::Time::delay(0.5);

        IAxisInfo* iinfo = nullptr;
        IControlMode* icmd = nullptr;
        ddremapper.view(icmd);
        ReturnValue retval;

        std::vector<yarp::dev::ControlModeEnum> modes;
        modes.resize(4);
        retval = icmd->getControlModes(modes);
        CHECK(!retval); // Expecting failure because the remapper has 4 joints but the underlying device has only 2

        //"Close all polydrivers and check"
        {
            CHECK(ddnws.close());
            CHECK(ddremapper.close());
            CHECK(ddfakemc.close());
        }
    }

    SECTION("Testing an invalid configuration (chain of remappers)")
    {
        PolyDriver ddfakemc;
        PolyDriver ddnws;
        PolyDriver ddremapper1;
        PolyDriver ddremapper2;

        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            p_cfg.put("name", "fakeboard1");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 2);
            REQUIRE(ddfakemc.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlboardremapper");
            p_cfg.put("joints", 4);
            Value* jlist = Value::makeList("fakeboard1");
            p_cfg.put("networks", jlist);
            p_cfg.put("fakeboard1", "0 3 0 3");
            REQUIRE(ddremapper1.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlboardremapper");
            p_cfg.put("joints", 4);
            Value* jlist = Value::makeList("rema1");
            p_cfg.put("networks", jlist);
            p_cfg.put("rema1", "0 3 0 3");
            REQUIRE(ddremapper2.open(p_cfg));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlBoard_nws_yarp");
            p_cfg.put("name", "/alljoints");
            REQUIRE(ddnws.open(p_cfg));
        }
        {
            yarp::dev::IMultipleWrapper* ww_rem=nullptr; ddremapper1.view(ww_rem);
            REQUIRE(ww_rem);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddfakemc,"fakeboard1");
            bool result_att = ww_rem->attachAll(pdlist);
            REQUIRE(result_att);
        }
        {
            yarp::dev::IMultipleWrapper* ww_rem=nullptr; ddremapper2.view(ww_rem);
            REQUIRE(ww_rem);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddremapper1,"rema1");
            bool result_att = ww_rem->attachAll(pdlist);
            REQUIRE(result_att);
        }
        {
            yarp::dev::IMultipleWrapper* ww_nws=nullptr; ddnws.view(ww_nws);
            REQUIRE(ww_nws);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddremapper2,"rema2");
            bool result_att = ww_nws->attachAll(pdlist);
            REQUIRE(result_att);
        }

        yarp::os::Time::delay(0.5);

        IAxisInfo* iinfo = nullptr;
        IControlMode* icmd = nullptr;
        ddremapper2.view(icmd);
        ReturnValue retval;

        std::vector<yarp::dev::ControlModeEnum> modes;
        modes.resize(4);
        retval = icmd->getControlModes(modes);
        CHECK(!retval); // Expecting failure because the remapper has 4 joints but the underlying device has only 2

        //"Close all polydrivers and check"
        {
            CHECK(ddnws.close());
            CHECK(ddremapper2.close());
            CHECK(ddremapper1.close());
            CHECK(ddfakemc.close());
        }
    }

    Network::setLocalMode(false);
}
