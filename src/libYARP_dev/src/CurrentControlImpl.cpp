/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <stdio.h>
using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementCurrentControl::ImplementCurrentControl(ICurrentControlRaw *tq)
{
    iCurrentRaw = tq;
    helper=0;
    temp=0;
//     fake =0;
    temp2=0;
    temp_int=0;
    tmpPids=0;
}

ImplementCurrentControl::~ImplementCurrentControl()
{
    uninitialize();
}

bool ImplementCurrentControl::initialize(int size, const int *amap, const double* ampsToSens)
{
    if (helper!=0)
        return false;

    helper = (void *)(new ControlBoardHelper(size, amap, 0, 0, 0, ampsToSens, 0, 0));
    yAssert (helper != 0);
    temp=new double [size];
    yAssert (temp != 0);
    temp2=new double [size];
    yAssert (temp2 != 0);
    temp_int=new int [size];
    yAssert (temp_int != 0);
    tmpPids=new Pid[size];
    yAssert (tmpPids!=0);

    return true;
}

bool ImplementCurrentControl::uninitialize()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    checkAndDestroy(temp);
    checkAndDestroy(temp2);
    checkAndDestroy(temp_int);
    checkAndDestroy(tmpPids);

    return true;
}

bool ImplementCurrentControl::getAxes(int *axes)
{
    return iCurrentRaw->getAxes(axes);
}

bool ImplementCurrentControl::getRefCurrent(int j, double *r)
{
    JOINTIDCHECK
    int k;
    bool ret;
    double current;
    k=castToMapper(helper)->toHw(j);
    ret = iCurrentRaw->getRefCurrentRaw(k, &current);
    *r = castToMapper(helper)->ampereS2A(current, k);
    return ret;
}

bool ImplementCurrentControl::getRefCurrents(double *t)
{
    bool ret;
    ret = iCurrentRaw->getRefCurrentsRaw(temp);
    castToMapper(helper)->ampereS2A(temp,t);
    return ret;
}

bool ImplementCurrentControl::setRefCurrents(const double *t)
{
    castToMapper(helper)->ampereA2S(t, temp);
    return iCurrentRaw->setRefCurrentsRaw(temp);
}

bool ImplementCurrentControl::setRefCurrent(int j, double t)
{
    JOINTIDCHECK
    int k;
    double sens;
    castToMapper(helper)->ampereA2S(t,j,sens,k);
    return iCurrentRaw->setRefCurrentRaw(k, sens);
}

bool ImplementCurrentControl::getCurrents(double *t)
{
    bool ret = iCurrentRaw->getCurrentsRaw(temp);
    castToMapper(helper)->ampereS2A(temp, t);
    return ret;
}

bool ImplementCurrentControl::setRefCurrents(const int n_joint, const int *joints, const double *t)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        castToMapper(helper)->ampereA2S(t[idx], joints[idx], temp[idx], temp_int[idx]);
    }
    return iCurrentRaw->setRefCurrentsRaw(n_joint, temp_int, temp);
}

bool ImplementCurrentControl::getCurrent(int j, double *t)
{
    JOINTIDCHECK
    int k;
    bool ret;
    double current;
    k=castToMapper(helper)->toHw(j);
    ret = iCurrentRaw->getCurrentRaw(k, &current);
    *t = castToMapper(helper)->ampereS2A(current, k);
    return ret;
}

bool ImplementCurrentControl::getCurrentRanges(double *min, double *max)
{
    bool ret = iCurrentRaw->getCurrentRangesRaw(temp, temp2);
    castToMapper(helper)->toUser(temp, min);
    castToMapper(helper)->toUser(temp2, max);
    return ret;
}

bool ImplementCurrentControl::getCurrentRange(int j, double *min, double *max)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iCurrentRaw->getCurrentRangeRaw(k, min, max);
}

bool ImplementCurrentControl::setCurrentPid(int j, const Pid &pid)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iCurrentRaw->setCurrentPidRaw(k, pid);
}

bool ImplementCurrentControl::setCurrentPids(const Pid *pids)
{
    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toHw(j);
        tmpPids[tmp]=pids[j];
    }

    return iCurrentRaw->setCurrentPidsRaw(tmpPids);
}

bool ImplementCurrentControl::getCurrentError(int j, double *err)
{
    JOINTIDCHECK
    int k;
    bool ret;
    double temp;
    k=castToMapper(helper)->toHw(j);
    ret = iCurrentRaw->getCurrentErrorRaw(k, &temp);
    *err=castToMapper(helper)->ampereS2A(temp, k);
    return ret;
}

bool ImplementCurrentControl::getCurrentErrors(double *errs)
{
    bool ret;
    ret = iCurrentRaw->getCurrentErrorsRaw(temp);
    castToMapper(helper)->ampereS2A(temp, errs);
    return ret;
}

bool ImplementCurrentControl::getCurrentPidOutput(int j, double *out)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return iCurrentRaw->getCurrentPidOutputRaw(k, out);
}

bool ImplementCurrentControl::getCurrentPidOutputs(double *outs)
{
    bool ret = iCurrentRaw->getCurrentPidOutputsRaw(temp);
    castToMapper(helper)->toUser(temp, outs);
    return ret;
}

bool ImplementCurrentControl::getCurrentPid(int j, Pid *pid)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return iCurrentRaw->getCurrentPidRaw(k, pid);
}

bool ImplementCurrentControl::getCurrentPids(Pid *pids)
{
    bool ret = iCurrentRaw->getCurrentPidsRaw(tmpPids);

    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toUser(j);
        pids[tmp]=tmpPids[j];
    }

    return ret;
}

bool ImplementCurrentControl::resetCurrentPid(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return iCurrentRaw->resetCurrentPidRaw(k);
}

bool ImplementCurrentControl::disableCurrentPid(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return iCurrentRaw->disableCurrentPidRaw(k);
}

bool ImplementCurrentControl::enableCurrentPid(int j)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);
    return iCurrentRaw->enableCurrentPidRaw(k);
}
