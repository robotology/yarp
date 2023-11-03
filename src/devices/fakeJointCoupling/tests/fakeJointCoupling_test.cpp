/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IJointCoupling.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeJointCoupling", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeJointCoupling", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeJointCoupling device")
    {
        PolyDriver ddjc;
        IJointCoupling* ijc=nullptr;
        {
            Property p_cfg;
            p_cfg.put("device", "fakeJointCoupling");
            REQUIRE(ddjc.open(p_cfg));
        }

        REQUIRE(ddjc.view(ijc));
        REQUIRE(ijc!=nullptr);
        yarp::sig::VectorOf<size_t> coupled_physical_joints;
        coupled_physical_joints.clear();
        CHECK(ijc->getCoupledPhysicalJoints(coupled_physical_joints));
        CHECK(coupled_physical_joints.size() == 2);
        CHECK(coupled_physical_joints[0] == 2);
        CHECK(coupled_physical_joints[1] == 3);

        yarp::sig::VectorOf<size_t> coupled_actuated_axes;
        coupled_actuated_axes.clear();
        CHECK(ijc->getCoupledActuatedAxes(coupled_actuated_axes));
        CHECK(coupled_actuated_axes.size() == 1);
        CHECK(coupled_actuated_axes[0] == 2);

        size_t nr_of_physical_joints{0};
        CHECK(ijc->getNrOfPhysicalJoints(nr_of_physical_joints));
        CHECK(nr_of_physical_joints == 4);

        size_t nr_of_actuated_axes{0};
        CHECK(ijc->getNrOfActuatedAxes(nr_of_actuated_axes));
        CHECK(nr_of_actuated_axes == 3);

        std::string physical_joint_name;
        CHECK(ijc->getPhysicalJointName(0, physical_joint_name));
        CHECK(physical_joint_name == "phys_joint_0");
        CHECK(ijc->getPhysicalJointName(1, physical_joint_name));
        CHECK(physical_joint_name == "phys_joint_1");
        CHECK(ijc->getPhysicalJointName(2, physical_joint_name));
        CHECK(physical_joint_name == "phys_joint_2");
        CHECK(ijc->getPhysicalJointName(3, physical_joint_name));
        CHECK(physical_joint_name == "phys_joint_3");
        CHECK_FALSE(ijc->getPhysicalJointName(4, physical_joint_name));

        std::string actuated_axis_name;
        CHECK(ijc->getActuatedAxisName(0, actuated_axis_name));
        CHECK(actuated_axis_name == "act_axes_0");
        CHECK(ijc->getActuatedAxisName(1, actuated_axis_name));
        CHECK(actuated_axis_name == "act_axes_1");
        CHECK(ijc->getActuatedAxisName(2, actuated_axis_name));
        CHECK(actuated_axis_name == "act_axes_2");
        CHECK_FALSE(ijc->getActuatedAxisName(3, actuated_axis_name));

        yarp::sig::Vector physJointsPos{0.0, 1.0, 2.0, 3.0};
        yarp::sig::Vector actAxesPos{0.0, 0.0, 0.0};
        yarp::sig::Vector physJointsVel{0.0, 1.0, 2.0, 3.0};
        yarp::sig::Vector actAxesVel{0.0, 0.0, 0.0};
        yarp::sig::Vector physJointsAcc{0.0, 1.0, 2.0, 3.0};
        yarp::sig::Vector actAxesAcc{0.0, 0.0, 0.0};;

        CHECK(ijc->convertFromPhysicalJointsToActuatedAxesPos(physJointsPos, actAxesPos));
        CHECK(actAxesPos[0] == 0.0);
        CHECK(actAxesPos[1] == 1.0);
        CHECK(actAxesPos[2] == 5.0);

        CHECK(ijc->convertFromPhysicalJointsToActuatedAxesVel(physJointsPos, physJointsVel, actAxesVel));
        CHECK(actAxesVel[0] == 0.0);
        CHECK(actAxesVel[1] == 1.0);
        CHECK(actAxesVel[2] == 10.0);

        CHECK(ijc->convertFromPhysicalJointsToActuatedAxesAcc(physJointsPos, physJointsVel, physJointsAcc, actAxesAcc));
        CHECK(actAxesAcc[0] == 0.0);
        CHECK(actAxesAcc[1] == 1.0);
        CHECK(actAxesAcc[2] == 15.0);

        CHECK_FALSE(ijc->convertFromPhysicalJointsToActuatedAxesTrq(physJointsPos, physJointsAcc, actAxesAcc));

        CHECK(ijc->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos));
        CHECK(physJointsPos[0] == 0.0);
        CHECK(physJointsPos[1] == 1.0);
        CHECK(physJointsPos[2] == 2.5);
        CHECK(physJointsPos[3] == 2.5);

        CHECK(ijc->convertFromActuatedAxesToPhysicalJointsVel(actAxesPos, actAxesVel, physJointsVel));
        CHECK(physJointsVel[0] == 0.0);
        CHECK(physJointsVel[1] == 1.0);
        CHECK(physJointsVel[2] == -2.5);
        CHECK(physJointsVel[3] == 7.5);

        CHECK(ijc->convertFromActuatedAxesToPhysicalJointsAcc(actAxesPos, actAxesVel, actAxesAcc, physJointsAcc));
        CHECK(physJointsAcc[0] == 0.0);
        CHECK(physJointsAcc[1] == 1.0);
        CHECK(physJointsAcc[2] == -10.0);
        CHECK(physJointsAcc[3] == 15.0);

        CHECK_FALSE(ijc->convertFromActuatedAxesToPhysicalJointsTrq(actAxesPos, actAxesAcc, physJointsAcc));

        double min{0.0}, max{0.0};
        CHECK(ijc->getPhysicalJointLimits(0, min, max));
        CHECK(min == -30.0);
        CHECK(max == 30.0);
        CHECK(ijc->getPhysicalJointLimits(1, min, max));
        CHECK(min == -10.0);
        CHECK(max == 10.0);
        CHECK(ijc->getPhysicalJointLimits(2, min, max));
        CHECK(min == -32.0);
        CHECK(max == 33.0);
        CHECK(ijc->getPhysicalJointLimits(3, min, max));
        CHECK(min == 0.0);
        CHECK(max == 120.0);




        //"Close all polydrivers and check"
        {
            CHECK(ddjc.close());
        }
    }

    Network::setLocalMode(false);
}
