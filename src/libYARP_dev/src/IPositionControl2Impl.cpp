/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>

using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK_DEL(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete [] temp_int; delete [] temp; return false;}
#define MJOINTIDCHECK_DEL1(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete [] temp; return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementPositionControl2::ImplementPositionControl2(IPositionControl2Raw *y) :
    iPosition2(y),
    helper(nullptr),
    nj(0)
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
    if(helper != nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(helper != nullptr);

    nj = size;
    return true;
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementPositionControl2::uninitialize()
{
    if(helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
    }

    return true;
}

bool ImplementPositionControl2::positionMove(int j, double ang)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->posA2E(ang, j, enc, k);
    return iPosition2->positionMoveRaw(k, enc);
}

bool ImplementPositionControl2::positionMove(const int n_joint, const int *joints, const double *refs)
{
    double *temp = new double[nj];
    int *temp_int = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->posA2E(refs[idx], joints[idx], temp[idx], temp_int[idx]);
    }
    bool ret = iPosition2->positionMoveRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl2::positionMove(const double *refs)
{
    double *pos =  new double [nj];
    castToMapper(helper)->posA2E(refs, pos);

    bool ret = iPosition2->positionMoveRaw(pos);
    delete [] pos;
    return ret;
}

bool ImplementPositionControl2::relativeMove(int j, double delta)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E(delta, j, enc, k);

    return iPosition2->relativeMoveRaw(k,enc);
}

bool ImplementPositionControl2::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    double *temp = new double[nj];
    int *temp_int = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->velA2E(deltas[idx], joints[idx], temp[idx], temp_int[idx]);
    }
    bool ret = iPosition2->relativeMoveRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl2::relativeMove(const double *deltas)
{
    double *temp = new double[nj];
    castToMapper(helper)->velA2E(deltas, temp);
    bool ret = iPosition2->relativeMoveRaw(temp);
    delete [] temp;
    return ret;
}

bool ImplementPositionControl2::checkMotionDone(int j, bool *flag)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);

    return iPosition2->checkMotionDoneRaw(k,flag);
}

bool ImplementPositionControl2::checkMotionDone(const int n_joint, const int *joints, bool *flags)
{
    int *temp = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        temp[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition2->checkMotionDoneRaw(n_joint, temp, flags);
    delete [] temp;
    return ret;
}

bool ImplementPositionControl2::checkMotionDone(bool *flag)
{
    return iPosition2->checkMotionDoneRaw(flag);
}

bool ImplementPositionControl2::setRefSpeed(int j, double sp)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E_abs(sp, j, enc, k);
    return iPosition2->setRefSpeedRaw(k, enc);
}

bool ImplementPositionControl2::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    double *temp = new double[nj];
    int *temp_int = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->velA2E_abs(spds[idx], joints[idx], temp[idx], temp_int[idx]);
    }

    bool ret = iPosition2->setRefSpeedsRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl2::setRefSpeeds(const double *spds)
{
    double *refs = new double[nj];
    castToMapper(helper)->velA2E_abs(spds, refs);

    bool ret = iPosition2->setRefSpeedsRaw(refs);
    delete [] refs;
    return ret;
}

bool ImplementPositionControl2::setRefAcceleration(int j, double acc)
{
    JOINTIDCHECK
    int k;
    double enc;

    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iPosition2->setRefAccelerationRaw(k, enc);
}

bool ImplementPositionControl2::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    double *temp = new double[nj];
    int *temp_int = new int[nj];

    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], temp[idx], temp_int[idx]);
    }

    bool ret = iPosition2->setRefAccelerationsRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl2::setRefAccelerations(const double *accs)
{
    double *refs = new double[nj];
    castToMapper(helper)->accA2E_abs(accs, refs);

    bool ret = iPosition2->setRefAccelerationsRaw(refs);
    delete [] refs;
    return ret;
}

bool ImplementPositionControl2::getRefSpeed(int j, double *ref)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    bool ret = iPosition2->getRefSpeedRaw(k, &enc);

    *ref=(castToMapper(helper)->velE2A_abs(enc, k));

    return ret;
}

bool ImplementPositionControl2::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    double *temp = new double[nj];
    int *temp_int = new int[nj];

    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition2->getRefSpeedsRaw(n_joint, temp_int, temp);

    for(int idx=0; idx<n_joint; idx++)
    {
        spds[idx]=castToMapper(helper)->velE2A_abs(temp[idx], temp_int[idx]);
    }
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl2::getRefSpeeds(double *spds)
{
    double *refs = new double[nj];
    bool ret = iPosition2->getRefSpeedsRaw(refs);
    castToMapper(helper)->velE2A_abs(refs, spds);
    delete [] refs;
    return ret;
}

bool ImplementPositionControl2::getRefAccelerations(double *accs)
{
    double *refs = new double[nj];
    bool ret=iPosition2->getRefAccelerationsRaw(refs);
    castToMapper(helper)->accE2A_abs(refs, accs);
    delete refs;
    return ret;
}

bool ImplementPositionControl2::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    double *temp = new double[nj];
    int *temp_int = new int[nj];

    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition2->getRefAccelerationsRaw(n_joint, temp_int, temp);

    for(int idx=0; idx<n_joint; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(temp[idx], temp_int[idx]);
    }
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl2::getRefAcceleration(int j, double *acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iPosition2->getRefAccelerationRaw(k, &enc);

    *acc=castToMapper(helper)->accE2A_abs(enc, k);

    return ret;
}

bool ImplementPositionControl2::stop(int j)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPosition2->stopRaw(k);
}

bool ImplementPositionControl2::stop(const int n_joint, const int *joints)
{
    int *temp = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        temp[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition2->stopRaw(n_joint, temp);
    delete [] temp;
    return ret;
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


bool ImplementPositionControl2::getTargetPosition(const int joint, double* ref)
{
    PJOINTIDCHECK(joint)
    int k;
    double enc;
    k=castToMapper(helper)->toHw(joint);
    bool ret = iPosition2->getTargetPositionRaw(k, &enc);

    *ref=castToMapper(helper)->posE2A(enc, k);

    return ret;
}

bool ImplementPositionControl2::getTargetPositions(double* refs)
{
    double *trgPos = new double[nj];
    bool ret=iPosition2->getTargetPositionsRaw(trgPos);
    castToMapper(helper)->posE2A(trgPos, refs);
    delete [] trgPos;
    return ret;
}

bool ImplementPositionControl2::getTargetPositions(const int n_joint, const int* joints, double* refs)
{
    int * temp = new int [nj];
    double *trgPos =  new double[nj];

    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        temp[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition2->getTargetPositionsRaw(n_joint, temp, trgPos);

    for(int idx=0; idx<n_joint; idx++)
    {
        refs[idx]=castToMapper(helper)->posE2A(trgPos[idx], temp[idx]);
    }
    delete [] temp;
    delete [] trgPos;
    return ret;
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
