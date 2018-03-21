/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <cstdio>
using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementTorqueControl::ImplementTorqueControl(ITorqueControlRaw *tq)
{
    iTorqueRaw = tq;
    helper=nullptr;
    temp=nullptr;
//     fake =0;
    temp2=nullptr;
    temp_int=nullptr;
    tmpPids=nullptr;
}

ImplementTorqueControl::~ImplementTorqueControl()
{
    uninitialize();
}

bool ImplementTorqueControl::initialize(int size, const int *amap, const double *enc, const double *zos, const double *nw)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos, nw));
    yAssert (helper != nullptr);
    temp=new double [size];
    yAssert (temp != nullptr);
    temp2=new double [size];
    yAssert (temp2 != nullptr);
    temp_int=new int [size];
    yAssert (temp_int != nullptr);
    tmpPids=new Pid[size];
    yAssert (tmpPids!=nullptr);

    return true;
}

bool ImplementTorqueControl::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }
    checkAndDestroy(temp);
    checkAndDestroy(temp2);
    checkAndDestroy(temp_int);
    checkAndDestroy(tmpPids);

    return true;
}

bool ImplementTorqueControl::getAxes(int *axes)
{
    return iTorqueRaw->getAxes(axes);
}

bool ImplementTorqueControl::getRefTorque(int j, double *r)
{
    JOINTIDCHECK
    int k;
    bool ret;
    double torque;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->getRefTorqueRaw(k, &torque);
    *r=castToMapper(helper)->trqS2N(torque, k);
    return ret;
}

bool ImplementTorqueControl::getBemfParam(int j, double *bemf)
{
    JOINTIDCHECK
    int k;
    bool ret;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->getBemfParamRaw(k, bemf);
    return ret;
}

bool ImplementTorqueControl::setBemfParam(int j, double bemf)
{
    JOINTIDCHECK
    int k;
    bool ret;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->setBemfParamRaw(k, bemf);
    return ret;
}

bool ImplementTorqueControl::setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->setMotorTorqueParamsRaw(k, params);
}

bool ImplementTorqueControl::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    JOINTIDCHECK
  int k=castToMapper(helper)->toHw(j);
  return iTorqueRaw->getMotorTorqueParamsRaw(k, params);
}

bool ImplementTorqueControl::getRefTorques(double *t)
{
    bool ret;
    ret = iTorqueRaw->getRefTorquesRaw(temp);
    castToMapper(helper)->trqS2N(temp,t);
    return ret;
}

bool ImplementTorqueControl::setRefTorques(const double *t)
{
    castToMapper(helper)->trqN2S(t, temp);
    return iTorqueRaw->setRefTorquesRaw(temp);
}

bool ImplementTorqueControl::setRefTorque(int j, double t)
{
    JOINTIDCHECK
    int k;
    double sens;
    castToMapper(helper)->trqN2S(t,j,sens,k);
    return iTorqueRaw->setRefTorqueRaw(k, sens);
}

bool ImplementTorqueControl::getTorques(double *t)
{
    bool ret = iTorqueRaw->getTorquesRaw(temp);
    castToMapper(helper)->toUser(temp, t);
    return ret;
}

bool ImplementTorqueControl::setRefTorques(const int n_joint, const int *joints, const double *t)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        castToMapper(helper)->trqN2S(t[idx], joints[idx], temp[idx], temp_int[idx]);
    }
    return iTorqueRaw->setRefTorquesRaw(n_joint, temp_int, temp);
}

bool ImplementTorqueControl::getTorque(int j, double *t)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueRaw(k, t);
}

bool ImplementTorqueControl::getTorqueRanges(double *min, double *max)
{
    bool ret = iTorqueRaw->getTorqueRangesRaw(temp,temp2);
    castToMapper(helper)->toUser(temp, min);
    castToMapper(helper)->toUser(temp2, max);
    return ret;
}

bool ImplementTorqueControl::getTorqueRange(int j, double *min, double *max)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueRangeRaw(k, min, max);
}
