// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <stdio.h>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>

using namespace yarp::dev;

ImplementPositionControl2::ImplementPositionControl2(IPositionControl2Raw *y) :
    iPosition2(y),
    helper(NULL),
    temp_int(NULL),
    temp_double(NULL)
{

}


ImplementPositionControl2::~ImplementPositionControl2()
{
    uninitialize();
}

/**
 * Allocate memory for internal data
 * @param size the number of joints
 * @param amap axis map for this device wrapper
 * @param enc encoder conversion factor, from high level to hardware
 * @param zos offset for setting the zero point. Units are relative to high level user interface (degrees)
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementPositionControl2::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if(helper != NULL)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,0));
    yAssert(helper != NULL);
    temp_double=new double [size];
    yAssert(temp_double != NULL);

    temp_int=new int [size];
    yAssert(temp_int != NULL);
    return true;
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementPositionControl2::uninitialize()
{
    if(helper != NULL)
    {
        delete castToMapper(helper);
        helper = NULL;
    }
    checkAndDestroy(temp_double);
    checkAndDestroy(temp_int);

    return true;
}

bool ImplementPositionControl2::setPositionMode()
{
    return iPosition2->setPositionModeRaw();
}

bool ImplementPositionControl2::positionMove(int j, double ang)
{
    int k;
    double enc;
    castToMapper(helper)->posA2E(ang, j, enc, k);
    return iPosition2->positionMoveRaw(k, enc);
}

bool ImplementPositionControl2::positionMove(const int n_joint, const int *joints, const double *refs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        castToMapper(helper)->posA2E(refs[idx], joints[idx], temp_double[idx], temp_int[idx]);
    }
    return iPosition2->positionMoveRaw(n_joint, temp_int, temp_double);
}

bool ImplementPositionControl2::positionMove(const double *refs)
{
    castToMapper(helper)->posA2E(refs, temp_double);

    return iPosition2->positionMoveRaw(temp_double);
}

bool ImplementPositionControl2::relativeMove(int j, double delta)
{
    int k;
    double enc;
    castToMapper(helper)->velA2E(delta, j, enc, k);

    return iPosition2->relativeMoveRaw(k,enc);
}

bool ImplementPositionControl2::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        castToMapper(helper)->velA2E(deltas[idx], joints[idx], temp_double[idx], temp_int[idx]);
    }
    return iPosition2->relativeMoveRaw(n_joint, temp_int, temp_double);
}

bool ImplementPositionControl2::relativeMove(const double *deltas)
{
    castToMapper(helper)->velA2E(deltas, temp_double);
    return iPosition2->relativeMoveRaw(temp_double);
}

bool ImplementPositionControl2::checkMotionDone(int j, bool *flag)
{
    int k=castToMapper(helper)->toHw(j);

    return iPosition2->checkMotionDoneRaw(k,flag);
}

bool ImplementPositionControl2::checkMotionDone(const int n_joint, const int *joints, bool *flags)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    return iPosition2->checkMotionDoneRaw(n_joint, temp_int, flags);
}

bool ImplementPositionControl2::checkMotionDone(bool *flag)
{
    return iPosition2->checkMotionDoneRaw(flag);
}

bool ImplementPositionControl2::setRefSpeed(int j, double sp)
{
    int k;
    double enc;
    castToMapper(helper)->velA2E_abs(sp, j, enc, k);
    return iPosition2->setRefSpeedRaw(k, enc);
}

bool ImplementPositionControl2::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        castToMapper(helper)->velA2E_abs(spds[idx], joints[idx], temp_double[idx], temp_int[idx]);
    }

    return iPosition2->setRefSpeedsRaw(n_joint, temp_int, temp_double);
}

bool ImplementPositionControl2::setRefSpeeds(const double *spds)
{
    castToMapper(helper)->velA2E_abs(spds, temp_double);

    return iPosition2->setRefSpeedsRaw(temp_double);
}

bool ImplementPositionControl2::setRefAcceleration(int j, double acc)
{
    int k;
    double enc;

    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iPosition2->setRefAccelerationRaw(k, enc);
}

bool ImplementPositionControl2::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], temp_double[idx], temp_int[idx]);
    }

    return iPosition2->setRefAccelerationsRaw(n_joint, temp_int, temp_double);
}

bool ImplementPositionControl2::setRefAccelerations(const double *accs)
{
    castToMapper(helper)->accA2E_abs(accs, temp_double);

    return iPosition2->setRefAccelerationsRaw(temp_double);
}

bool ImplementPositionControl2::getRefSpeed(int j, double *ref)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    bool ret = iPosition2->getRefSpeedRaw(k, &enc);

    *ref=(castToMapper(helper)->velE2A_abs(enc, k));

    return ret;
}

bool ImplementPositionControl2::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition2->getRefSpeedsRaw(n_joint, temp_int, temp_double);

    for(int idx=0; idx<n_joint; idx++)
    {
        spds[idx]=castToMapper(helper)->velE2A_abs(temp_double[idx], temp_int[idx]);
    }

    return ret;
}

bool ImplementPositionControl2::getRefSpeeds(double *spds)
{
    bool ret = iPosition2->getRefSpeedsRaw(temp_double);
    castToMapper(helper)->velE2A_abs(temp_double, spds);
    return ret;
}

bool ImplementPositionControl2::getRefAccelerations(double *accs)
{
    bool ret=iPosition2->getRefAccelerationsRaw(temp_double);
    castToMapper(helper)->accE2A_abs(temp_double, accs);
    return ret;
}

bool ImplementPositionControl2::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition2->getRefAccelerationsRaw(n_joint, temp_int, temp_double);

    for(int idx=0; idx<n_joint; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(temp_double[idx], temp_int[idx]);
    }
    return ret;
}

bool ImplementPositionControl2::getRefAcceleration(int j, double *acc)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iPosition2->getRefAccelerationRaw(k, &enc);

    *acc=castToMapper(helper)->accE2A_abs(enc, k);

    return ret;
}

bool ImplementPositionControl2::stop(int j)
{
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPosition2->stopRaw(k);
}

bool ImplementPositionControl2::stop(const int n_joint, const int *joints)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    return iPosition2->stopRaw(n_joint, temp_int);
}

bool ImplementPositionControl2::stop()
{
    return iPosition2->stopRaw();
}

bool ImplementPositionControl2::getAxes(int *axis)
{
    (*axis)=castToMapper(helper)->axes();

    return true;
}


/////////////////// End Implement PostionControl2



// Stub interface

bool StubImplPositionControl2Raw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        yError("%s: not yet implemented\n", func);
    else
        yError("Function not yet implemented\n");

    return false;
}
