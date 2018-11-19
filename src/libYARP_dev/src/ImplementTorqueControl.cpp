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
#define MJOINTIDCHECK_DEL(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete[] temp_int; delete [] temp; return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementTorqueControl::ImplementTorqueControl(ITorqueControlRaw *tq):nj(0)
{
    iTorqueRaw = tq;
    helper=nullptr;
}

ImplementTorqueControl::~ImplementTorqueControl()
{
    uninitialize();
}

bool ImplementTorqueControl::initialize(int size, const int *amap, const double *enc, const double *zos, const double *nw, const double* amps, const double* dutys, const double* bemfs, const double* ktaus)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos, nw, amps, nullptr, dutys,bemfs,ktaus));
    yAssert (helper != nullptr);

    nj=size;
    return true;
}

bool ImplementTorqueControl::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }
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

bool ImplementTorqueControl::setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params)
{
    JOINTIDCHECK
    int k;
    
    yarp::dev::MotorTorqueParameters params_raw;
    castToMapper(helper)->bemf_user2raw(params.bemf, j, params_raw.bemf, k);
    castToMapper(helper)->ktau_user2raw(params.ktau, j, params_raw.ktau, k);
    params_raw.bemf_scale = params.bemf_scale;
    params_raw.ktau_scale = params.ktau_scale;

    return iTorqueRaw->setMotorTorqueParamsRaw(k, params_raw);
}

bool ImplementTorqueControl::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);

    yarp::dev::MotorTorqueParameters params_raw;
    bool b = iTorqueRaw->getMotorTorqueParamsRaw(k, &params_raw);
    int tmp_j;

    if (b)
    {
        *params = params_raw;
        castToMapper(helper)->bemf_raw2user(params_raw.bemf, k, (*params).bemf, tmp_j);
        castToMapper(helper)->ktau_raw2user(params_raw.ktau, k, (*params).ktau, tmp_j);
        (*params).bemf_scale = params_raw.bemf_scale;
        (*params).ktau_scale = params_raw.ktau_scale;
    }
    return b;
}

bool ImplementTorqueControl::getRefTorques(double *t)
{
    bool ret;
    auto* refTorques = new double[nj];
    ret = iTorqueRaw->getRefTorquesRaw(refTorques);
    castToMapper(helper)->trqS2N(refTorques,t);
    delete [] refTorques;
    return ret;
}

bool ImplementTorqueControl::setRefTorques(const double *t)
{
    auto* refTorques = new double[nj];
    castToMapper(helper)->trqN2S(t, refTorques);
    bool ret = iTorqueRaw->setRefTorquesRaw(refTorques);
    delete [] refTorques;
    return ret;
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
    auto* torques = new double[nj];
    bool ret = iTorqueRaw->getTorquesRaw(torques);
    castToMapper(helper)->toUser(torques, t);
    delete [] torques;
    return ret;
}

bool ImplementTorqueControl::setRefTorques(const int n_joint, const int *joints, const double *t)
{
    int *temp_int = new int[nj];
    auto* temp = new double[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL(idx)
        castToMapper(helper)->trqN2S(t[idx], joints[idx], temp[idx], temp_int[idx]);
    }
    bool ret = iTorqueRaw->setRefTorquesRaw(n_joint, temp_int, temp);
    delete [] temp;
    delete [] temp_int;
    return ret;
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
    auto* t_min = new double[nj];
    auto* t_max = new double[nj];
    bool ret = iTorqueRaw->getTorqueRangesRaw(t_min,t_max);
    castToMapper(helper)->toUser(t_min, min);
    castToMapper(helper)->toUser(t_max, max);
    delete [] t_min;
    delete [] t_max;
    return ret;
}

bool ImplementTorqueControl::getTorqueRange(int j, double *min, double *max)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueRangeRaw(k, min, max);
}
