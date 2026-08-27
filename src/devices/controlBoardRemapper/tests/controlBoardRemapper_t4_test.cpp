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
#include <yarp/dev/tests/ParametersTest.h>

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

    SECTION("Testing an invalid configuration (invalid set of joints)")
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
            yarp::os::Value* jlist = yarp::os::Value::makeList("joint0 joint1 joint2 joint3");
            p_cfg.put("axesNames", jlist);
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
            //This must fail because the remapper requires 4 joints,
            //but the fake board only has two.
            REQUIRE(!result_att);
        }
        //"Close all polydrivers and check"
         {
            CHECK(ddremapper.close());
            CHECK(ddfakemc.close());
        }
    }

    Network::setLocalMode(false);
}
