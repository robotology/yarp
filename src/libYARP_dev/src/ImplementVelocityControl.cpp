/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/dev/ImplementVelocityControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>

using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK_DEL1(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete[] tmp_joints; return false;}
#define MJOINTIDCHECK_DEL2(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete[] tmp_joints; delete[] tmp_vals; return false;}

ImplementVelocityControl::ImplementVelocityControl(IVelocityControlRaw *y) :
    iVelocity(y),
    helper(nullptr),
    nj(0)
{

}

ImplementVelocityControl::~ImplementVelocityControl()
{
    uninitialize();
}

bool ImplementVelocityControl::initialize(int size, const int *axis_map, const double *enc, const double *zeros)
{
    if (helper != nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, axis_map, enc, zeros));
    yAssert (helper != nullptr);
    nj=size;
    return true;
}

bool ImplementVelocityControl::uninitialize()
{
    if(helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
    }
    return true;
}

bool ImplementVelocityControl::getAxes(int *ax)
{
    (*ax)=castToMapper(helper)->axes();
    return true;
}

bool ImplementVelocityControl::velocityMove(int j, double sp)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E(sp, j, enc, k);
    return iVelocity->velocityMoveRaw(k, enc);
}

bool ImplementVelocityControl::velocityMove(const int n_joint, const int *joints, const double *spds)
{
    int *tmp_joints=new int[nj];
    auto* tmp_vals=new double[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL2(idx)
        castToMapper(helper)->velA2E(spds[idx], joints[idx], tmp_vals[idx], tmp_joints[idx]);
    }
    bool ret = iVelocity->velocityMoveRaw(n_joint, tmp_joints, tmp_vals);
    delete [] tmp_joints;
    delete [] tmp_vals;
    return ret;
}

bool ImplementVelocityControl::velocityMove(const double *sp)
{
    auto* tmp=new double[nj];
    castToMapper(helper)->velA2E(sp, tmp);
    bool ret = iVelocity->velocityMoveRaw(tmp);
    delete [] tmp;
    return ret;
}

bool ImplementVelocityControl::getRefVelocity(const int j, double* vel)
{
    JOINTIDCHECK
    int k;
    double tmp;
    k=castToMapper(helper)->toHw(j);
    bool ret = iVelocity->getRefVelocityRaw(k, &tmp);
    *vel=castToMapper(helper)->velE2A(tmp, k);
    return ret;
}

bool ImplementVelocityControl::getRefVelocities(double *vels)
{
    auto* tmp=new double[nj];
    bool ret=iVelocity->getRefVelocitiesRaw(tmp);
    castToMapper(helper)->velE2A(tmp, vels);
    delete [] tmp;
    return ret;
}

bool ImplementVelocityControl::getRefVelocities(const int n_joint, const int *joints, double *vels)
{
    int *tmp_joints = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        tmp_joints[idx]=castToMapper(helper)->toHw(joints[idx]);
    }
    auto* tmp_vels =new double [nj];
    bool ret = iVelocity->getRefVelocitiesRaw(n_joint, tmp_joints, tmp_vels);

    for(int idx=0; idx<n_joint; idx++)
    {
        vels[idx]=castToMapper(helper)->velE2A(tmp_vels[idx], tmp_joints[idx]);
    }
    delete [] tmp_vels;
    return ret;
}

bool ImplementVelocityControl::setRefAcceleration(int j, double acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iVelocity->setRefAccelerationRaw(k, enc);
}

bool ImplementVelocityControl::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    int *tmp_joints=new int[nj];
    auto* tmp_vals=new double[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL2(idx)
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], tmp_vals[idx], tmp_joints[idx]);
    }
    bool ret = iVelocity->setRefAccelerationsRaw(n_joint, tmp_joints, tmp_vals);
    delete [] tmp_joints;
    delete [] tmp_vals;
    
    return ret;
}

bool ImplementVelocityControl::setRefAccelerations(const double *accs)
{
    auto* tmp=new double[nj];
    castToMapper(helper)->accA2E_abs(accs, tmp);
    bool ret = iVelocity->setRefAccelerationsRaw(tmp);
    delete[] tmp;
    return ret;
}

bool ImplementVelocityControl::getRefAcceleration(int j, double *acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iVelocity->getRefAccelerationRaw(k, &enc);
    *acc=castToMapper(helper)->accE2A_abs(enc, k);
    return ret;
}

bool ImplementVelocityControl::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    int * tmp_joints=new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        tmp_joints[idx]=castToMapper(helper)->toHw(joints[idx]);
    }
    auto* tmp_accs=new double[nj];
    bool ret = iVelocity->getRefAccelerationsRaw(n_joint, tmp_joints, tmp_accs);

    for(int idx=0; idx<n_joint; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(tmp_accs[idx], tmp_joints[idx]);
    }
    delete[]tmp_joints;
    delete[]tmp_accs;
    return ret;
}


bool ImplementVelocityControl::getRefAccelerations(double *accs)
{
    auto* tmp=new double[nj];
    bool ret=iVelocity->getRefAccelerationsRaw(tmp);
    castToMapper(helper)->accE2A_abs(tmp, accs);
    delete[]tmp;
    return ret;
}


bool ImplementVelocityControl::stop(int j)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iVelocity->stopRaw(k);
}


bool ImplementVelocityControl::stop(const int n_joint, const int *joints)
{
    int *tmp_joints=new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        tmp_joints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    bool ret = iVelocity->stopRaw(n_joint, tmp_joints);
    delete[]tmp_joints;
    return ret;
}


bool ImplementVelocityControl::stop()
{
    return iVelocity->stopRaw();
}
