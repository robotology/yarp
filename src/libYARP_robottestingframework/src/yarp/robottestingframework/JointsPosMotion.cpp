/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robottestingframework/JointsPosMotion.h>

#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IControlMode.h>

#include <robottestingframework/TestAssert.h>

#include <cmath>

using yarp::dev::VOCAB_IM_STIFF;

class yarp::robottestingframework::jointsPosMotion::Private
{
public:
    void init(yarp::dev::PolyDriver *polydriver);
    size_t getIndexOfJoint(int j);
    void readJointsLimits();

    yarp::sig::Vector jointsList;
    yarp::sig::Vector encoders;
    yarp::sig::Vector speed;
    yarp::sig::Vector max_lims;
    yarp::sig::Vector min_lims;
    double tolerance;
    double timeout;
    size_t n_joints;

    yarp::dev::PolyDriver        *dd;
    yarp::dev::IPositionControl *ipos;
    yarp::dev::IControlMode     *icmd;
    yarp::dev::IInteractionMode  *iimd;
    yarp::dev::IEncoders         *ienc;
    yarp::dev::IControlLimits    *ilim;
};


void yarp::robottestingframework::jointsPosMotion::Private::init(yarp::dev::PolyDriver *polydriver)
{
    jointsList = 0;
    n_joints = 0;
    encoders = 0;
    speed = 0;

    tolerance = 1.0;
    timeout = 5.0;

    dd = polydriver;
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(dd->isValid(), "Unable to open device driver");
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(dd->view(ienc), "Unable to open encoders interface");
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(dd->view(ipos), "Unable to open position interface");
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(dd->view(icmd), "Unable to open control mode interface");
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(dd->view(iimd), "Unable to open interaction mode interface");
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(dd->view(ilim), "Unable to open limits interface");
}


size_t yarp::robottestingframework::jointsPosMotion::Private::getIndexOfJoint(int j)
{
    for(size_t i = 0; i < n_joints; i++) {
        if(jointsList[i] == j) {
            return i;
        }
    }
    return jointsList.size()+1;
}


void yarp::robottestingframework::jointsPosMotion::Private::readJointsLimits()
{
    max_lims.resize(n_joints);
    min_lims.resize(n_joints);
    for (size_t i = 0; i < n_joints; i++) {
        ilim->getLimits((int)jointsList[i], &min_lims[i], &max_lims[i]);
    }

}



yarp::robottestingframework::jointsPosMotion::jointsPosMotion(yarp::dev::PolyDriver *polydriver, yarp::sig::Vector &jlist) :
        mPriv(new Private)
{
    mPriv->init(polydriver);

    mPriv->n_joints = jlist.size();
    mPriv->jointsList.resize(mPriv->n_joints);
    mPriv->jointsList = jlist;

    mPriv->encoders.resize(mPriv->n_joints); mPriv->encoders.zero();
    mPriv->speed = yarp::sig::Vector(mPriv->n_joints, 10.0);

    //send default speed
    for (size_t i = 0; i < mPriv->n_joints; i++) {
        mPriv->ipos->setRefSpeed((int)mPriv->jointsList[i], mPriv->speed[i]);
    }
    mPriv->readJointsLimits();
}



yarp::robottestingframework::jointsPosMotion::~jointsPosMotion()
{
    delete mPriv;
}


void yarp::robottestingframework::jointsPosMotion::setTolerance(double tolerance)
{
    mPriv->tolerance = tolerance;
}


