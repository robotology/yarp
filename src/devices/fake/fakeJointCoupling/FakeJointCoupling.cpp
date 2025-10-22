/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeJointCoupling.h"

#include <yarp/conf/environment.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/NetType.h>
#include <yarp/dev/Drivers.h>

#include <sstream>
#include <cstring>
#include <algorithm>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::os::impl;


namespace {
YARP_LOG_COMPONENT(FAKEJOINTCOUPLING, "yarp.device.fakeJointCoupling")
}

bool FakeJointCoupling::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) {return false;}

    yarp::sig::VectorOf<size_t> coupled_physical_joints {2,3};
    yarp::sig::VectorOf<size_t> coupled_actuated_axes {2};
    std::vector<std::string> physical_joint_names{"phys_joint_0", "phys_joint_1", "phys_joint_2", "phys_joint_3"};
    std::vector<std::string> actuated_axes_names{"act_axes_0", "act_axes_1", "act_axes_2"};
    std::vector<std::pair<double, double>> physical_joint_limits{{-30.0, 30.0}, {-10.0, 10.0}, {-32.0, 33.0}, {0.0, 120.0}};
    initialise(coupled_physical_joints, coupled_actuated_axes, physical_joint_names, actuated_axes_names, physical_joint_limits);
    return true;
}
bool FakeJointCoupling::close() {
    return true;
}

