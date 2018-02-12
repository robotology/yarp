/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
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
    helper=nullptr;
    temp=nullptr;
//     fake =0;
    temp2=nullptr;
    temp_int=nullptr;
    tmpPids=nullptr;
}

ImplementCurrentControl::~ImplementCurrentControl()
{
    uninitialize();
}

bool ImplementCurrentControl::initialize(int size, const int *amap, const double* ampsToSens)
{
    if (helper!=nullptr)
        return false;

    helper = (void *)(new ControlBoardHelper(size, amap, nullptr, 0, 0, ampsToSens, nullptr, 0));
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

bool ImplementCurrentControl::uninitialize()
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

bool ImplementCurrentControl::getNumberOfMotors(int *axes)
{
    return iCurrentRaw->getNumberOfMotorsRaw(axes);
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
