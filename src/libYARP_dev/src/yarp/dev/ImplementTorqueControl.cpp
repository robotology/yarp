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
    m_iraw(tq),
    m_helper(nullptr)
{;}

ImplementTorqueControl::~ImplementTorqueControl()
{
    uninitialize();
}

bool ImplementTorqueControl::initialize(int size, const int *amap, const double *enc, const double *zos, const double *nw, const double* amps, const double* dutys, const double* bemfs, const double* ktaus)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos, nw, amps, nullptr, dutys,bemfs,ktaus));
    yAssert (m_helper != nullptr);

    m_buffer_doubles.resize(size);
    m_buffer_ints.resize(size);

    return true;
}

bool ImplementTorqueControl::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementTorqueControl::getAxes(int *axes)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(axes);

    return m_iraw->getAxes(axes);
}

ReturnValue ImplementTorqueControl::getRefTorque(int j, double *r)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(r);

    int k;
    ReturnValue ret;
    double torque;
    k=castToMapper(m_helper)->toHw(j);
    ret = m_iraw->getRefTorqueRaw(k, &torque);
    *r=castToMapper(m_helper)->trqS2N(torque, k);
    return ret;
}

ReturnValue ImplementTorqueControl::setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;

    yarp::dev::MotorTorqueParameters params_raw;
    castToMapper(m_helper)->bemf_user2raw(params.bemf, j, params_raw.bemf, k);
    castToMapper(m_helper)->ktau_user2raw(params.ktau, j, params_raw.ktau, k);
    params_raw.bemf_scale = params.bemf_scale;
    params_raw.ktau_scale = params.ktau_scale;

    castToMapper(m_helper)->viscousPos_user2raw(params.viscousPos, j, params_raw.viscousPos, k);
    castToMapper(m_helper)->viscousNeg_user2raw(params.viscousNeg, j, params_raw.viscousNeg, k);
    castToMapper(m_helper)->coulombPos_user2raw(params.coulombPos, j, params_raw.coulombPos, k);
    castToMapper(m_helper)->coulombNeg_user2raw(params.coulombNeg, j, params_raw.coulombNeg, k);
    castToMapper(m_helper)->velocityThres_user2raw(params.velocityThres, j, params_raw.velocityThres, k);

    return m_iraw->setMotorTorqueParamsRaw(k, params_raw);
}

ReturnValue ImplementTorqueControl::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(params);

    int k=castToMapper(m_helper)->toHw(j);

    yarp::dev::MotorTorqueParameters params_raw;
    ReturnValue b = m_iraw->getMotorTorqueParamsRaw(k, &params_raw);
    int tmp_j;

    if (b)
    {
        *params = params_raw;
        castToMapper(m_helper)->bemf_raw2user(params_raw.bemf, k, (*params).bemf, tmp_j);
        castToMapper(m_helper)->ktau_raw2user(params_raw.ktau, k, (*params).ktau, tmp_j);
        (*params).bemf_scale = params_raw.bemf_scale;
        (*params).ktau_scale = params_raw.ktau_scale;
        castToMapper(m_helper)->viscousPos_raw2user(params_raw.viscousPos, k, (*params).viscousPos, tmp_j);
        castToMapper(m_helper)->viscousNeg_raw2user(params_raw.viscousNeg, k, (*params).viscousNeg, tmp_j);
        castToMapper(m_helper)->coulombPos_raw2user(params_raw.coulombPos, k, (*params).coulombPos, tmp_j);
        castToMapper(m_helper)->coulombNeg_raw2user(params_raw.coulombNeg, k, (*params).coulombNeg, tmp_j);
        castToMapper(m_helper)->velocityThres_raw2user(params_raw.velocityThres, k, (*params).velocityThres, tmp_j);
    }
    return b;
}

ReturnValue ImplementTorqueControl::getRefTorques(double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(t);

    ReturnValue ret = m_iraw->getRefTorquesRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->trqS2N(m_buffer_doubles.data(),t);

    return ret;
}

ReturnValue ImplementTorqueControl::setRefTorques(const double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(t);

    castToMapper(m_helper)->trqN2S(t, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setRefTorquesRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementTorqueControl::setRefTorque(int j, double t)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double sens;
    castToMapper(m_helper)->trqN2S(t,j,sens,k);
    return m_iraw->setRefTorqueRaw(k, sens);
}

ReturnValue ImplementTorqueControl::getTorques(double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(t);

    ReturnValue ret = m_iraw->getTorquesRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->toUser(m_buffer_doubles.data(), t);

    return ret;
}

ReturnValue ImplementTorqueControl::setRefTorques(const int n_joints, const int *joints, const double *t)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(t);
    JOINTSIDCHECK(n_joints,joints);

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_doubles[idx] =  castToMapper(m_helper)->trqN2S(t[idx], joints[idx]);
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }
    ReturnValue ret = m_iraw->setRefTorquesRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementTorqueControl::getTorque(int j, double *t)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(t);

    int k;
    k=castToMapper(m_helper)->toHw(j);
    return m_iraw->getTorqueRaw(k, t);
}

ReturnValue ImplementTorqueControl::getTorqueRanges(double *min, double *max)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(min);
    POINTERCHECK(max);

    ReturnValue ret = m_iraw->getTorqueRangesRaw(m_buffer_doubles.data(),m_buffer_doubles2.data());
    castToMapper(m_helper)->toUser(m_buffer_doubles.data(), min);
    castToMapper(m_helper)->toUser(m_buffer_doubles2.data(), max);

    return ret;
}

ReturnValue ImplementTorqueControl::getTorqueRange(int j, double *min, double *max)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(min);
    POINTERCHECK(max);

    int k;
    k=castToMapper(m_helper)->toHw(j);
    return m_iraw->getTorqueRangeRaw(k, min, max);
}
