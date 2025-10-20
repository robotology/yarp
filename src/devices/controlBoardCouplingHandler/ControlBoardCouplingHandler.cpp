/*
 * SPDX-FileCopyrightText: 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardCouplingHandler.h"
#include "ControlBoardCouplingHandlerLogComponent.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <cassert>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

bool ControlBoardCouplingHandler::close()
{
    return detach();
}

bool ControlBoardCouplingHandler::open(Searchable& config)
{

    if(!parseParams(config))
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Error in parsing parameters";
        return false;
    }

    Property joint_coupling_config;
    joint_coupling_config.fromString(config.toString());
    joint_coupling_config.unput("device"); // remove the device parameter from the config otherwise we have recursion
    joint_coupling_config.put("device", m_coupling_device);
    if(!jointCouplingHandler.open(joint_coupling_config)) {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Error in opening jointCouplingHandler device";
        return false;
    }

    if(!jointCouplingHandler.view(iJntCoupling)) {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Error viewing the IJointCoupling interface";
        return false;
    }

    configureBuffers();

    return true;
}

bool ControlBoardCouplingHandler::attach(yarp::dev::PolyDriver* poly)
{
    // For both cases, now configure everything that need
    // all the attribute to be correctly configured
    bool ok {false};

    if (!poly->isValid()) {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Device " << poly << " to attach to is not valid ... cannot proceed";
        return false;
    }

    ok = poly->view(iJntEnc);
    ok = ok && poly->view(iAxInfo);

    if (!ok)
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER, "attachAll failed  some subdevice was not found or its attach failed");
        return false;
    }

    return ok;
}

void ControlBoardCouplingHandler::configureBuffers() {
    if (!iJntCoupling) {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "IJointCoupling interface not available";
        return;
    }
    size_t nrOfPhysicalJoints;
    size_t nrOfActuatedAxes;
    auto ok = iJntCoupling->getNrOfPhysicalJoints(nrOfPhysicalJoints);
    ok = ok && iJntCoupling->getNrOfActuatedAxes(nrOfActuatedAxes);
    if(!ok)
    {
        yCError(CONTROLBOARDCOUPLINGHANDLER) << "Error in getting the number of physical joints or actuated axes";
        return;
    }
    physJointsPos.resize(nrOfPhysicalJoints);
    physJointsVel.resize(nrOfPhysicalJoints);
    physJointsAcc.resize(nrOfPhysicalJoints);
    physJointsTime.resize(nrOfPhysicalJoints);
    actAxesPos.resize(nrOfActuatedAxes);
    actAxesVel.resize(nrOfActuatedAxes);
    actAxesAcc.resize(nrOfActuatedAxes);
    actAxesTime.resize(nrOfActuatedAxes);
    return;
}

bool ControlBoardCouplingHandler::detach()
{
    iJntEnc      = nullptr;
    iAxInfo      = nullptr;
    iJntCoupling = nullptr;
    jointCouplingHandler.close();
    return true;
}


//////////////////////////////////////////////////////////////////////////////
/// ControlBoard methods
//////////////////////////////////////////////////////////////////////////////


/* IEncoders */
ReturnValue ControlBoardCouplingHandler::resetEncoder(int j)
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue ControlBoardCouplingHandler::resetEncoders()
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue ControlBoardCouplingHandler::setEncoder(int j, double val)
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue ControlBoardCouplingHandler::setEncoders(const double *vals)
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue ControlBoardCouplingHandler::getEncoder(int j, double *v)
{
    ReturnValue ok = ReturnValue::return_code::return_value_error_generic;
    if (iJntEnc && iJntCoupling) {
        ok = iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos);
        if (ok) {
            *v = physJointsPos[j];
            return ok;
        }
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getEncoders(double *encs)
{
    ReturnValue ok = ReturnValue::return_code::return_value_error_generic;
    if (iJntEnc && iJntCoupling) {
        ok = iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos);
        if (ok) {
            std::copy(physJointsPos.begin(), physJointsPos.end(), encs);
            return ok;
        }
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getEncodersTimed(double *encs, double *t)
{
    ReturnValue ok = ReturnValue::return_code::return_value_error_generic;
    if (iJntEnc && iJntCoupling) {
        // TODO t has to be probably resized
        ok = iJntEnc->getEncodersTimed(actAxesPos.data(), actAxesTime.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos);
        if (ok) {
            std::copy(physJointsPos.begin(), physJointsPos.end(), encs);
            for(size_t i = 0; i < physJointsTime.size(); i++)
            {
                //TODO check if this is the correct way to take the time
                t[i] = actAxesTime[0];
            }
            return ok;
        }
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getEncoderTimed(int j, double *v, double *t)
{
    ReturnValue ok = ReturnValue::return_code::return_value_error_generic;
    if (iJntEnc && iJntCoupling) {
        ok = iJntEnc->getEncodersTimed(actAxesPos.data(), actAxesTime.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos);
        if (ok) {
            *v = physJointsPos[j];
            //TODO check if this is the correct way to take the time
            *t = actAxesTime[0];
            return ok;
        }
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getEncoderSpeed(int j, double *sp)
{
    ReturnValue ok = ReturnValue::return_code::return_value_error_generic;
    if (iJntEnc && iJntCoupling) {
        ok = iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntEnc->getEncoderSpeeds(actAxesVel.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsVel(actAxesPos, actAxesVel, physJointsVel);
        if (ok) {
            *sp = physJointsVel[j];
            return ok;
        }
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getEncoderSpeeds(double *spds)
{
    ReturnValue ok = ReturnValue::return_code::return_value_error_generic;
    if (iJntEnc && iJntCoupling) {
        ok = iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntEnc->getEncoderSpeeds(actAxesVel.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsVel(actAxesPos, actAxesVel, physJointsVel);
        if (ok) {
            std::copy(physJointsVel.begin(), physJointsVel.end(), spds);
            return ok;
        }
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getEncoderAcceleration(int j, double *acc)
{
    ReturnValue ok = ReturnValue::return_code::return_value_error_generic;
    if (iJntEnc && iJntCoupling) {
        ok = iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntEnc->getEncoderSpeeds(actAxesVel.data());
        ok = ok && iJntEnc->getEncoderAccelerations(actAxesAcc.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsAcc(actAxesPos, actAxesVel, actAxesAcc, physJointsAcc);
        if (ok) {
            *acc = physJointsAcc[j];
            return ok;
        }
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getEncoderAccelerations(double *accs)
{
    ReturnValue ok = ReturnValue::return_code::return_value_error_generic;
    if (iJntEnc && iJntCoupling) {
        ok = iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntEnc->getEncoderSpeeds(actAxesVel.data());
        ok = ok && iJntEnc->getEncoderAccelerations(actAxesAcc.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsAcc(actAxesPos, actAxesVel, actAxesAcc, physJointsAcc);
        if (ok) {
            std::copy(physJointsAcc.begin(), physJointsAcc.end(), accs);
            return ok;
        }
    }
    return ok;
}

/* IAxisInfo */

ReturnValue ControlBoardCouplingHandler::getAxes(int *ax)
{
    ReturnValue ok{ReturnValue::return_code::return_value_error_generic};
    if (iJntCoupling) {
        size_t nrOfPhysicalJoints {0};
        ok = iJntCoupling->getNrOfPhysicalJoints(nrOfPhysicalJoints);
        if (ok) {
            *ax = nrOfPhysicalJoints;
            return ok;
        }
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getAxisName(int j, std::string& name)
{
    ReturnValue ok{ReturnValue::return_code::return_value_error_generic};
    if (iJntCoupling) {
        ok = iJntCoupling->getPhysicalJointName(j, name);
        return ok;
    }
    return ok;
}

ReturnValue ControlBoardCouplingHandler::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    ReturnValue ok{ReturnValue::return_code::return_value_error_generic};
    // TODO I am not sure how to handle this function
    type = VOCAB_JOINTTYPE_REVOLUTE;
    return ReturnValue_ok;
}
