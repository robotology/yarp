/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperMultiple.h>
#include <yarp/dev/tests/IPositionControlTest.h>
#include <yarp/dev/tests/ITorqueControlTest.h>
#include <yarp/dev/tests/IAxisInfoTest.h>

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
        ITorqueControl* itrq = nullptr;
        IAxisInfo* iinfo = nullptr;

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
            yarp::dev::IMultipleWrapper* ww_nws; ddremapper.view(ww_nws);
            yarp::dev::PolyDriverList pdlist; pdlist.push(&ddmc,"fakeboard1");
            bool result_att = ww_nws->attachAll(pdlist);
            REQUIRE(result_att);
        }

        ddremapper.view(ipos);
        ddremapper.view(itrq);
        ddremapper.view(iinfo);
        yarp::dev::tests::exec_iPositionControl_test_1(ipos);
        yarp::dev::tests::exec_iTorqueControl_test_1(itrq);
        yarp::dev::tests::exec_iAxisInfo_test_1(iinfo);

        //"Close all polydrivers and check"
        {
            CHECK(ddremapper.close());
            CHECK(ddmc.close());
        }
    }

    Network::setLocalMode(false);
}
