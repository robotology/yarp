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



        yarp::sig::VectorOf<size_t> coupled_physical_joints;
        coupled_physical_joints.clear();
        CHECK(ijc->getCoupledPhysicalJoints(coupled_physical_joints));
        CHECK(coupled_physical_joints.size() == 2);
        CHECK(coupled_physical_joints[0] == 3);
        CHECK(coupled_physical_joints[1] == 4);

        yarp::sig::VectorOf<size_t> coupled_actuated_axes;
        coupled_actuated_axes.clear();
        CHECK(ijc->getCoupledActuatedAxes(coupled_actuated_axes));
        CHECK(coupled_actuated_axes.size() == 1);
        CHECK(coupled_actuated_axes[0] == 2);

        size_t nr_of_physical_joints{0};
        CHECK(ijc->getNrOfPhysicalJoints(&nr_of_physical_joints));
        CHECK(nr_of_physical_joints == 5);

        size_t nr_of_actuated_axes{0};
        CHECK(ijc->getNrOfActuatedAxes(&nr_of_actuated_axes));
        CHECK(nr_of_actuated_axes == 3);

        std::string physical_joint_name;
        CHECK(ijc->getPhysicalJointName(0, physical_joint_name));
        CHECK(physical_joint_name == "phys_joint_0");
        CHECK(ijc->getPhysicalJointName(1, physical_joint_name));
        CHECK(physical_joint_name == "phys_joint_1");
        CHECK(ijc->getPhysicalJointName(2, physical_joint_name));
        CHECK(physical_joint_name == "phys_joint_2");

        std::string actuated_axis_name;
        CHECK(ijc->getActuatedAxisName(0, actuated_axis_name));
        CHECK(actuated_axis_name == "act_axes_0");
        CHECK(ijc->getActuatedAxisName(1, actuated_axis_name));
        CHECK(actuated_axis_name == "act_axes_1");

        //"Close all polydrivers and check"
        {
            CHECK(ddjc.close());
        }




    // virtual bool convertFromPhysicalJointsToActuatedAxesPos(const yarp::sig::Vector& physJointsPos, yarp::sig::Vector& actAxesPos) = 0;
    // virtual bool convertFromPhysicalJointsToActuatedAxesVel(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, yarp::sig::Vector& actAxesVel) = 0;
    // virtual bool convertFromPhysicalJointsToActuatedAxesAcc(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, const yarp::sig::Vector& physJointsAcc, yarp::sig::Vector& actAxesAcc) = 0;
    // virtual bool convertFromPhysicalJointsToActuatedAxesTrq(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsTrq, yarp::sig::Vector& actAxesTrq) = 0;
    // virtual bool convertFromActuatedAxesToPhysicalJointsPos(const yarp::sig::Vector& actAxesPos, yarp::sig::Vector& physJointsPos) = 0;
    // virtual bool convertFromActuatedAxesToPhysicalJointsVel(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, yarp::sig::Vector& physJointsVel) = 0;
    // virtual bool convertFromActuatedAxesToPhysicalJointsAcc(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, const yarp::sig::Vector& actAxesAcc, yarp::sig::Vector& physJointsAcc) = 0;
    // virtual bool convertFromActuatedAxesToPhysicalJointsTrq(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesTrq, yarp::sig::Vector& physJointsTrq) = 0;
    // virtual bool getCoupledActuatedAxes(yarp::sig::VectorOf<size_t>& coupActAxesIndexes)=0;
    // virtual bool getPhysicalJointLimits(size_t physicalJointIndex, double& min, double& max)=0;
    }

    Network::setLocalMode(false);
}
