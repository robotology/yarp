/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
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
    yCDebug(CONTROLBOARDCOUPLINGHANDLER) << "Opening jointCouplingHandler device with config: " << joint_coupling_config.toString();
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
    actAxesPos.resize(nrOfActuatedAxes);
    actAxesVel.resize(nrOfActuatedAxes);
    actAxesAcc.resize(nrOfActuatedAxes);
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
bool ControlBoardCouplingHandler::resetEncoder(int j)
{
    return false;
}

bool ControlBoardCouplingHandler::resetEncoders()
{
    return false;
}

bool ControlBoardCouplingHandler::setEncoder(int j, double val)
{
    return false;
}

bool ControlBoardCouplingHandler::setEncoders(const double *vals)
{
    return false;
}

bool ControlBoardCouplingHandler::getEncoder(int j, double *v)
{
    bool ok = false;
    if (iJntEnc && iJntCoupling) {
        ok = ok && iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos);
        if (ok) {
            *v = physJointsPos[j];
            return ok;
        }
    }
    return ok;
}

bool ControlBoardCouplingHandler::getEncoders(double *encs)
{
    bool ok = false;
    if (iJntEnc && iJntCoupling) {
        ok = ok && iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos);
        if (ok) {
            encs = physJointsPos.data();
            return ok;
        }
    }
    return ok;
}

bool ControlBoardCouplingHandler::getEncodersTimed(double *encs, double *t)
{
    bool ok = false;
    if (iJntEnc && iJntCoupling) {
        // TODO t has to be probably resized
        ok = ok && iJntEnc->getEncodersTimed(actAxesPos.data(), t);
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos);
        if (ok) {
            encs = physJointsPos.data();
            return ok;
        }
    }
    return ok;
}

bool ControlBoardCouplingHandler::getEncoderTimed(int j, double *v, double *t)
{
    bool ok = false;
    if (iJntEnc && iJntCoupling) {
        ok = ok && iJntEnc->getEncodersTimed(actAxesPos.data(), t);
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsPos(actAxesPos, physJointsPos);
        if (ok) {
            *v = physJointsPos[j];
            return ok;
        }
    }
    return ok;
}

bool ControlBoardCouplingHandler::getEncoderSpeed(int j, double *sp)
{
    bool ok = false;
    if (iJntEnc && iJntCoupling) {
        ok = ok && iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntEnc->getEncoderSpeeds(actAxesVel.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsVel(actAxesPos, actAxesVel, physJointsVel);
        if (ok) {
            *sp = physJointsVel[j];
            return ok;
        }
    }
    return ok;
}

bool ControlBoardCouplingHandler::getEncoderSpeeds(double *spds)
{
    bool ok = false;
    if (iJntEnc && iJntCoupling) {
        ok = ok && iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntEnc->getEncoderSpeeds(actAxesVel.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsVel(actAxesPos, actAxesVel, physJointsVel);
        if (ok) {
            spds = physJointsVel.data();
            return ok;
        }
    }
    return ok;
}

bool ControlBoardCouplingHandler::getEncoderAcceleration(int j, double *acc)
{
    bool ok = false;
    if (iJntEnc && iJntCoupling) {
        ok = ok && iJntEnc->getEncoders(actAxesPos.data());
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

bool ControlBoardCouplingHandler::getEncoderAccelerations(double *accs)
{
    bool ok = false;
    if (iJntEnc && iJntCoupling) {
        ok = ok && iJntEnc->getEncoders(actAxesPos.data());
        ok = ok && iJntEnc->getEncoderSpeeds(actAxesVel.data());
        ok = ok && iJntEnc->getEncoderAccelerations(actAxesAcc.data());
        ok = ok && iJntCoupling->convertFromActuatedAxesToPhysicalJointsAcc(actAxesPos, actAxesVel, actAxesAcc, physJointsAcc);
        if (ok) {
            accs = physJointsAcc.data();
            return ok;
        }
    }
    return ok;
}

// /* IControlLimits */

// bool ControlBoardCouplingHandler::setLimits(int j, double min, double max)
// {
//     int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
//     size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

//     RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

//     if (!p)
//     {
//         return false;
//     }

//     if (p->lim)
//     {
//         return p->lim->setLimits(off,min, max);
//     }

//     return false;
// }

// bool ControlBoardCouplingHandler::getLimits(int j, double *min, double *max)
// {
//     int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
//     size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

//     RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

//     if (!p)
//     {
//         return false;
//     }

//     if (p->lim)
//     {
//         return p->lim->getLimits(off,min, max);
//     }

//     return false;
// }

// bool ControlBoardCouplingHandler::setVelLimits(int j, double min, double max)
// {
//     int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
//     size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

//     RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

//     if (!p)
//     {
//         return false;
//     }

//     if (!p->lim)
//     {
//         return false;
//     }

//     return p->lim->setVelLimits(off,min, max);
// }

// bool ControlBoardCouplingHandler::getVelLimits(int j, double *min, double *max)
// {
//     int off=(int)remappedControlBoards.lut[j].axisIndexInSubControlBoard;
//     size_t subIndex=remappedControlBoards.lut[j].subControlBoardIndex;

//     RemappedSubControlBoard *p=remappedControlBoards.getSubControlBoard(subIndex);

//     if (!p)
//     {
//         return false;
//     }

//     if(!p->lim)
//     {
//         return false;
//     }

//     return p->lim->getVelLimits(off,min, max);
// }

/* IAxisInfo */

bool ControlBoardCouplingHandler::getAxes(int *ax)
{
    bool ok{false};
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

bool ControlBoardCouplingHandler::getAxisName(int j, std::string& name)
{
    bool ok{false};
    if (iJntCoupling) {
        ok = iJntCoupling->getPhysicalJointName(j, name);
        return ok;
    }
    return ok;
}

bool ControlBoardCouplingHandler::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    bool ok{false};
    // TODO I am not sure how to handle this function
    type = VOCAB_JOINTTYPE_REVOLUTE;
    return ok;
}
