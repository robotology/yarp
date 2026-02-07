/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include "FakeLaserWithMotor.h"
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/math/Vec2D.h>
#include <iostream>
#include <limits>
#include <cstring>
#include <cstdlib>
#include <cmath>

//#define LASER_DEBUG
#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

YARP_LOG_COMPONENT(FAKE_LASER_MOTORS, "yarp.devices.fakeLaserWithMotor.motors")

using namespace yarp::os;
using namespace yarp::dev;

template <class T,
    std::enable_if_t<std::is_trivial<T>::value, int> = 0>
    T* allocAndCheck(int size)
{
    T* t = new T[size];
    yAssert(t != 0);
    memset(t, 0, sizeof(T) * size);
    return t;
}

template <class T,
    std::enable_if_t<!std::is_trivial<T>::value, int> = 0>
    T* allocAndCheck(int size)
{
    T* t = new T[size];
    yAssert(t != 0);
    return t;
}

template <class T>
void checkAndDestroy(T*& p) {
    if (p != 0) {
        delete[] p;
        p = 0;
    }
}


//Motor interfaces
ReturnValue FakeLaserWithMotor::getEncodersTimedRaw(double* encs, double* stamps)
{
    ReturnValue ret = getEncodersRaw(encs);
    m_mutex.lock();
    for (int i = 0; i < m_njoints; i++) {
        stamps[i] = m_timestamp.getTime();
    }
    m_mutex.unlock();
    return ret;
}

ReturnValue FakeLaserWithMotor::getEncoderTimedRaw(int j, double* encs, double* stamp)
{
    ReturnValue ret = getEncoderRaw(j, encs);
    m_mutex.lock();
    *stamp = m_timestamp.getTime();
    m_mutex.unlock();

    return ret;
}

ReturnValue FakeLaserWithMotor::getAxes(int* ax)
{
    *ax = m_njoints;
    return ReturnValue_ok;
}

