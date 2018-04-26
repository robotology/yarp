/*
 * Copyright (C) 2013 Istituto Italiano di Tecnologia (IIT)
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>

using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementVelocityControl2::ImplementVelocityControl2(IVelocityControl2Raw *y) :
    iVelocity2(y),
    helper(nullptr),
    temp_int(nullptr),
    temp_double(nullptr),
    tempPids()
{

}

ImplementVelocityControl2::~ImplementVelocityControl2()
{
    uninitialize();
}

bool ImplementVelocityControl2::initialize(int size, const int *axis_map, const double *enc, const double *zeros)
{
    if (helper != nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, axis_map, enc, zeros,nullptr));
    yAssert (helper != nullptr);
    temp_double = new double [size];
    yAssert (temp_double != nullptr);
    temp_int = new int [size];
    yAssert (temp_int != nullptr);
    tempPids = new Pid [size];
    yAssert (tempPids != nullptr);

    return true;
}

bool ImplementVelocityControl2::uninitialize()
{
    if(helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
    }
    checkAndDestroy(temp_double);
    checkAndDestroy(temp_int);
    checkAndDestroy(tempPids);

    return true;
}

bool ImplementVelocityControl2::getAxes(int *ax)
{
    (*ax)=castToMapper(helper)->axes();
    return true;
}

bool ImplementVelocityControl2::velocityMove(int j, double sp)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E(sp, j, enc, k);
    return iVelocity2->velocityMoveRaw(k, enc);
}

bool ImplementVelocityControl2::velocityMove(const int n_joint, const int *joints, const double *spds)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        castToMapper(helper)->velA2E(spds[idx], joints[idx], temp_double[idx], temp_int[idx]);
    }
    return iVelocity2->velocityMoveRaw(n_joint, temp_int, temp_double);
}

bool ImplementVelocityControl2::velocityMove(const double *sp)
{
    castToMapper(helper)->velA2E(sp, temp_double);
    return iVelocity2->velocityMoveRaw(temp_double);
}

bool ImplementVelocityControl2::getRefVelocity(const int j, double* vel)
{
    JOINTIDCHECK
    int k;
    double tmp;
    k=castToMapper(helper)->toHw(j);
    bool ret = iVelocity2->getRefVelocityRaw(k, &tmp);
    *vel=castToMapper(helper)->velE2A(tmp, k);
    return ret;
}

bool ImplementVelocityControl2::getRefVelocities(double *vels)
{
    bool ret=iVelocity2->getRefVelocitiesRaw(temp_double);
    castToMapper(helper)->velE2A(temp_double, vels);
    return ret;
}

bool ImplementVelocityControl2::getRefVelocities(const int n_joint, const int *joints, double *vels)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iVelocity2->getRefVelocitiesRaw(n_joint, temp_int, temp_double);

    for(int idx=0; idx<n_joint; idx++)
    {
        vels[idx]=castToMapper(helper)->velE2A(temp_double[idx], temp_int[idx]);
    }
    return ret;
}

bool ImplementVelocityControl2::setRefAcceleration(int j, double acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iVelocity2->setRefAccelerationRaw(k, enc);
}

bool ImplementVelocityControl2::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], temp_double[idx], temp_int[idx]);
    }
    return iVelocity2->setRefAccelerationsRaw(n_joint, temp_int, temp_double);
}

bool ImplementVelocityControl2::setRefAccelerations(const double *accs)
{
    castToMapper(helper)->accA2E_abs(accs, temp_double);
    return iVelocity2->setRefAccelerationsRaw(temp_double);
}

bool ImplementVelocityControl2::getRefAcceleration(int j, double *acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iVelocity2->getRefAccelerationRaw(k, &enc);
    *acc=castToMapper(helper)->accE2A_abs(enc, k);
    return ret;
}

bool ImplementVelocityControl2::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iVelocity2->getRefAccelerationsRaw(n_joint, temp_int, temp_double);

    for(int idx=0; idx<n_joint; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(temp_double[idx], temp_int[idx]);
    }
    return ret;
}


bool ImplementVelocityControl2::getRefAccelerations(double *accs)
{
    bool ret=iVelocity2->getRefAccelerationsRaw(temp_double);
    castToMapper(helper)->accE2A_abs(temp_double, accs);
    return ret;
}


bool ImplementVelocityControl2::stop(int j)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iVelocity2->stopRaw(k);
}


bool ImplementVelocityControl2::stop(const int n_joint, const int *joints)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    return iVelocity2->stopRaw(n_joint, temp_int);
}


bool ImplementVelocityControl2::stop()
{
    return iVelocity2->stopRaw();
}
