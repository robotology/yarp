/*
 * SPDX-FileCopyrightText: 2006-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/WrapperSingle.h>

#include <vector>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;


TEST_CASE("dev::ControlBoardCouplingHandlerTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeJointCoupling", "device");
    YARP_REQUIRE_PLUGIN("controlBoardCouplingHandler", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");

    Network::setLocalMode(true);

    SECTION("Test the controlboard coupling handler")
    {
        PolyDriver ddjc;
        IEncodersTimed* iet=nullptr;
        IAxisInfo* iai=nullptr, *iaifmc=nullptr;
        IPositionControl* ipc=nullptr;
        PolyDriver ddcch, ddfmc;

        ////////"Checking opening polydrivers"
        {
            Property p_cfg;
            p_cfg.put("device", "fakeMotionControl");
            Property& grp = p_cfg.addGroup("GENERAL");
            grp.put("Joints", 3);
            REQUIRE(ddfmc.open(p_cfg));
            REQUIRE(ddfmc.view(ipc));
            REQUIRE(ipc!=nullptr);
            REQUIRE(ddfmc.view(iaifmc));
        }
        {
            Property p_cfg;
            p_cfg.put("device", "controlBoardCouplingHandler");
            p_cfg.put("coupling_device", "fakeJointCoupling"); // 3 act axes, 4 phys joints
            REQUIRE(ddcch.open(p_cfg));
        }
        //attach
        {
            WrapperSingle* ww_nws; ddcch.view(ww_nws);
            REQUIRE(ww_nws);
            bool result_att = ww_nws->attach(&ddfmc);
            REQUIRE(result_att);
        }
        {
            REQUIRE(ddcch.view(iet));
            REQUIRE(iet!=nullptr);
            REQUIRE(ddcch.view(iai));
            REQUIRE(iai!=nullptr);
        }
        {
            int axes = 0;
            CHECK(iaifmc->getAxes(&axes));
            CHECK(axes == 3);
        }
        {
            int axes = 0;
            std::string physical_joint_name;
            CHECK(iai->getAxes(&axes));
            CHECK(axes == 4);
            CHECK(iai->getAxisName(0, physical_joint_name));
            CHECK(physical_joint_name == "phys_joint_0");
            CHECK(iai->getAxisName(1, physical_joint_name));
            CHECK(physical_joint_name == "phys_joint_1");
            CHECK(iai->getAxisName(2, physical_joint_name));
            CHECK(physical_joint_name == "phys_joint_2");
            CHECK(iai->getAxisName(3, physical_joint_name));
            CHECK(physical_joint_name == "phys_joint_3");
        }
        {
            int axes = 0;
            CHECK(iaifmc->getAxes(&axes));
            CHECK(axes == 3);
        }

        {
            // Let's move the underlyng motor control and check the coupling
            CHECK(ipc->setRefSpeed(0, 1000));
            CHECK(ipc->setRefSpeed(1, 1000));
            CHECK(ipc->setRefSpeed(2, 1000));
            CHECK(ipc->positionMove(0, 10));
            CHECK(ipc->positionMove(1, 20));
            CHECK(ipc->positionMove(2, 30));
            yarp::os::Time::delay(0.1);
            std::vector<double> physJointsPos;
            physJointsPos.resize(4);
            CHECK(iet->getEncoders(physJointsPos.data()));
            CHECK(physJointsPos[0] == 10);
            CHECK(physJointsPos[1] == 20);
            CHECK(physJointsPos[2] == 15);
            CHECK(physJointsPos[3] == 15);
            double singleEncVal {0};
            CHECK(iet->getEncoder(0, &singleEncVal));
            CHECK(singleEncVal == 10);
            CHECK(iet->getEncoder(1, &singleEncVal));
            CHECK(singleEncVal == 20);
            CHECK(iet->getEncoder(2, &singleEncVal));
            CHECK(singleEncVal == 15);
            CHECK(iet->getEncoder(3, &singleEncVal));
            CHECK(singleEncVal == 15);
        }
        {
            std::vector<double> physJointsVel,physJointsAcc;
            physJointsVel.resize(4);
            physJointsAcc.resize(4);
            CHECK(iet->getEncoderSpeeds(physJointsVel.data()));
            CHECK(iet->getEncoderAccelerations(physJointsAcc.data()));
            CHECK(physJointsVel[0] == 0);
            CHECK(physJointsVel[1] == 0);
            CHECK(physJointsVel[2] == 15);
            CHECK(physJointsVel[3] == 15);
            CHECK(physJointsAcc[0] == 0);
            CHECK(physJointsAcc[1] == 0);
            CHECK(physJointsAcc[2] == 15);
            CHECK(physJointsAcc[3] == 15);
        }

    }

    Network::setLocalMode(false);
}
