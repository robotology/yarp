/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include "fakeLaserWithMotor.h"

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
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
bool FakeLaserWithMotor::getEncodersTimedRaw(double* encs, double* stamps)
{
    bool ret = getEncodersRaw(encs);
    m_mutex.lock();
    for (int i = 0; i < m_njoints; i++) {
        stamps[i] = m_timestamp.getTime();
    }
    m_mutex.unlock();
    return ret;
}

bool FakeLaserWithMotor::getEncoderTimedRaw(int j, double* encs, double* stamp)
{
    bool ret = getEncoderRaw(j, encs);
    m_mutex.lock();
    *stamp = m_timestamp.getTime();
    m_mutex.unlock();

    return ret;
}

bool FakeLaserWithMotor::getAxes(int* ax)
{
    *ax = m_njoints;
    return true;
}

bool FakeLaserWithMotor::positionMoveRaw(int j, double ref)
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
    return true;
}

bool FakeLaserWithMotor::positionMoveRaw(const double* refs)
{
    bool ret = true;
    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        ret &= positionMoveRaw(j, refs[index]);
    }
    return ret;
}

bool FakeLaserWithMotor::relativeMoveRaw(int j, double delta)
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
    return true;
}

bool FakeLaserWithMotor::relativeMoveRaw(const double* deltas)
{
    bool ret = true;
    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        ret &= relativeMoveRaw(j, deltas[index]);
    }
    return ret;
}


bool FakeLaserWithMotor::checkMotionDoneRaw(int j, bool* flag)
{
    *flag = false;
    return true;
}

bool FakeLaserWithMotor::checkMotionDoneRaw(bool* flag)
{
    bool ret = true;
    bool val, tot_res = true;

    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        ret &= checkMotionDoneRaw(j, &val);
        tot_res &= val;
    }
    *flag = tot_res;
    return ret;
}

bool FakeLaserWithMotor::setRefSpeedRaw(int j, double sp)
{
    // Velocity is expressed in iDegrees/s
    // save internally the new value of speed; it'll be used in the positionMove
    int index = j;
    _ref_speeds[index] = sp;
    return true;
}

bool FakeLaserWithMotor::setRefSpeedsRaw(const double* spds)
{
    // Velocity is expressed in iDegrees/s
    // save internally the new value of speed; it'll be used in the positionMove
    for (int j = 0, index = 0; j < m_njoints; j++, index++)
    {
        _ref_speeds[index] = spds[index];
    }
    return true;
}

bool FakeLaserWithMotor::setRefAccelerationRaw(int j, double acc)
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

    return true;
}

bool FakeLaserWithMotor::setRefAccelerationsRaw(const double* accs)
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
    return true;
}

bool FakeLaserWithMotor::getRefSpeedRaw(int j, double* spd)
{
    *spd = _ref_speeds[j];
    return true;
}

bool FakeLaserWithMotor::getRefSpeedsRaw(double* spds)
{
    memcpy(spds, _ref_speeds, sizeof(double) * m_njoints);
    return true;
}

bool FakeLaserWithMotor::getRefAccelerationRaw(int j, double* acc)
{
    *acc = _ref_accs[j];
    return true;
}

bool FakeLaserWithMotor::getRefAccelerationsRaw(double* accs)
{
    memcpy(accs, _ref_accs, sizeof(double) * m_njoints);
    return true;
}

bool FakeLaserWithMotor::stopRaw(int j)
{
    return true;
}

bool FakeLaserWithMotor::stopRaw()
{
    bool ret = true;
    for (int j = 0; j < m_njoints; j++)
    {
        ret &= stopRaw(j);
    }
    return ret;
}
///////////// END Position Control INTERFACE  //////////////////

bool FakeLaserWithMotor::getControlModeRaw(int j, int* v)
{
    *v = _controlModes[j];
    return true;
}