ReturnValue FakeJointCoupling::convertFromPhysicalJointsToActuatedAxesPos(const yarp::sig::Vector& physJointsPos, yarp::sig::Vector& actAxesPos) {
    size_t nrOfPhysicalJoints;
    size_t nrOfActuatedAxes;
    auto ok = getNrOfPhysicalJoints(nrOfPhysicalJoints);
    ok = ok && getNrOfActuatedAxes(nrOfActuatedAxes);
    if (!ok || physJointsPos.size() != nrOfPhysicalJoints || actAxesPos.size() != nrOfActuatedAxes) {
        // yCDebug(FAKEJOINTCOUPLING) << ok <<physJointsPos.size()<<nrOfPhysicalJoints<<actAxesPos.size()<<nrOfActuatedAxes;
        yCError(FAKEJOINTCOUPLING) << "convertFromPhysicalJointsToActuatedAxesPos: input or output vectors have wrong size";
        return ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }
    actAxesPos[0] = physJointsPos[0];
    actAxesPos[1] = physJointsPos[1];
    actAxesPos[2] = physJointsPos[2] + physJointsPos[3];
    return ReturnValue_ok;
}
ReturnValue FakeJointCoupling::convertFromPhysicalJointsToActuatedAxesVel(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, yarp::sig::Vector& actAxesVel) {
    size_t nrOfPhysicalJoints;
    size_t nrOfActuatedAxes;
    auto ok = getNrOfPhysicalJoints(nrOfPhysicalJoints);
    ok = ok && getNrOfActuatedAxes(nrOfActuatedAxes);
    if (!ok || physJointsPos.size() != nrOfPhysicalJoints || physJointsVel.size() != nrOfPhysicalJoints || actAxesVel.size() != nrOfActuatedAxes) {
        yCError(FAKEJOINTCOUPLING) << "convertFromPhysicalJointsToActuatedAxesPos: input or output vectors have wrong size";
        return ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }
    actAxesVel[0] = physJointsVel[0];
    actAxesVel[1] = physJointsVel[1];
    actAxesVel[2] = physJointsPos[2] + physJointsPos[3] + physJointsVel[2] + physJointsVel[3];
    return ReturnValue_ok;
}
ReturnValue FakeJointCoupling::convertFromPhysicalJointsToActuatedAxesAcc(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel,
                                                                   const yarp::sig::Vector& physJointsAcc, yarp::sig::Vector& actAxesAcc) {
    size_t nrOfPhysicalJoints;
    size_t nrOfActuatedAxes;
    auto ok = getNrOfPhysicalJoints(nrOfPhysicalJoints);
    ok = ok && getNrOfActuatedAxes(nrOfActuatedAxes);
    if(!ok || physJointsPos.size() != nrOfPhysicalJoints || physJointsVel.size() != nrOfPhysicalJoints || physJointsAcc.size() != nrOfPhysicalJoints || actAxesAcc.size() != nrOfActuatedAxes) {
        yCError(FAKEJOINTCOUPLING) << "convertFromPhysicalJointsToActuatedAxesPos: input or output vectors have wrong size";
        return ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }
    actAxesAcc[0] = physJointsAcc[0];
    actAxesAcc[1] = physJointsAcc[1];
    actAxesAcc[2] = physJointsPos[2] + physJointsPos[3] + physJointsVel[2] + physJointsVel[3] + physJointsAcc[2] + physJointsAcc[3];
    return ReturnValue_ok;
}
ReturnValue FakeJointCoupling::convertFromPhysicalJointsToActuatedAxesTrq(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsTrq, yarp::sig::Vector& actAxesTrq) {
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}
ReturnValue FakeJointCoupling::convertFromActuatedAxesToPhysicalJointsPos(const yarp::sig::Vector& actAxesPos, yarp::sig::Vector& physJointsPos) {
    size_t nrOfPhysicalJoints;
    size_t nrOfActuatedAxes;
    auto ok = getNrOfPhysicalJoints(nrOfPhysicalJoints);
    ok = ok && getNrOfActuatedAxes(nrOfActuatedAxes);
    if(!ok || actAxesPos.size() != nrOfActuatedAxes || physJointsPos.size() != nrOfPhysicalJoints) {
        yCError(FAKEJOINTCOUPLING) << "convertFromActuatedAxesToPhysicalJointsPos: input or output vectors have wrong size";
        return ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }
    physJointsPos[0] = actAxesPos[0];
    physJointsPos[1] = actAxesPos[1];
    physJointsPos[2] = actAxesPos[2] / 2.0;
    physJointsPos[3] = actAxesPos[2] / 2.0;
    return ReturnValue_ok;
}
ReturnValue FakeJointCoupling::convertFromActuatedAxesToPhysicalJointsVel(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, yarp::sig::Vector& physJointsVel) {
    size_t nrOfPhysicalJoints;
    size_t nrOfActuatedAxes;
    auto ok = getNrOfPhysicalJoints(nrOfPhysicalJoints);
    ok = ok && getNrOfActuatedAxes(nrOfActuatedAxes);
    if(!ok || actAxesPos.size() != nrOfActuatedAxes || actAxesVel.size() != nrOfActuatedAxes || physJointsVel.size() != nrOfPhysicalJoints) {
        yCError(FAKEJOINTCOUPLING) << "convertFromActuatedAxesToPhysicalJointsVel: input or output vectors have wrong size";
        return ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }
    physJointsVel[0] = actAxesVel[0];
    physJointsVel[1] = actAxesVel[1];
    physJointsVel[2] = actAxesPos[2] / 2.0 - actAxesVel[2] / 2.0;
    physJointsVel[3] = actAxesPos[2] / 2.0 + actAxesVel[2] / 2.0;
    return ReturnValue_ok;

}
ReturnValue FakeJointCoupling::convertFromActuatedAxesToPhysicalJointsAcc(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, const yarp::sig::Vector& actAxesAcc, yarp::sig::Vector& physJointsAcc) {
    size_t nrOfPhysicalJoints;
    size_t nrOfActuatedAxes;
    auto ok = getNrOfPhysicalJoints(nrOfPhysicalJoints);
    ok = ok && getNrOfActuatedAxes(nrOfActuatedAxes);
    if(!ok || actAxesPos.size() != nrOfActuatedAxes || actAxesVel.size() != nrOfActuatedAxes || actAxesAcc.size() != nrOfActuatedAxes || physJointsAcc.size() != nrOfPhysicalJoints) {
        yCError(FAKEJOINTCOUPLING) << "convertFromActuatedAxesToPhysicalJointsAcc: input or output vectors have wrong size";
        return ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }
    physJointsAcc[0] = actAxesAcc[0];
    physJointsAcc[1] = actAxesAcc[1];
    physJointsAcc[2] = actAxesPos[2] / 2.0 - actAxesVel[2] / 2.0 - actAxesAcc[2] / 2.0;
    physJointsAcc[3] = actAxesPos[2] / 2.0 + actAxesVel[2] / 2.0 + actAxesAcc[2] / 2.0;
    return ReturnValue_ok;
}
ReturnValue FakeJointCoupling::convertFromActuatedAxesToPhysicalJointsTrq(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesTrq, yarp::sig::Vector& physJointsTrq) {
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}
