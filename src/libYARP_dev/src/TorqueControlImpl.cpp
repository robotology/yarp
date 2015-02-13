// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * 
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <stdio.h>
using namespace yarp::dev;

ImplementTorqueControl::ImplementTorqueControl(ITorqueControlRaw *tq)
{
    iTorqueRaw = tq;
    helper=0;
    temp=0;
    temp2=0;
    tmpPids=0;
}

ImplementTorqueControl::~ImplementTorqueControl()
{
    uninitialize();
}

bool ImplementTorqueControl::initialize(int size, const int *amap, const double *enc, const double *zos, const double *nw)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos, nw));
    yAssert (helper != 0);
    temp=new double [size];
    yAssert (temp != 0);
    temp2=new double [size];
    yAssert (temp2 != 0);
    tmpPids=new Pid[size];
    yAssert (tmpPids!=0);

    return true;
}

bool ImplementTorqueControl::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    checkAndDestroy(temp);
    checkAndDestroy(temp2);
    checkAndDestroy(tmpPids);

    return true;
}

bool ImplementTorqueControl::getAxes(int *axes)
{
    return iTorqueRaw->getAxes(axes);
}

bool ImplementTorqueControl::setTorqueMode()
{
    return iTorqueRaw->setTorqueModeRaw();
}

bool ImplementTorqueControl::getRefTorque(int j, double *r)
{
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
    int k;
    bool ret;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->getBemfParamRaw(k, bemf);
    return ret;
}

bool ImplementTorqueControl::setBemfParam(int j, double bemf)
{
    int k;
    bool ret;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->setBemfParamRaw(k, bemf);
    return ret;
}

bool ImplementTorqueControl::setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params) 
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->setMotorTorqueParamsRaw(k, params);
}

bool ImplementTorqueControl::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params) 
{
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

bool ImplementTorqueControl::getTorque(int j, double *t)
{
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
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueRangeRaw(k, min, max);
}

bool ImplementTorqueControl::setTorquePid(int j, const Pid &pid)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->setTorquePidRaw(k, pid);
}

bool ImplementTorqueControl::setTorquePids(const Pid *pids)
{
    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toHw(j);
        tmpPids[tmp]=pids[j];
    }
    
    return iTorqueRaw->setTorquePidsRaw(tmpPids);
}

bool ImplementTorqueControl::setTorqueErrorLimit(int j, double limit)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->setTorqueErrorLimitRaw(k, limit);
}

bool ImplementTorqueControl::getTorqueErrorLimit(int j, double *limit)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueErrorLimitRaw(k, limit);
}

bool ImplementTorqueControl::setTorqueErrorLimits(const double *limits)
{
    castToMapper(helper)->toHw(limits, temp);
    return iTorqueRaw->setTorqueErrorLimitsRaw(temp);
}

bool ImplementTorqueControl::getTorqueError(int j, double *err)
{
    int k;
    bool ret;
    double temp;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->getTorqueErrorRaw(k, &temp);
    *err=castToMapper(helper)->trqS2N(temp, k);
    return ret;
}

bool ImplementTorqueControl::getTorqueErrors(double *errs)
{
    bool ret;
    ret = iTorqueRaw->getTorqueErrorsRaw(temp);
    castToMapper(helper)->trqS2N(temp, errs);
    return ret;
}
    
bool ImplementTorqueControl::getTorquePidOutput(int j, double *out)
{
    int k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorquePidOutputRaw(k, out);
}

bool ImplementTorqueControl::getTorquePidOutputs(double *outs)
{
    bool ret=iTorqueRaw->getTorquePidOutputsRaw(temp);
    castToMapper(helper)->toUser(temp, outs);
    return ret;
}

bool ImplementTorqueControl::getTorquePid(int j, Pid *pid)
{
  int k=castToMapper(helper)->toHw(j);
  return iTorqueRaw->getTorquePidRaw(k, pid);
}

bool ImplementTorqueControl::getTorquePids(Pid *pids)
{
    bool ret=iTorqueRaw->getTorquePidsRaw(tmpPids);

    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toUser(j);
        pids[tmp]=tmpPids[j];
    }

    return ret;
}

bool ImplementTorqueControl::getTorqueErrorLimits(double *limits)
{
    bool ret=iTorqueRaw->getTorqueErrorLimitsRaw(temp);
    castToMapper(helper)->toUser(temp, limits);
    return ret;
}

bool ImplementTorqueControl::resetTorquePid(int j)
{
    int k=castToMapper(helper)->toHw(j);    
    return iTorqueRaw->resetTorquePidRaw(k);
}

bool ImplementTorqueControl::disableTorquePid(int j)
{
    int k=castToMapper(helper)->toHw(j);    
    return iTorqueRaw->disableTorquePidRaw(k);
}

bool ImplementTorqueControl::enableTorquePid(int j)
{
    int k=castToMapper(helper)->toHw(j);    
    return iTorqueRaw->enableTorquePidRaw(k);
}
bool ImplementTorqueControl::setTorqueOffset(int j, double v)
{
    int k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->setTorqueOffsetRaw(k, v);
}