// IControl Mode 2
bool FakeLaserWithMotor::getControlModesRaw(int* v)
{
    bool ret = true;
    for (int j = 0; j < m_njoints; j++)
    {
        ret = ret && getControlModeRaw(j, &v[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::getControlModesRaw(const int n_joint, const int* joints, int* modes)
{
    bool ret = true;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && getControlModeRaw(joints[j], &modes[j]);
    }
    return ret;
}



bool FakeLaserWithMotor::setControlModeRaw(const int j, const int _mode)
{
    if (_mode == VOCAB_CM_FORCE_IDLE)
    {
        _controlModes[j] = VOCAB_CM_IDLE;
    }
    else
    {
        _controlModes[j] = _mode;
    }
    return true;
}


bool FakeLaserWithMotor::setControlModesRaw(const int n_joint, const int* joints, int* modes)
{
    bool ret = true;
    for (int i = 0; i < n_joint; i++)
    {
        ret &= setControlModeRaw(joints[i], modes[i]);
    }
    return ret;
}

bool FakeLaserWithMotor::setControlModesRaw(int* modes)
{
    bool ret = true;
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
    return true;
}


bool FakeLaserWithMotor::setEncoderRaw(int j, double val)
{
    return NOT_YET_IMPLEMENTED("setEncoder");
}

bool FakeLaserWithMotor::setEncodersRaw(const double* vals)
{
    return NOT_YET_IMPLEMENTED("setEncoders");
}

bool FakeLaserWithMotor::resetEncoderRaw(int j)
{
    return NOT_YET_IMPLEMENTED("resetEncoder");
}

bool FakeLaserWithMotor::resetEncodersRaw()
{
    return NOT_YET_IMPLEMENTED("resetEncoders");
}

bool FakeLaserWithMotor::getEncoderRaw(int j, double* value)
{
    bool ret = true;

    *value = _encoders[j];

    return ret;
}

bool FakeLaserWithMotor::getEncodersRaw(double* encs)
{
    bool ret = true;
    for (int j = 0; j < m_njoints; j++)
    {
        bool ok = getEncoderRaw(j, &encs[j]);
        ret = ret && ok;

    }
    return ret;
}

bool FakeLaserWithMotor::getEncoderSpeedRaw(int j, double* sp)
{
    // To avoid returning uninitialized memory, we set the encoder speed to 0
    *sp = 0.0;
    return true;
}

bool FakeLaserWithMotor::getEncoderSpeedsRaw(double* spds)
{
    bool ret = true;
    for (int j = 0; j < m_njoints; j++)
    {
        ret &= getEncoderSpeedRaw(j, &spds[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::getEncoderAccelerationRaw(int j, double* acc)
{
    // To avoid returning uninitialized memory, we set the encoder acc to 0
    *acc = 0.0;

    return true;
}

bool FakeLaserWithMotor::getEncoderAccelerationsRaw(double* accs)
{
    bool ret = true;
    for (int j = 0; j < m_njoints; j++)
    {
        ret &= getEncoderAccelerationRaw(j, &accs[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::setRefAccelerationsRaw(const int n_joint, const int* joints, const double* accs)
{
    bool ret = true;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && setRefAccelerationRaw(joints[j], accs[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::getRefSpeedsRaw(const int n_joint, const int* joints, double* spds)
{
    bool ret = true;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && getRefSpeedRaw(joints[j], &spds[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::getRefAccelerationsRaw(const int n_joint, const int* joints, double* accs)
{
    bool ret = true;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && getRefAccelerationRaw(joints[j], &accs[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::stopRaw(const int n_joint, const int* joints)
{
    bool ret = true;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && stopRaw(joints[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::positionMoveRaw(const int n_joint, const int* joints, const double* refs)
{
    for (int j = 0; j < n_joint; j++)
    {
        yCDebug(FAKE_LASER_MOTORS, "j: %d; ref %f;\n", joints[j], refs[j]); fflush(stdout);
    }

    bool ret = true;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && positionMoveRaw(joints[j], refs[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::relativeMoveRaw(const int n_joint, const int* joints, const double* deltas)
{
    bool ret = true;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && relativeMoveRaw(joints[j], deltas[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::checkMotionDoneRaw(const int n_joint, const int* joints, bool* flag)
{
    bool ret = true;
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

bool FakeLaserWithMotor::setRefSpeedsRaw(const int n_joint, const int* joints, const double* spds)
{
    bool ret = true;
    for (int j = 0; j < n_joint; j++)
    {
        ret = ret && setRefSpeedRaw(joints[j], spds[j]);
    }
    return ret;
}

bool FakeLaserWithMotor::getTargetPositionRaw(int axis, double* ref)
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
    return true;
}

bool FakeLaserWithMotor::getTargetPositionsRaw(double* refs)
{
    bool ret = true;
    for (int i = 0; i < m_njoints; i++) {
        ret &= getTargetPositionRaw(i, &refs[i]);
    }
    return ret;
}

bool FakeLaserWithMotor::getTargetPositionsRaw(int nj, const int* jnts, double* refs)
{
    bool ret = true;
    for (int i = 0; i < nj; i++)
    {
        ret &= getTargetPositionRaw(jnts[i], &refs[i]);
    }
    return ret;
}

bool FakeLaserWithMotor::velocityMoveRaw(int j, double sp)
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
    return true;
}

bool FakeLaserWithMotor::velocityMoveRaw(const double* sp)
{
    bool ret = true;
    for (int i = 0; i < m_njoints; i++) {
        ret &= velocityMoveRaw(i, sp[i]);
    }
    return ret;
}
bool FakeLaserWithMotor::velocityMoveRaw(const int n_joint, const int* joints, const double* spds)
{
    bool ret = true;
    for (int i = 0; i < n_joint; i++)
    {
        ret &= velocityMoveRaw(joints[i], spds[i]);
    }
    return ret;
}

bool FakeLaserWithMotor::getRefVelocityRaw(int axis, double* ref)
{
    *ref = _command_speeds[axis];
    return true;
}

bool FakeLaserWithMotor::getRefVelocitiesRaw(double* refs)
{
    bool ret = true;
    for (int i = 0; i < m_njoints; i++)
    {
        ret &= getRefVelocityRaw(i, &refs[i]);
    }
    return ret;
}

bool FakeLaserWithMotor::getRefVelocitiesRaw(int nj, const int* jnts, double* refs)
{
    bool ret = true;
    for (int i = 0; i < nj; i++)
    {
        ret &= getRefVelocityRaw(jnts[i], &refs[i]);
    }
    return ret;
}


bool FakeLaserWithMotor::getAxisNameRaw(int axis, std::string& name)
{
    if (axis >= 0 && axis < m_njoints)
    {
        name = _axisName[axis];
        return true;
    }
    else
    {
        name = "ERROR";
        return false;
    }
}

bool FakeLaserWithMotor::getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type)
{
    if (axis >= 0 && axis < m_njoints)
    {
        type = _jointType[axis];
        return true;
    }
    else
    {
        return false;
    }
}
