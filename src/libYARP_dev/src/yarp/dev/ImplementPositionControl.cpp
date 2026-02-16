/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ImplementPositionControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/os/Log.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;

ImplementPositionControl::ImplementPositionControl(yarp::dev::IPositionControlRaw *y) :
    m_iraw(y),
    m_helper(nullptr)
{;}


ImplementPositionControl::~ImplementPositionControl()
{
    uninitialize();
}

/**
 * Allocate memory for internal data
 * @param size the number of joints
 * @param amap axis map for this device wrapper
 * @param enc encoder conversion factor, from high level to hardware
 * @param zos offset for setting the zero point. Units are relative to high level user interface (degrees)
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementPositionControl::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(m_helper != nullptr);

    m_buffer_doubles.resize(size);
    m_buffer_ints.resize(size);

    return true;
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementPositionControl::uninitialize()
{
    return true;
}

ReturnValue ImplementPositionControl::positionMove(int j, double ang)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;
    castToMapper(m_helper)->posA2E(ang, j, enc, k);
    return m_iraw->positionMoveRaw(k, enc);
}

ReturnValue ImplementPositionControl::positionMove(const int n_joints, const int *joints, const double *refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(refs);
    JOINTSIDCHECK(n_joints,joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
        m_buffer_doubles[idx] = castToMapper(m_helper)->posA2E(refs[idx], joints[idx]);
    }
    ReturnValue ret = m_iraw->positionMoveRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionControl::positionMove(const double *refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(refs);

    castToMapper(m_helper)->posA2E(refs, m_buffer_doubles.data());

    ReturnValue ret = m_iraw->positionMoveRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionControl::relativeMove(int j, double delta)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;
    castToMapper(m_helper)->velA2E(delta, j, enc, k);

    return m_iraw->relativeMoveRaw(k,enc);
}

ReturnValue ImplementPositionControl::relativeMove(const int n_joints, const int *joints, const double *deltas)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(deltas);
    JOINTSIDCHECK(n_joints,joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
        m_buffer_doubles[idx] = castToMapper(m_helper)->velA2E(deltas[idx], joints[idx]);
    }
    ReturnValue ret = m_iraw->relativeMoveRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionControl::relativeMove(const double *deltas)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(deltas);

    castToMapper(m_helper)->velA2E(deltas, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->relativeMoveRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionControl::checkMotionDone(int j, bool *flag)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(flag);

    int k=castToMapper(m_helper)->toHw(j);

    return m_iraw->checkMotionDoneRaw(k,flag);
}

ReturnValue ImplementPositionControl::checkMotionDone(const int n_joints, const int *joints, bool *flags)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(flags);
    JOINTSIDCHECK(n_joints,joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }
    ReturnValue ret = m_iraw->checkMotionDoneRaw(n_joints, m_buffer_ints.data(), flags);

    return ret;
}

ReturnValue ImplementPositionControl::checkMotionDone(bool *flag)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(flag);

    return m_iraw->checkMotionDoneRaw(flag);
}

ReturnValue ImplementPositionControl::setTrajSpeed(int j, double sp)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;
    castToMapper(m_helper)->velA2E_abs(sp, j, enc, k);
    return m_iraw->setTrajSpeedRaw(k, enc);
}

ReturnValue ImplementPositionControl::setTrajSpeeds(const int n_joints, const int *joints, const double *spds)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(spds);
    JOINTSIDCHECK(n_joints,joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        castToMapper(m_helper)->velA2E_abs(spds[idx], joints[idx], m_buffer_doubles[idx], m_buffer_ints[idx]);
    }
    ReturnValue ret = m_iraw->setTrajSpeedsRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionControl::setTrajSpeeds(const double *spds)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(spds);

    castToMapper(m_helper)->velA2E_abs(spds, m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setTrajSpeedsRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionControl::setTrajAcceleration(int j, double acc)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double enc;

    castToMapper(m_helper)->accA2E_abs(acc, j, enc, k);
    return m_iraw->setTrajAccelerationRaw(k, enc);
}

ReturnValue ImplementPositionControl::setTrajAccelerations(const int n_joints, const int *joints, const double *accs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(accs);
    JOINTSIDCHECK(n_joints,joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        castToMapper(m_helper)->accA2E_abs(accs[idx], joints[idx], m_buffer_doubles[idx], m_buffer_ints[idx]);
    }

    ReturnValue ret = m_iraw->setTrajAccelerationsRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionControl::setTrajAccelerations(const double *accs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(accs);

    castToMapper(m_helper)->accA2E_abs(accs, m_buffer_doubles.data());

    ReturnValue ret = m_iraw->setTrajAccelerationsRaw(m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPositionControl::getTrajSpeed(int j, double *ref)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(ref);
    JOINTIDCHECK(j)

    int k;
    double enc;
    k=castToMapper(m_helper)->toHw(j);

    ReturnValue ret = m_iraw->getTrajSpeedRaw(k, &enc);

    *ref=(castToMapper(m_helper)->velE2A_abs(enc, k));

    return ret;
}

ReturnValue ImplementPositionControl::getTrajSpeeds(const int n_joints, const int *joints, double *spds)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(spds);
    JOINTSIDCHECK(n_joints,joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }

    ReturnValue ret = m_iraw->getTrajSpeedsRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    for(int idx=0; idx<n_joints; idx++)
    {
        spds[idx]=castToMapper(m_helper)->velE2A_abs(m_buffer_doubles[idx], m_buffer_ints[idx]);
    }

    return ret;
}

ReturnValue ImplementPositionControl::getTrajSpeeds(double *spds)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(spds);

    ReturnValue ret = m_iraw->getTrajSpeedsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->velE2A_abs(m_buffer_doubles.data(), spds);

    return ret;
}

ReturnValue ImplementPositionControl::getTrajAccelerations(double *accs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(accs);

    ReturnValue ret=m_iraw->getTrajAccelerationsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->accE2A_abs(m_buffer_doubles.data(), accs);

    return ret;
}

ReturnValue ImplementPositionControl::getTrajAccelerations(const int n_joints, const int *joints, double *accs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(accs);
    JOINTSIDCHECK(n_joints,joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }

    ReturnValue ret = m_iraw->getTrajAccelerationsRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    for(int idx=0; idx<n_joints; idx++)
    {
        accs[idx]=castToMapper(m_helper)->accE2A_abs(m_buffer_doubles[idx], m_buffer_ints[idx]);
    }

    return ret;
}

ReturnValue ImplementPositionControl::getTrajAcceleration(int j, double *acc)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(acc);
    JOINTIDCHECK(j)

    int k;
    double enc;
    k=castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getTrajAccelerationRaw(k, &enc);

    *acc=castToMapper(m_helper)->accE2A_abs(enc, k);

    return ret;
}

ReturnValue ImplementPositionControl::stop(int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    k=castToMapper(m_helper)->toHw(j);

    return m_iraw->stopRaw(k);
}

ReturnValue ImplementPositionControl::stop(const int n_joint, const int *joints)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);

    for(int idx=0; idx<n_joint; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }

    ReturnValue ret = m_iraw->stopRaw(n_joint, m_buffer_ints.data());

    return ret;
}

ReturnValue ImplementPositionControl::stop()
{
    std::lock_guard lock(m_imp_mutex);

    return m_iraw->stopRaw();
}

ReturnValue ImplementPositionControl::getAxes(int *axis)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(axis);

    (*axis)=castToMapper(m_helper)->axes();

    return ReturnValue_ok;
}


ReturnValue ImplementPositionControl::getTargetPosition(const int j, double* ref)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(ref);
    JOINTIDCHECK(j)

    int k;
    double enc;
    k=castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getTargetPositionRaw(k, &enc);

    *ref=castToMapper(m_helper)->posE2A(enc, k);

    return ret;
}

ReturnValue ImplementPositionControl::getTargetPositions(double* refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(refs);

    ReturnValue ret=m_iraw->getTargetPositionsRaw(m_buffer_doubles.data());
    castToMapper(m_helper)->posE2A(m_buffer_doubles.data(), refs);

    return ret;
}

ReturnValue ImplementPositionControl::getTargetPositions(const int n_joints, const int* joints, double* refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(joints);
    POINTERCHECK(refs);
    JOINTSIDCHECK(n_joints,joints)

    for(int idx=0; idx<n_joints; idx++)
    {
        m_buffer_ints[idx] = castToMapper(m_helper)->toHw(joints[idx]);
    }
    ReturnValue ret = m_iraw->getTargetPositionsRaw(n_joints, m_buffer_ints.data(), m_buffer_doubles.data());

    for(int idx=0; idx<n_joints; idx++)
    {
        refs[idx]=castToMapper(m_helper)->posE2A(m_buffer_doubles[idx], m_buffer_ints[idx]);
    }

    return ret;
}
/////////////////// End Implement PostionControl
