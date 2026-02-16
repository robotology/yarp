/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ImplementVelocityControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/os/Log.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;

ImplementVelocityControl::ImplementVelocityControl(IVelocityControlRaw *y) :
    m_iraw(y),
    m_helper(nullptr)
{;}

ImplementVelocityControl::~ImplementVelocityControl()
{
    uninitialize();
}

bool ImplementVelocityControl::initialize(int size, const int *axis_map, const double *enc, const double *zeros)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, axis_map, enc, zeros));
    yAssert (m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}

bool ImplementVelocityControl::uninitialize()
{
    if(m_helper != nullptr)
    {
        delete castToMapper(m_helper);
        m_helper = nullptr;
    }

    return true;
}

ReturnValue ImplementVelocityControl::getAxes(int *ax)
{
    (*ax)=castToMapper(m_helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementVelocityControl::velocityMove(int j, double sp)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;
    castToMapper(m_helper)->velA2E(sp, j, enc, k);
    return m_iraw->velocityMoveRaw(k, enc);
}

ReturnValue ImplementVelocityControl::velocityMove(const int n_joints, const int *joints, const double *spds)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(spds);
    JOINTSIDCHECK(n_joints, joints)

    std::vector<int> vectorInt_tmp(n_joints);
    std::vector<double> vectorDouble_tmp(n_joints);

    for(int idx=0; idx<n_joints; idx++)
    {
        vectorInt_tmp[idx] = castToMapper(m_helper)->toHw(joints[idx]);
        vectorDouble_tmp[idx] = castToMapper(m_helper)->velA2E(spds[idx], joints[idx]);
    }
    ReturnValue ret = m_iraw->velocityMoveRaw(n_joints, vectorInt_tmp.data(), vectorDouble_tmp.data());

    return ret;
}

ReturnValue ImplementVelocityControl::velocityMove(const double *sp)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(sp);

    castToMapper(m_helper)->velA2E(sp, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->velocityMoveRaw(m_buffer_doubles.data());
    return ret;
}

ReturnValue ImplementVelocityControl::getTargetVelocity(const int j, double* vel)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(vel);

    int k;
    double tmp;
    k=castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getTargetVelocityRaw(k, &tmp);
    *vel=castToMapper(m_helper)->velE2A(tmp, k);
    return ret;
}

ReturnValue ImplementVelocityControl::getTargetVelocities(double *vels)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(vels);

    ReturnValue ret=m_iraw->getTargetVelocitiesRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->velE2A(m_buffer_doubles.data(), vels);
    return ret;
}

ReturnValue ImplementVelocityControl::getTargetVelocities(const int n_joints, const int *joints, double *vels)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(vels);
    JOINTSIDCHECK(n_joints,joints)

    std::vector<int> vectorInt_tmp(n_joints);
    std::vector<double> vectorDouble_tmp(n_joints);

    for(int idx=0; idx<n_joints; idx++)
    {
        vectorInt_tmp[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }

    ReturnValue ret = m_iraw->getTargetVelocitiesRaw(n_joints, vectorInt_tmp.data(), vectorDouble_tmp.data());

    for(int idx=0; idx<n_joints; idx++)
    {
        vels[idx]=castToMapper(m_helper)->velE2A(vectorDouble_tmp[idx], vectorInt_tmp[idx]);
    }

    return ret;
}

ReturnValue ImplementVelocityControl::setTrajAcceleration(int j, double acc)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;
    castToMapper(m_helper)->accA2E_abs(acc, j, enc, k);
    return m_iraw->setTrajAccelerationRaw(k, enc);
}

ReturnValue ImplementVelocityControl::setTrajAccelerations(const int n_joints, const int *joints, const double *accs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(accs);
    JOINTSIDCHECK(n_joints,joints)

    std::vector<int> vectorInt_tmp(n_joints);
    std::vector<double> vectorDouble_tmp(n_joints);

    for(int idx=0; idx<n_joints; idx++)
    {
        castToMapper(m_helper)->accA2E_abs(accs[idx], joints[idx], vectorDouble_tmp[idx], vectorInt_tmp[idx]);
    }
    ReturnValue ret = m_iraw->setTrajAccelerationsRaw(n_joints, vectorInt_tmp.data(), vectorDouble_tmp.data());

    return ret;
}

ReturnValue ImplementVelocityControl::setTrajAccelerations(const double *accs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(accs);

    castToMapper(m_helper)->accA2E_abs(accs, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setTrajAccelerationsRaw(m_buffer_doubles.data());
    return ret;
}

ReturnValue ImplementVelocityControl::getTrajAcceleration(int j, double *acc)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(acc);

    int k;
    double enc;
    k=castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getTrajAccelerationRaw(k, &enc);
    *acc=castToMapper(m_helper)->accE2A_abs(enc, k);
    return ret;
}

ReturnValue ImplementVelocityControl::getTrajAccelerations(const int n_joints, const int *joints, double *accs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(accs);
    JOINTSIDCHECK(n_joints,joints)

    std::vector<int> vectorInt_tmp(n_joints);
    std::vector<double> vectorDouble_tmp(n_joints);

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx]=castToMapper(m_helper)->toHw(joints[idx]);
    }

    ReturnValue ret = m_iraw->getTrajAccelerationsRaw(n_joints, vectorInt_tmp.data(), vectorDouble_tmp.data());

    for(int idx=0; idx<n_joints; idx++)
    {
        accs[idx]=castToMapper(m_helper)->accE2A_abs(vectorDouble_tmp[idx], vectorInt_tmp[idx]);
    }

    return ret;
}


ReturnValue ImplementVelocityControl::getTrajAccelerations(double *accs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(accs);

    ReturnValue ret=m_iraw->getTrajAccelerationsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->accE2A_abs(m_buffer_doubles.data(), accs);
    return ret;
}


ReturnValue ImplementVelocityControl::stop(int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    k=castToMapper(m_helper)->toHw(j);
    return m_iraw->stopRaw(k);
}


ReturnValue ImplementVelocityControl::stop(const int n_joints, const int *joints)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTSIDCHECK(n_joints,joints)
    POINTERCHECK(joints);

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }
    ReturnValue ret = m_iraw->stopRaw(n_joints, m_buffer_ints.data());
    return ret;
}


ReturnValue ImplementVelocityControl::stop()
{
    return m_iraw->stopRaw();
}