yarp::dev::ReturnValue FakeLaserWithMotor::positionMoveRaw(int j, double ref)
{
    int mode = 0;
    getControlModeRaw(j, &mode);
    if ((mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS) &&
        (mode != VOCAB_CM_IDLE))
    {
        yCError(FAKE_LASER_MOTORS) << "positionMoveRaw: skipping command because joint " << j << " is not in VOCAB_CM_POSITION mode";
    }
    _posCtrl_references[j] = ref;
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::positionMoveRaw(const double* refs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        ret &= positionMoveRaw(j, refs[index]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::relativeMoveRaw(int j, double delta)
{
    int mode = 0;
    getControlModeRaw(j, &mode);
    if ((mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS) &&
        (mode != VOCAB_CM_IDLE))
    {
        yCError(FAKE_LASER_MOTORS) << "relativeMoveRaw: skipping command because joint " << j << " is not in VOCAB_CM_POSITION mode";
    }
    _posCtrl_references[j] += delta;
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::relativeMoveRaw(const double* deltas)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        ret &= relativeMoveRaw(j, deltas[index]);
    }
    return ret;
}


ReturnValue FakeLaserWithMotor::checkMotionDoneRaw(int j, bool* flag)
{
    *flag = false;
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::checkMotionDoneRaw(bool* flag)
{
    ReturnValue ret = ReturnValue_ok;
    bool val, tot_res = true;

    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        ret &= checkMotionDoneRaw(j, &val);
        tot_res &= val;
    }
    *flag = tot_res;
    return ret;
}

ReturnValue FakeLaserWithMotor::setTrajSpeedRaw(int j, double sp)
{
    // Velocity is expressed in iDegrees/s
    // save internally the new value of speed; it'll be used in the positionMove
    int index = j;
    _ref_speeds[index] = sp;
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::setTrajSpeedsRaw(const double* spds)
{
    // Velocity is expressed in iDegrees/s
    // save internally the new value of speed; it'll be used in the positionMove
    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        _ref_speeds[index] = spds[index];
    }
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::setTrajAccelerationRaw(int j, double acc)
{
    // Acceleration is expressed in iDegrees/s^2
    // save internally the new value of the acceleration; it'll be used in the velocityMove command

    if (acc > 1e6)
    {
        _ref_accs[j] = 1e6;
    }
    else if (acc < -1e6)
    {
        _ref_accs[j] = -1e6;
    }
    else
    {
        _ref_accs[j] = acc;
    }

    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::setTrajAccelerationsRaw(const double* accs)
{
    // Acceleration is expressed in iDegrees/s^2
    // save internally the new value of the acceleration; it'll be used in the velocityMove command
    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        if (accs[j] > 1e6)
        {
            _ref_accs[index] = 1e6;
        }
        else if (accs[j] < -1e6)
        {
            _ref_accs[index] = -1e6;
        }
        else
        {
            _ref_accs[index] = accs[j];
        }
    }
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getTrajSpeedRaw(int j, double* spd)
{
    *spd = _ref_speeds[j];
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getTrajSpeedsRaw(double* spds)
{
    memcpy(spds, _ref_speeds, sizeof(double) * m_njoints);
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getTrajAccelerationRaw(int j, double* acc)
{
    *acc = _ref_accs[j];
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getTrajAccelerationsRaw(double* accs)
{
    memcpy(accs, _ref_accs, sizeof(double) * m_njoints);
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::stopRaw(int j)
{
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::stopRaw()
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < m_njoints; j++)
    {
        ret &= stopRaw(j);
    }
    return ret;
}
///////////// END Position Control INTERFACE  //////////////////

ReturnValue FakeLaserWithMotor::getAvailableControlModesRaw(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail)
{
    avail = _availableControlModes[j];
    return ReturnValue_ok;
}


ReturnValue FakeLaserWithMotor::getControlModeRaw(int j, int* v)
{
    *v = _controlModes[j];
    return ReturnValue_ok;
}

// IControl Mode 2
ReturnValue FakeLaserWithMotor::getControlModesRaw(int* v)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < m_njoints; j++)
    {
        ret = ret && getControlModeRaw(j, &v[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getControlModesRaw(const int n_joint, const int* joints, int* modes)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && getControlModeRaw(joints[j], &modes[j]);
    }
    return ret;
}



ReturnValue FakeLaserWithMotor::setControlModeRaw(const int j, const int _mode)
{
    if (_mode == VOCAB_CM_FORCE_IDLE)
    {
        _controlModes[j] = VOCAB_CM_IDLE;
    }
    else
    {
        _controlModes[j] = _mode;
    }
    return ReturnValue_ok;
}


ReturnValue FakeLaserWithMotor::setControlModesRaw(const int n_joint, const int* joints, int* modes)
{
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < n_joint; i++)
    {
        ret &= setControlModeRaw(joints[i], modes[i]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::setControlModesRaw(int* modes)
{
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < m_njoints; i++)
    {
        ret &= setControlModeRaw(i, modes[i]);
    }
    return ret;
}

bool FakeLaserWithMotor::alloc(int nj)
{
    _controlModes = allocAndCheck<int>(nj);
    _encoders = allocAndCheck<double>(nj);
    _axisName = new std::string[nj];
    _jointType = new JointTypeEnum[nj];

    // Reserve space for data stored locally. values are initialized to 0
    _posCtrl_references = allocAndCheck<double>(nj);
    _command_speeds = allocAndCheck<double>(nj);
    _ref_speeds = allocAndCheck<double>(nj);
    _ref_accs = allocAndCheck<double>(nj);
    _availableControlModes = allocAndCheck<std::vector<yarp::dev::SelectableControlModeEnum>>(nj);

    for (int i = 0; i < nj; ++i)
    {
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_IDLE);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_IDLE);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_TORQUE);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_POSITION);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_POSITION_DIRECT);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_VELOCITY);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_VELOCITY_DIRECT);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_CURRENT);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_PWM);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_MIXED);
        _availableControlModes[i].push_back(yarp::dev::SelectableControlModeEnum::VOCAB_CM_FORCE_IDLE);
    }

    //resizeBuffers();

    return true;
}

bool FakeLaserWithMotor::dealloc()
{
    checkAndDestroy(_axisName);
    checkAndDestroy(_jointType);
    checkAndDestroy(_controlModes);
    checkAndDestroy(_encoders);
    checkAndDestroy(_posCtrl_references);
    checkAndDestroy(_ref_speeds);
    checkAndDestroy(_command_speeds);
    checkAndDestroy(_ref_accs);
    checkAndDestroy(_availableControlModes);
    return true;
}


ReturnValue FakeLaserWithMotor::setEncoderRaw(int j, double val)
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue FakeLaserWithMotor::setEncodersRaw(const double* vals)
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue FakeLaserWithMotor::resetEncoderRaw(int j)
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue FakeLaserWithMotor::resetEncodersRaw()
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue FakeLaserWithMotor::getEncoderRaw(int j, double* value)
{
    ReturnValue ret = ReturnValue_ok;

    *value = _encoders[j];

    return ret;
}

