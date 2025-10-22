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
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return ReturnValue::return_code::return_value_error_input_out_of_bounds;}

ImplementPositionControl::ImplementPositionControl(yarp::dev::IPositionControlRaw *y) :
    iPosition(y),
    helper(nullptr),
    intBuffManager(nullptr),
    doubleBuffManager(nullptr),
    boolBuffManager(nullptr)
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
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(helper != nullptr);

    intBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<int> (size);
    yAssert (intBuffManager != nullptr);

    doubleBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<double> (size);
    yAssert (doubleBuffManager != nullptr);

    boolBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<bool> (size, 1);
    yAssert (boolBuffManager != nullptr);
    return true;
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementPositionControl::uninitialize()
{
    if(helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
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

    if(boolBuffManager)
    {
        delete boolBuffManager;
        boolBuffManager=nullptr;
    }

    return true;
}

ReturnValue ImplementPositionControl::positionMove(int j, double ang)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->posA2E(ang, j, enc, k);
    return iPosition->positionMoveRaw(k, enc);
}

ReturnValue ImplementPositionControl::positionMove(const int n_joints, const int *joints, const double *refs)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
        buffValues[idx] = castToMapper(helper)->posA2E(refs[idx], joints[idx]);
    }
    ReturnValue ret = iPosition->positionMoveRaw(n_joints, buffJoints.getData(), buffValues.getData());

    intBuffManager->releaseBuffer(buffJoints);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::positionMove(const double *refs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->posA2E(refs, buffValues.getData());

    ReturnValue ret = iPosition->positionMoveRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::relativeMove(int j, double delta)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E(delta, j, enc, k);

    return iPosition->relativeMoveRaw(k,enc);
}

ReturnValue ImplementPositionControl::relativeMove(const int n_joints, const int *joints, const double *deltas)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
        buffValues[idx] = castToMapper(helper)->velA2E(deltas[idx], joints[idx]);
    }
    ReturnValue ret = iPosition->relativeMoveRaw(n_joints, buffJoints.getData(), buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementPositionControl::relativeMove(const double *deltas)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->velA2E(deltas, buffValues.getData());
    ReturnValue ret = iPosition->relativeMoveRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::checkMotionDone(int j, bool *flag)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);

    return iPosition->checkMotionDoneRaw(k,flag);
}

ReturnValue ImplementPositionControl::checkMotionDone(const int n_joints, const int *joints, bool *flags)
{
    JOINTSIDCHECK
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    ReturnValue ret = iPosition->checkMotionDoneRaw(n_joints, buffJoints.getData(), flags);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementPositionControl::checkMotionDone(bool *flag)
{
    return iPosition->checkMotionDoneRaw(flag);
}

ReturnValue ImplementPositionControl::setTrajSpeed(int j, double sp)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E_abs(sp, j, enc, k);
    return iPosition->setTrajSpeedRaw(k, enc);
}

ReturnValue ImplementPositionControl::setTrajSpeeds(const int n_joints, const int *joints, const double *spds)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joints; idx++)
    {
        castToMapper(helper)->velA2E_abs(spds[idx], joints[idx], buffValues[idx], buffJoints[idx]);
    }
    ReturnValue ret = iPosition->setTrajSpeedsRaw(n_joints, buffJoints.getData(), buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementPositionControl::setTrajSpeeds(const double *spds)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->velA2E_abs(spds, buffValues.getData());
    ReturnValue ret = iPosition->setTrajSpeedsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::setTrajAcceleration(int j, double acc)
{
    JOINTIDCHECK
    int k;
    double enc;

    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iPosition->setTrajAccelerationRaw(k, enc);
}

ReturnValue ImplementPositionControl::setTrajAccelerations(const int n_joints, const int *joints, const double *accs)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joints; idx++)
    {
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], buffValues[idx], buffJoints[idx]);
    }

    ReturnValue ret = iPosition->setTrajAccelerationsRaw(n_joints, buffJoints.getData(), buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementPositionControl::setTrajAccelerations(const double *accs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->accA2E_abs(accs, buffValues.getData());

    ReturnValue ret = iPosition->setTrajAccelerationsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::getTrajSpeed(int j, double *ref)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    ReturnValue ret = iPosition->getTrajSpeedRaw(k, &enc);

    *ref=(castToMapper(helper)->velE2A_abs(enc, k));

    return ret;
}

ReturnValue ImplementPositionControl::getTrajSpeeds(const int n_joints, const int *joints, double *spds)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iPosition->getTrajSpeedsRaw(n_joints, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joints; idx++)
    {
        spds[idx]=castToMapper(helper)->velE2A_abs(buffValues[idx], buffJoints[idx]);
    }
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementPositionControl::getTrajSpeeds(double *spds)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iPosition->getTrajSpeedsRaw(buffValues.getData());
    castToMapper(helper)->velE2A_abs(buffValues.getData(), spds);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::getTrajAccelerations(double *accs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret=iPosition->getTrajAccelerationsRaw(buffValues.getData());
    castToMapper(helper)->accE2A_abs(buffValues.getData(), accs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::getTrajAccelerations(const int n_joints, const int *joints, double *accs)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iPosition->getTrajAccelerationsRaw(n_joints, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joints; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(buffValues[idx], buffJoints[idx]);
    }
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementPositionControl::getTrajAcceleration(int j, double *acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    ReturnValue ret = iPosition->getTrajAccelerationRaw(k, &enc);

    *acc=castToMapper(helper)->accE2A_abs(enc, k);

    return ret;
}

ReturnValue ImplementPositionControl::stop(int j)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPosition->stopRaw(k);
}

ReturnValue ImplementPositionControl::stop(const int n_joint, const int *joints)
{
    yarp::dev::impl::Buffer<int> buffValues =intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        buffValues[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    ReturnValue ret = iPosition->stopRaw(n_joint, buffValues.getData());
    intBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::stop()
{
    return iPosition->stopRaw();
}

ReturnValue ImplementPositionControl::getAxes(int *axis)
{
    (*axis)=castToMapper(helper)->axes();

    return ReturnValue_ok;
}


ReturnValue ImplementPositionControl::getTargetPosition(const int j, double* ref)
{
    JOINTIDCHECK

    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    ReturnValue ret = iPosition->getTargetPositionRaw(k, &enc);

    *ref=castToMapper(helper)->posE2A(enc, k);

    return ret;
}

ReturnValue ImplementPositionControl::getTargetPositions(double* refs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret=iPosition->getTargetPositionsRaw(buffValues.getData());
    castToMapper(helper)->posE2A(buffValues.getData(), refs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionControl::getTargetPositions(const int n_joints, const int* joints, double* refs)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints =intBuffManager->getBuffer();
    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iPosition->getTargetPositionsRaw(n_joints, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joints; idx++)
    {
        refs[idx]=castToMapper(helper)->posE2A(buffValues[idx], buffJoints[idx]);
    }
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}
/////////////////// End Implement PostionControl
