// -*- mode:C++ { } tab-width:4 { } c-basic-offset:4 { } indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Valentina Gaggero
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <math.h>
#include <rtf/TestAssert.h>
#include <yarp/os/Time.h>
#include <rtf/yarp/JointsPosMotion.h>

using namespace RTF;
using namespace RTF::YARP;
using namespace yarp::os;
using namespace yarp::dev;



//jointsPosMotion::jointsPosMotion(){}
void jointsPosMotion::init(yarp::dev::PolyDriver *polydriver)
{

    jointsList = 0;
    n_joints = 0;
    encoders = 0;
    speed = 0;

    tolerance = 1.0;
    timeout = 5.0;

    dd = polydriver;
    RTF_ASSERT_ERROR_IF(dd->isValid(),"Unable to open device driver");
    RTF_ASSERT_ERROR_IF(dd->view(ienc),"Unable to open encoders interface");
    RTF_ASSERT_ERROR_IF(dd->view(ipos),"Unable to open position interface");
    RTF_ASSERT_ERROR_IF(dd->view(icmd),"Unable to open control mode interface");
    RTF_ASSERT_ERROR_IF(dd->view(iimd),"Unable to open interaction mode interface");
    RTF_ASSERT_ERROR_IF(dd->view(ilim),"Unable to open limits interface");

}


jointsPosMotion::jointsPosMotion(yarp::dev::PolyDriver *polydriver, yarp::sig::Vector &jlist)
{
    init(polydriver);

    n_joints = jlist.size();
    jointsList.resize(n_joints);
    jointsList = jlist;

    encoders.resize(n_joints); encoders.zero();
    speed = yarp::sig::Vector(n_joints, 10.0);

    //send default speed
    for (unsigned int i=0; i<n_joints; i++)
    {
        ipos->setRefSpeed((int)jointsList[i],speed[i]);
    }
    readJointsLimits();

}

jointsPosMotion::~jointsPosMotion()
{
   // if (dd) {delete dd; dd =0;}
}


void jointsPosMotion::setTolerance(double tolerance) {tolerance = tolerance;}


bool jointsPosMotion::setAndCheckPosControlMode()
{
    for (unsigned int i=0; i<jointsList.size(); i++)
    {
        icmd->setControlMode((int)jointsList[i],VOCAB_CM_POSITION);
        iimd->setInteractionMode((int)jointsList[i],VOCAB_IM_STIFF);
        yarp::os::Time::delay(0.010);
    }

    int cmode;
    yarp::dev::InteractionModeEnum imode;
    double time_started = yarp::os::Time::now();

    while (1)
    {
        int ok=0;
        for (unsigned int i=0; i<n_joints; i++)
        {
            icmd->getControlMode ((int)jointsList[i],&cmode);
            iimd->getInteractionMode((int)jointsList[i],&imode);
            if (cmode==VOCAB_CM_POSITION && imode==VOCAB_IM_STIFF) ok++;
        }
        if (ok==n_joints) break;
        if (yarp::os::Time::now()-time_started > timeout)
        {
            RTF_ASSERT_ERROR("Unable to set control mode/interaction mode");
        }

        yarp::os::Time::delay(0.2);

    }

    return true;
}

void jointsPosMotion::setTimeout(double timeout){timeout = timeout;}

int jointsPosMotion::getIndexOfJoint(int j)
{
    for(int i=0; i<n_joints; i++)
    {
        if(jointsList[i] == j)
            return i;
    }
    return jointsList.size()+1;
}

void jointsPosMotion::setSpeed(yarp::sig::Vector &speedlist)
{
    RTF_ASSERT_ERROR_IF((speedlist.size() != jointsList.size()), "speed list has a different size of joint list");
    speed = speedlist;
    for (unsigned int i=0; i<n_joints; i++)
    {
        ipos->setRefSpeed((int)jointsList[i],speed[i]);
    }
}



bool jointsPosMotion::goToSingle(int j, double pos, double *reached_pos)
{
    int i = getIndexOfJoint(j);
    RTF_ASSERT_ERROR_IF(i<n_joints, "cannot move a joint not in list.");

    ipos->positionMove((int)jointsList[i],pos);
    double tmp=0;

    double time_started = yarp::os::Time::now();
    bool ret = true;
    while (1)
    {
        ienc->getEncoder((int)jointsList[i],&tmp);
        if (fabs(tmp-pos)<tolerance)
            break;

        if (yarp::os::Time::now()-time_started > timeout)
        {
            ret  = false;
            break;
        }

        yarp::os::Time::delay(0.2);
    }

    if(reached_pos != NULL)
    {
        *reached_pos = tmp;
    }
    return(ret);
}


bool jointsPosMotion::goTo(yarp::sig::Vector positions, yarp::sig::Vector *reached_pos)
{
    for (unsigned int i=0; i<n_joints; i++)
    {
        ipos->positionMove((int)jointsList[i],positions[i]);
    }

    double time_started = yarp::os::Time::now();
    yarp::sig::Vector tmp(n_joints);tmp.zero();
    bool ret = true;

    while (1)
    {
        int in_position=0;
        for (unsigned int i=0; i<n_joints; i++)
        {
            ienc->getEncoder((int)jointsList[i],&tmp[i]);
            if (fabs(tmp[i]-positions[i])<tolerance)
                in_position++;
        }
        if (in_position==n_joints)
            break;

        if (yarp::os::Time::now()-time_started > timeout)
        {
            ret = false;
            break;
        }
        yarp::os::Time::delay(0.2);
    }

    if(reached_pos != NULL)
    {
        reached_pos->resize(n_joints);
        *reached_pos = tmp;
    }
    return(ret);
}

void jointsPosMotion::readJointsLimits(void)
{
    max_lims.resize(n_joints);
    min_lims.resize(n_joints);
    for (int i=0; i <n_joints; i++)
    {
        ilim->getLimits((int)jointsList[i],&min_lims[i],&max_lims[i]);
    }

}


bool jointsPosMotion::checkJointLimitsReached(int j)
{
    int i = getIndexOfJoint(j);

    RTF_ASSERT_ERROR_IF(i<n_joints, "cannot move a joint not in list.");

    double enc=0;
    ienc->getEncoder((int)jointsList[i],&enc);
    if (fabs(enc-max_lims[i]) < tolerance ||
         fabs(enc-min_lims[i]) < tolerance )
        return true;
    else
        return false;
}

