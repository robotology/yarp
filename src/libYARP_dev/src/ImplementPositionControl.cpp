/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/dev/ImplementPositionControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>

using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK_DEL(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete [] temp_int; delete [] temp; return false;}
#define MJOINTIDCHECK_DEL1(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete [] temp; return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementPositionControl::ImplementPositionControl(yarp::dev::IPositionControlRaw *y) :
    iPosition(y),
    helper(nullptr),
    nj(0)
{

}


ImplementPositionControl::~ImplementPositionControl()
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
bool ImplementPositionControl::initialize(int size, const int *amap, const double *enc, const double *zos)
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
bool ImplementPositionControl::uninitialize()
{
    if(helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
    }

    return true;
}

bool ImplementPositionControl::positionMove(int j, double ang)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->posA2E(ang, j, enc, k);
    return iPosition->positionMoveRaw(k, enc);
}

bool ImplementPositionControl::positionMove(const int n_joint, const int *joints, const double *refs)
{
    auto* temp = new double[nj];
    int *temp_int = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->posA2E(refs[idx], joints[idx], temp[idx], temp_int[idx]);
    }
    bool ret = iPosition->positionMoveRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl::positionMove(const double *refs)
{
    auto* pos =  new double [nj];
    castToMapper(helper)->posA2E(refs, pos);

    bool ret = iPosition->positionMoveRaw(pos);
    delete [] pos;
    return ret;
}

bool ImplementPositionControl::relativeMove(int j, double delta)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E(delta, j, enc, k);

    return iPosition->relativeMoveRaw(k,enc);
}

bool ImplementPositionControl::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    auto* temp = new double[nj];
    int *temp_int = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->velA2E(deltas[idx], joints[idx], temp[idx], temp_int[idx]);
    }
    bool ret = iPosition->relativeMoveRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl::relativeMove(const double *deltas)
{
    auto* temp = new double[nj];
    castToMapper(helper)->velA2E(deltas, temp);
    bool ret = iPosition->relativeMoveRaw(temp);
    delete [] temp;
    return ret;
}

bool ImplementPositionControl::checkMotionDone(int j, bool *flag)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);

    return iPosition->checkMotionDoneRaw(k,flag);
}

bool ImplementPositionControl::checkMotionDone(const int n_joint, const int *joints, bool *flags)
{
    int *temp = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        temp[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition->checkMotionDoneRaw(n_joint, temp, flags);
    delete [] temp;
    return ret;
}

bool ImplementPositionControl::checkMotionDone(bool *flag)
{
    bool *flags_tmp = new bool[nj];
    bool ret = iPosition->checkMotionDoneRaw(flags_tmp);
    for(int i=0; i<nj; i++)
    {
        flag[i] = flags_tmp[castToMapper(helper)->toHw(i)];
    }
    delete []flags_tmp;
    return ret;
}

bool ImplementPositionControl::setRefSpeed(int j, double sp)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E_abs(sp, j, enc, k);
    return iPosition->setRefSpeedRaw(k, enc);
}

bool ImplementPositionControl::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    auto* temp = new double[nj];
    int *temp_int = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->velA2E_abs(spds[idx], joints[idx], temp[idx], temp_int[idx]);
    }

    bool ret = iPosition->setRefSpeedsRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl::setRefSpeeds(const double *spds)
{
    auto* refs = new double[nj];
    castToMapper(helper)->velA2E_abs(spds, refs);

    bool ret = iPosition->setRefSpeedsRaw(refs);
    delete [] refs;
    return ret;
}

bool ImplementPositionControl::setRefAcceleration(int j, double acc)
{
    JOINTIDCHECK
    int k;
    double enc;

    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iPosition->setRefAccelerationRaw(k, enc);
}

bool ImplementPositionControl::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    auto* temp = new double[nj];
    int *temp_int = new int[nj];

    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], temp[idx], temp_int[idx]);
    }

    bool ret = iPosition->setRefAccelerationsRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl::setRefAccelerations(const double *accs)
{
    auto* refs = new double[nj];
    castToMapper(helper)->accA2E_abs(accs, refs);

    bool ret = iPosition->setRefAccelerationsRaw(refs);
    delete [] refs;
    return ret;
}

bool ImplementPositionControl::getRefSpeed(int j, double *ref)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    bool ret = iPosition->getRefSpeedRaw(k, &enc);

    *ref=(castToMapper(helper)->velE2A_abs(enc, k));

    return ret;
}

bool ImplementPositionControl::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    auto* temp = new double[nj];
    int *temp_int = new int[nj];

    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition->getRefSpeedsRaw(n_joint, temp_int, temp);

    for(int idx=0; idx<n_joint; idx++)
    {
        spds[idx]=castToMapper(helper)->velE2A_abs(temp[idx], temp_int[idx]);
    }
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl::getRefSpeeds(double *spds)
{
    auto* refs = new double[nj];
    bool ret = iPosition->getRefSpeedsRaw(refs);
    castToMapper(helper)->velE2A_abs(refs, spds);
    delete [] refs;
    return ret;
}

bool ImplementPositionControl::getRefAccelerations(double *accs)
{
    auto* refs = new double[nj];
    bool ret=iPosition->getRefAccelerationsRaw(refs);
    castToMapper(helper)->accE2A_abs(refs, accs);
    delete [] refs;
    return ret;
}

bool ImplementPositionControl::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    auto* temp = new double[nj];
    int *temp_int = new int[nj];

    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition->getRefAccelerationsRaw(n_joint, temp_int, temp);

    for(int idx=0; idx<n_joint; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(temp[idx], temp_int[idx]);
    }
    delete [] temp;
    delete [] temp_int;
    return ret;
}

bool ImplementPositionControl::getRefAcceleration(int j, double *acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iPosition->getRefAccelerationRaw(k, &enc);

    *acc=castToMapper(helper)->accE2A_abs(enc, k);

    return ret;
}

bool ImplementPositionControl::stop(int j)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPosition->stopRaw(k);
}

bool ImplementPositionControl::stop(const int n_joint, const int *joints)
{
    int *temp = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        temp[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition->stopRaw(n_joint, temp);
    delete [] temp;
    return ret;
}

bool ImplementPositionControl::stop()
{
    return iPosition->stopRaw();
}

bool ImplementPositionControl::getAxes(int *axis)
{
    (*axis)=castToMapper(helper)->axes();

    return true;
}


bool ImplementPositionControl::getTargetPosition(const int joint, double* ref)
{
    PJOINTIDCHECK(joint)
    int k;
    double enc;
    k=castToMapper(helper)->toHw(joint);
    bool ret = iPosition->getTargetPositionRaw(k, &enc);

    *ref=castToMapper(helper)->posE2A(enc, k);

    return ret;
}

bool ImplementPositionControl::getTargetPositions(double* refs)
{
    auto* trgPos = new double[nj];
    bool ret=iPosition->getTargetPositionsRaw(trgPos);
    castToMapper(helper)->posE2A(trgPos, refs);
    delete [] trgPos;
    return ret;
}

bool ImplementPositionControl::getTargetPositions(const int n_joint, const int* joints, double* refs)
{
    int * temp = new int [nj];

    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        temp[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    auto* trgPos =  new double[nj];
    bool ret = iPosition->getTargetPositionsRaw(n_joint, temp, trgPos);

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

bool StubImplPositionControlRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        yError("%s: not yet implemented\n", func);
    else
        yError("Function not yet implemented\n");

    return false;
}
