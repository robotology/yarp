/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <stdio.h>
using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementCurrentControl::ImplementCurrentControl(ICurrentControlRaw *tq):nj(0)
{
    iCurrentRaw = tq;
    helper=nullptr;
}

ImplementCurrentControl::~ImplementCurrentControl()
{
    uninitialize();
}

bool ImplementCurrentControl::initialize(int size, const int *amap, const double* ampsToSens)
{
    if (helper!=nullptr)
        return false;
    nj=size;

    helper = (void *)(new ControlBoardHelper(size, amap, nullptr, nullptr, nullptr, ampsToSens, nullptr, nullptr));
    yAssert (helper != nullptr);

    return true;
}

bool ImplementCurrentControl::uninitialize()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }
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
    double *tmp = new double[nj];
    ret = iCurrentRaw->getRefCurrentsRaw(tmp);
    castToMapper(helper)->ampereS2A(tmp,t);
    delete [] tmp;
    return ret;
}

bool ImplementCurrentControl::setRefCurrents(const double *t)
{
    double *tmp = new double[nj];
    castToMapper(helper)->ampereA2S(t, tmp);
    bool ret = iCurrentRaw->setRefCurrentsRaw(tmp);
    delete [] tmp;
    return ret;
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
    double *tmp = new double[nj];
    bool ret = iCurrentRaw->getCurrentsRaw(tmp);
    castToMapper(helper)->ampereS2A(tmp, t);
    delete [] tmp;
    return ret;
}

bool ImplementCurrentControl::setRefCurrents(const int n_joint, const int *joints, const double *t)
{
    double *tmp = new double[nj];
    int *tmp_joints = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        castToMapper(helper)->ampereA2S(t[idx], joints[idx], tmp[idx], tmp_joints[idx]);
    }
    bool ret = iCurrentRaw->setRefCurrentsRaw(n_joint, tmp_joints, tmp);
    delete [] tmp;
    delete [] tmp_joints;
    return ret;
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
    double *tmp_min = new double[nj];
    double *tmp_max = new double[nj];
    bool ret = iCurrentRaw->getCurrentRangesRaw(tmp_min, tmp_max);
    castToMapper(helper)->toUser(tmp_min, min);
    castToMapper(helper)->toUser(tmp_max, max);
    delete [] tmp_min;
    delete [] tmp_max;
    return ret;
}

bool ImplementCurrentControl::getCurrentRange(int j, double *min, double *max)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iCurrentRaw->getCurrentRangeRaw(k, min, max);
}
