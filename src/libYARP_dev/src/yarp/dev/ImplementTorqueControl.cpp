/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;

ImplementTorqueControl::ImplementTorqueControl(ITorqueControlRaw *tq):
    iTorqueRaw(tq),
    helper(nullptr),
    intBuffManager(nullptr),
    doubleBuffManager(nullptr)
{;}

ImplementTorqueControl::~ImplementTorqueControl()
{
    uninitialize();
}

bool ImplementTorqueControl::initialize(int size, const int *amap, const double *enc, const double *zos, const double *nw, const double* amps, const double* dutys, const double* bemfs, const double* ktaus)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos, nw, amps, nullptr, dutys,bemfs,ktaus));
    yAssert (helper != nullptr);

    intBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<int> (size);
    yAssert (intBuffManager != nullptr);

    doubleBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<double> (size);
    yAssert (doubleBuffManager != nullptr);

    return true;
}

bool ImplementTorqueControl::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    if(intBuffManager)
    {
        delete intBuffManager;
        intBuffManager=nullptr;
    }

    if(doubleBuffManager)
    {
        delete doubleBuffManager;
        doubleBuffManager=nullptr;
    }

    return true;
}

ReturnValue ImplementTorqueControl::getAxes(int *axes)
{
    return iTorqueRaw->getAxes(axes);
}

ReturnValue ImplementTorqueControl::getRefTorque(int j, double *r)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    ReturnValue ret;
    double torque;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->getRefTorqueRaw(k, &torque);
    *r=castToMapper(helper)->trqS2N(torque, k);
    return ret;
}

ReturnValue ImplementTorqueControl::setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;

    yarp::dev::MotorTorqueParameters params_raw;
    castToMapper(helper)->bemf_user2raw(params.bemf, j, params_raw.bemf, k);
    castToMapper(helper)->ktau_user2raw(params.ktau, j, params_raw.ktau, k);
    params_raw.bemf_scale = params.bemf_scale;
    params_raw.ktau_scale = params.ktau_scale;

    castToMapper(helper)->viscousPos_user2raw(params.viscousPos, j, params_raw.viscousPos, k);
    castToMapper(helper)->viscousNeg_user2raw(params.viscousNeg, j, params_raw.viscousNeg, k);
    castToMapper(helper)->coulombPos_user2raw(params.coulombPos, j, params_raw.coulombPos, k);
    castToMapper(helper)->coulombNeg_user2raw(params.coulombNeg, j, params_raw.coulombNeg, k);
    castToMapper(helper)->velocityThres_user2raw(params.velocityThres, j, params_raw.velocityThres, k);

    return iTorqueRaw->setMotorTorqueParamsRaw(k, params_raw);
}

ReturnValue ImplementTorqueControl::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k=castToMapper(helper)->toHw(j);

    yarp::dev::MotorTorqueParameters params_raw;
    ReturnValue b = iTorqueRaw->getMotorTorqueParamsRaw(k, &params_raw);
    int tmp_j;

    if (b)
    {
        *params = params_raw;
        castToMapper(helper)->bemf_raw2user(params_raw.bemf, k, (*params).bemf, tmp_j);
        castToMapper(helper)->ktau_raw2user(params_raw.ktau, k, (*params).ktau, tmp_j);
        (*params).bemf_scale = params_raw.bemf_scale;
        (*params).ktau_scale = params_raw.ktau_scale;
        castToMapper(helper)->viscousPos_raw2user(params_raw.viscousPos, k, (*params).viscousPos, tmp_j);
        castToMapper(helper)->viscousNeg_raw2user(params_raw.viscousNeg, k, (*params).viscousNeg, tmp_j);
        castToMapper(helper)->coulombPos_raw2user(params_raw.coulombPos, k, (*params).coulombPos, tmp_j);
        castToMapper(helper)->coulombNeg_raw2user(params_raw.coulombNeg, k, (*params).coulombNeg, tmp_j);
        castToMapper(helper)->velocityThres_raw2user(params_raw.velocityThres, k, (*params).velocityThres, tmp_j);
    }
    return b;
}

ReturnValue ImplementTorqueControl::getRefTorques(double *t)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iTorqueRaw->getRefTorquesRaw(buffValues.getData());
    castToMapper(helper)->trqS2N(buffValues.getData(),t);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementTorqueControl::setRefTorques(const double *t)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->trqN2S(t, buffValues.getData());
    ReturnValue ret = iTorqueRaw->setRefTorquesRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementTorqueControl::setRefTorque(int j, double t)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    double sens;
    castToMapper(helper)->trqN2S(t,j,sens,k);
    return iTorqueRaw->setRefTorqueRaw(k, sens);
}

ReturnValue ImplementTorqueControl::getTorques(double *t)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iTorqueRaw->getTorquesRaw(buffValues.getData());
    castToMapper(helper)->toUser(buffValues.getData(), t);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementTorqueControl::setRefTorques(const int n_joints, const int *joints, const double *t)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        buffValues[idx] =  castToMapper(helper)->trqN2S(t[idx], joints[idx]);
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    ReturnValue ret = iTorqueRaw->setRefTorquesRaw(n_joints, buffJoints.getData(), buffValues.getData());

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementTorqueControl::getTorque(int j, double *t)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueRaw(k, t);
}

ReturnValue ImplementTorqueControl::getTorqueRanges(double *min, double *max)
{
    yarp::dev::impl::Buffer<double> buffMin = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffMax = doubleBuffManager->getBuffer();

    ReturnValue ret = iTorqueRaw->getTorqueRangesRaw(buffMin.getData(),buffMax.getData());
    castToMapper(helper)->toUser(buffMin.getData(), min);
    castToMapper(helper)->toUser(buffMax.getData(), max);
    doubleBuffManager->releaseBuffer(buffMin);
    doubleBuffManager->releaseBuffer(buffMax);
    return ret;
}

ReturnValue ImplementTorqueControl::getTorqueRange(int j, double *min, double *max)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueRangeRaw(k, min, max);
}
