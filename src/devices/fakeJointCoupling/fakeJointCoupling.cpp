/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeJointCoupling.h"

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

bool FakeJointCoupling::open(yarp::os::Searchable &par) {
    return true;
}
bool FakeJointCoupling::close() {
    return true;
}

bool FakeJointCoupling::convertFromPhysicalJointsToActuatedAxesPos(const yarp::sig::Vector& physJointsPos, yarp::sig::Vector& actAxesPos) {
    if (physJointsPos.size() != actAxesPos.size()) {
        yCError(FAKEJOINTCOUPLING) << "convertFromPhysicalJointsToActuatedAxesPos: input and output vectors have different size";
        return false;
    }
    std::transform(physJointsPos.begin(), physJointsPos.end(), actAxesPos.begin(), [](double pos) { return pos * 2; });
    return true;
}
bool FakeJointCoupling::convertFromPhysicalJointsToActuatedAxesVel(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel, yarp::sig::Vector& actAxesVel) {
    if(physJointsPos.size() != physJointsVel.size() || physJointsPos.size() != actAxesVel.size()) {
        yCError(FAKEJOINTCOUPLING) << "convertFromPhysicalJointsToActuatedAxesVel: input and output vectors have different size";
        return false;
    }
    for(size_t i = 0; i < physJointsPos.size(); i++) {
        actAxesVel[i] = 2 * physJointsPos[i] * + 2 * physJointsVel[i];
    }
    return true;
}
bool FakeJointCoupling::convertFromPhysicalJointsToActuatedAxesAcc(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsVel,
                                                                   const yarp::sig::Vector& physJointsAcc, yarp::sig::Vector& actAxesAcc) {
    if(physJointsPos.size() != physJointsVel.size() || physJointsPos.size() != physJointsAcc.size() || physJointsPos.size() != actAxesAcc.size()) {
        yCError(FAKEJOINTCOUPLING) << "convertFromPhysicalJointsToActuatedAxesAcc: input and output vectors have different size";
        return false;
    }
    for(size_t i = 0; i < physJointsPos.size(); i++) {
        actAxesAcc[i] = 2 * physJointsPos[i] + 2 * physJointsVel[i] + 2* physJointsAcc[i];
    }
    return true;
}
bool FakeJointCoupling::convertFromPhysicalJointsToActuatedAxesTrq(const yarp::sig::Vector& physJointsPos, const yarp::sig::Vector& physJointsTrq, yarp::sig::Vector& actAxesTrq) {
    yCError(FAKEJOINTCOUPLING) << "convertFromPhysicalJointsToActuatedAxesTrq: not implemented yet";
    return false;
}
bool FakeJointCoupling::convertFromActuatedAxesToPhysicalJointsPos(const yarp::sig::Vector& actAxesPos, yarp::sig::Vector& physJointsPos) {
    if(actAxesPos.size() != physJointsPos.size()) {
        yCError(FAKEJOINTCOUPLING) << "convertFromActuatedAxesToPhysicalJointsPos: input and output vectors have different size";
        return false;
    }
    std::transform(actAxesPos.begin(), actAxesPos.end(), physJointsPos.begin(), [](double pos) { return pos / 2.0; });
    return true;
}
bool FakeJointCoupling::convertFromActuatedAxesToPhysicalJointsVel(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, yarp::sig::Vector& physJointsVel) {
    if(actAxesPos.size() != actAxesVel.size() || actAxesPos.size() != physJointsVel.size()) {
        yCError(FAKEJOINTCOUPLING) << "convertFromActuatedAxesToPhysicalJointsVel: input and output vectors have different size";
        return false;
    }
    for(size_t i = 0; i < actAxesPos.size(); i++) {
        physJointsVel[i] = actAxesPos[i] / 2.0 + actAxesVel[i] / 2.0;
    }
    return true;

}
bool FakeJointCoupling::convertFromActuatedAxesToPhysicalJointsAcc(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesVel, const yarp::sig::Vector& actAxesAcc, yarp::sig::Vector& physJointsAcc) {
    if(actAxesPos.size() != actAxesVel.size() || actAxesPos.size() != actAxesAcc.size() || actAxesPos.size() != physJointsAcc.size()) {
        yCError(FAKEJOINTCOUPLING) << "convertFromActuatedAxesToPhysicalJointsAcc: input and output vectors have different size";
        return false;
    }
    for(size_t i = 0; i < actAxesPos.size(); i++) {
        physJointsAcc[i] = actAxesPos[i] / 2.0 + actAxesVel[i] / 2.0 + actAxesAcc[i] / 2.0;
    }
    return true;
}
bool FakeJointCoupling::convertFromActuatedAxesToPhysicalJointsTrq(const yarp::sig::Vector& actAxesPos, const yarp::sig::Vector& actAxesTrq, yarp::sig::Vector& physJointsTrq) {
    yCWarning(FAKEJOINTCOUPLING) << "convertFromActuatedAxesToPhysicalJointsTrq: not implemented yet";
    return false;
}