bool yarp::robottestingframework::jointsPosMotion::setAndCheckPosControlMode()
{
    for (size_t i = 0; i < mPriv->jointsList.size(); i++) {
        mPriv->icmd->setControlMode((int)mPriv->jointsList[i], VOCAB_CM_POSITION);
        mPriv->iimd->setInteractionMode((int)mPriv->jointsList[i], VOCAB_IM_STIFF);
        yarp::os::Time::delay(0.010);
    }

    int cmode;
    yarp::dev::InteractionModeEnum imode;
    double time_started = yarp::os::Time::now();

    while (1) {
        size_t ok = 0;
        for (size_t i = 0; i < mPriv->n_joints; i++) {
            mPriv->icmd->getControlMode ((int)mPriv->jointsList[i], &cmode);
            mPriv->iimd->getInteractionMode((int)mPriv->jointsList[i], &imode);
            if (cmode == VOCAB_CM_POSITION && imode == VOCAB_IM_STIFF) {
                ok++;
            }
        }
        if (ok == mPriv->n_joints) {
            break;
        }
        if (yarp::os::Time::now() - time_started > mPriv->timeout) {
            ROBOTTESTINGFRAMEWORK_ASSERT_ERROR("Unable to set control mode/interaction mode");
        }

        yarp::os::Time::delay(0.2);

    }

    return true;
}


void yarp::robottestingframework::jointsPosMotion::setTimeout(double timeout)
{
    mPriv->timeout = timeout;
}



void yarp::robottestingframework::jointsPosMotion::setSpeed(yarp::sig::Vector &speedlist)
{
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE((speedlist.size() != mPriv->jointsList.size()), "Speed list has a different size of joint list");
    mPriv->speed = speedlist;
    for (size_t i = 0; i < mPriv->n_joints; i++) {
        mPriv->ipos->setRefSpeed((int)mPriv->jointsList[i], mPriv->speed[i]);
    }
}


bool yarp::robottestingframework::jointsPosMotion::goToSingle(int j, double pos, double *reached_pos)
{
    size_t i = mPriv->getIndexOfJoint(j);
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(i < mPriv->n_joints, "Cannot move a joint not in list.");

    mPriv->ipos->positionMove((int)mPriv->jointsList[i], pos);
    double tmp = 0;

    double time_started = yarp::os::Time::now();
    bool ret = true;
    while (1) {
        mPriv->ienc->getEncoder((int)mPriv->jointsList[i], &tmp);
        if (fabs(tmp-pos)<mPriv->tolerance) {
            break;
        }

        if (yarp::os::Time::now()-time_started > mPriv->timeout) {
            ret  = false;
            break;
        }

        yarp::os::Time::delay(0.2);
    }

    if(reached_pos != nullptr) {
        *reached_pos = tmp;
    }
    return(ret);
}


bool yarp::robottestingframework::jointsPosMotion::goTo(yarp::sig::Vector positions, yarp::sig::Vector *reached_pos)
{
    for (unsigned int i=0; i<mPriv->n_joints; i++) {
        mPriv->ipos->positionMove((int)mPriv->jointsList[i], positions[i]);
    }

    double time_started = yarp::os::Time::now();
    yarp::sig::Vector tmp(mPriv->n_joints);tmp.zero();
    bool ret = true;

    while (1) {
        size_t in_position = 0;
        for (size_t i = 0; i < mPriv->n_joints; i++) {
            mPriv->ienc->getEncoder((int)mPriv->jointsList[i], &tmp[i]);
            if (fabs(tmp[i] - positions[i]) < mPriv->tolerance) {
                in_position++;
            }
        }
        if (in_position == mPriv->n_joints) {
            break;
        }

        if (yarp::os::Time::now()-time_started > mPriv->timeout) {
            ret = false;
            break;
        }
        yarp::os::Time::delay(0.2);
    }

    if(reached_pos != nullptr) {
        reached_pos->resize(mPriv->n_joints);
        *reached_pos = tmp;
    }
    return(ret);
}



bool yarp::robottestingframework::jointsPosMotion::checkJointLimitsReached(int j)
{
    size_t i = mPriv->getIndexOfJoint(j);

    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(i < mPriv->n_joints, "Cannot move a joint not in list.");

    double enc=0;
    mPriv->ienc->getEncoder((int)mPriv->jointsList[i], &enc);
    if (fabs(enc-mPriv->max_lims[i]) < mPriv->tolerance ||
            fabs(enc-mPriv->min_lims[i]) < mPriv->tolerance ) {
        return true;
    } else {
        return false;
    }
}