ReturnValue FakeLaserWithMotor::getEncodersRaw(double* encs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < m_njoints; j++)
    {
        ReturnValue ok = getEncoderRaw(j, &encs[j]);
        ret = ret && ok;

    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getEncoderSpeedRaw(int j, double* sp)
{
    // To avoid returning uninitialized memory, we set the encoder speed to 0
    *sp = 0.0;
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getEncoderSpeedsRaw(double* spds)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < m_njoints; j++)
    {
        ret &= getEncoderSpeedRaw(j, &spds[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getEncoderAccelerationRaw(int j, double* acc)
{
    // To avoid returning uninitialized memory, we set the encoder acc to 0
    *acc = 0.0;

    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getEncoderAccelerationsRaw(double* accs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < m_njoints; j++)
    {
        ret &= getEncoderAccelerationRaw(j, &accs[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::setTrajAccelerationsRaw(const int n_joint, const int* joints, const double* accs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && setTrajAccelerationRaw(joints[j], accs[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getTrajSpeedsRaw(const int n_joint, const int* joints, double* spds)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && getTrajSpeedRaw(joints[j], &spds[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getTrajAccelerationsRaw(const int n_joint, const int* joints, double* accs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && getTrajAccelerationRaw(joints[j], &accs[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::stopRaw(const int n_joint, const int* joints)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && stopRaw(joints[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::positionMoveRaw(const int n_joint, const int* joints, const double* refs)
{
    for (int j = 0; j < n_joint; j++)
    {
        yCDebug(FAKE_LASER_MOTORS, "j: %d; ref %f;\n", joints[j], refs[j]); fflush(stdout);
    }

    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && positionMoveRaw(joints[j], refs[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::relativeMoveRaw(const int n_joint, const int* joints, const double* deltas)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && relativeMoveRaw(joints[j], deltas[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::checkMotionDoneRaw(const int n_joint, const int* joints, bool* flag)
{
    ReturnValue ret = ReturnValue_ok;
    bool val = true;
    bool tot_val = true;

    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && checkMotionDoneRaw(joints[j], &val);
        tot_val &= val;
    }
    *flag = tot_val;
    return ret;
}

ReturnValue FakeLaserWithMotor::setTrajSpeedsRaw(const int n_joint, const int* joints, const double* spds)
{
    ReturnValue ret = ReturnValue_ok;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && setTrajSpeedRaw(joints[j], spds[j]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getTargetPositionRaw(int axis, double* ref)
{
    int mode = 0;
    getControlModeRaw(axis, &mode);
    if ((mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS))
    {
        yCWarning(FAKE_LASER_MOTORS) << "getTargetPosition: Joint " << axis << " is not in POSITION mode, therefore the value returned by " <<
            "this call is for reference only and may not reflect the actual behaviour of the motor/firmware.";
    }
    *ref = _posCtrl_references[axis];
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getTargetPositionsRaw(double* refs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < m_njoints; i++) {
        ret &= getTargetPositionRaw(i, &refs[i]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getTargetPositionsRaw(int nj, const int* jnts, double* refs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < nj; i++)
    {
        ret &= getTargetPositionRaw(jnts[i], &refs[i]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::velocityMoveRaw(int j, double sp)
{
    int mode = 0;
    getControlModeRaw(j, &mode);
    if ((mode != VOCAB_CM_VELOCITY) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_VEL) &&
        (mode != VOCAB_CM_IDLE))
    {
        yCError(FAKE_LASER_MOTORS) << "velocityMoveRaw: skipping command because board " << " joint " << j << " is not in VOCAB_CM_VELOCITY mode";
    }
    _command_speeds[j] = sp;
    //last_velocity_command[j] = yarp::os::Time::now();
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::velocityMoveRaw(const double* sp)
{
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < m_njoints; i++) {
        ret &= velocityMoveRaw(i, sp[i]);
    }
    return ret;
}
ReturnValue FakeLaserWithMotor::velocityMoveRaw(const int n_joint, const int* joints, const double* spds)
{
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < n_joint; i++)
    {
        ret &= velocityMoveRaw(joints[i], spds[i]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getTargetVelocityRaw(int axis, double* ref)
{
    *ref = _command_speeds[axis];
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getTargetVelocitiesRaw(double* refs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < m_njoints; i++)
    {
        ret &= getTargetVelocityRaw(i, &refs[i]);
    }
    return ret;
}

ReturnValue FakeLaserWithMotor::getTargetVelocitiesRaw(int nj, const int* jnts, double* refs)
{
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < nj; i++)
    {
        ret &= getTargetVelocityRaw(jnts[i], &refs[i]);
    }
    return ret;
}


ReturnValue FakeLaserWithMotor::getAxisNameRaw(int j, std::string& name)
{
    JOINTIDCHECK(m_njoints);
    name = _axisName[j];
    return ReturnValue_ok;
}

ReturnValue FakeLaserWithMotor::getJointTypeRaw(int j, yarp::dev::JointTypeEnum& type)
{
    JOINTIDCHECK(m_njoints);
    type = _jointType[j];
    return ReturnValue_ok;
}
