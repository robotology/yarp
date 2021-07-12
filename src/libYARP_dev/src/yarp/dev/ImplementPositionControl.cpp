/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ImplementPositionControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

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
    if(helper != nullptr)
        return false;

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

bool ImplementPositionControl::positionMove(int j, double ang)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->posA2E(ang, j, enc, k);
    return iPosition->positionMoveRaw(k, enc);
}

bool ImplementPositionControl::positionMove(const int n_joint, const int *joints, const double *refs)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();

    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
        buffValues[idx] = castToMapper(helper)->posA2E(refs[idx], joints[idx]);
    }
    bool ret = iPosition->positionMoveRaw(n_joint, buffJoints.getData(), buffValues.getData());

    intBuffManager->releaseBuffer(buffJoints);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::positionMove(const double *refs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->posA2E(refs, buffValues.getData());

    bool ret = iPosition->positionMoveRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::relativeMove(int j, double delta)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E(delta, j, enc, k);

    return iPosition->relativeMoveRaw(k,enc);
}

bool ImplementPositionControl::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
        buffValues[idx] = castToMapper(helper)->velA2E(deltas[idx], joints[idx]);
    }
    bool ret = iPosition->relativeMoveRaw(n_joint, buffJoints.getData(), buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementPositionControl::relativeMove(const double *deltas)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->velA2E(deltas, buffValues.getData());
    bool ret = iPosition->relativeMoveRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::checkMotionDone(int j, bool *flag)
{
    JOINTIDCHECK
    int k=castToMapper(helper)->toHw(j);

    return iPosition->checkMotionDoneRaw(k,flag);
}

bool ImplementPositionControl::checkMotionDone(const int n_joint, const int *joints, bool *flags)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    bool ret = iPosition->checkMotionDoneRaw(n_joint, buffJoints.getData(), flags);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementPositionControl::checkMotionDone(bool *flag)
{
    return iPosition->checkMotionDoneRaw(flag);
}

bool ImplementPositionControl::setRefSpeed(int j, double sp)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E_abs(sp, j, enc, k);
    return iPosition->setRefSpeedRaw(k, enc);
}

bool ImplementPositionControl::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        castToMapper(helper)->velA2E_abs(spds[idx], joints[idx], buffValues[idx], buffJoints[idx]);
    }
    bool ret = iPosition->setRefSpeedsRaw(n_joint, buffJoints.getData(), buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementPositionControl::setRefSpeeds(const double *spds)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->velA2E_abs(spds, buffValues.getData());
    bool ret = iPosition->setRefSpeedsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::setRefAcceleration(int j, double acc)
{
    JOINTIDCHECK
    int k;
    double enc;

    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iPosition->setRefAccelerationRaw(k, enc);
}

bool ImplementPositionControl::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], buffValues[idx], buffJoints[idx]);
    }

    bool ret = iPosition->setRefAccelerationsRaw(n_joint, buffJoints.getData(), buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementPositionControl::setRefAccelerations(const double *accs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->accA2E_abs(accs, buffValues.getData());

    bool ret = iPosition->setRefAccelerationsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::getRefSpeed(int j, double *ref)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    bool ret = iPosition->getRefSpeedRaw(k, &enc);

    *ref=(castToMapper(helper)->velE2A_abs(enc, k));

    return ret;
}

bool ImplementPositionControl::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = iPosition->getRefSpeedsRaw(n_joint, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joint; idx++)
    {
        spds[idx]=castToMapper(helper)->velE2A_abs(buffValues[idx], buffJoints[idx]);
    }
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementPositionControl::getRefSpeeds(double *spds)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = iPosition->getRefSpeedsRaw(buffValues.getData());
    castToMapper(helper)->velE2A_abs(buffValues.getData(), spds);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::getRefAccelerations(double *accs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret=iPosition->getRefAccelerationsRaw(buffValues.getData());
    castToMapper(helper)->accE2A_abs(buffValues.getData(), accs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = iPosition->getRefAccelerationsRaw(n_joint, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joint; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(buffValues[idx], buffJoints[idx]);
    }
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementPositionControl::getRefAcceleration(int j, double *acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iPosition->getRefAccelerationRaw(k, &enc);

    *acc=castToMapper(helper)->accE2A_abs(enc, k);

    return ret;
}

bool ImplementPositionControl::stop(int j)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPosition->stopRaw(k);
}

bool ImplementPositionControl::stop(const int n_joint, const int *joints)
{
    yarp::dev::impl::Buffer<int> buffValues =intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        buffValues[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPosition->stopRaw(n_joint, buffValues.getData());
    intBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::stop()
{
    return iPosition->stopRaw();
}

bool ImplementPositionControl::getAxes(int *axis)
{
    (*axis)=castToMapper(helper)->axes();

    return true;
}


bool ImplementPositionControl::getTargetPosition(const int joint, double* ref)
{
    if(!castToMapper(helper)->checkAxisId(joint))
        return false;
    int k;
    double enc;
    k=castToMapper(helper)->toHw(joint);
    bool ret = iPosition->getTargetPositionRaw(k, &enc);

    *ref=castToMapper(helper)->posE2A(enc, k);

    return ret;
}

bool ImplementPositionControl::getTargetPositions(double* refs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret=iPosition->getTargetPositionsRaw(buffValues.getData());
    castToMapper(helper)->posE2A(buffValues.getData(), refs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionControl::getTargetPositions(const int n_joint, const int* joints, double* refs)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<int> buffJoints =intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = iPosition->getTargetPositionsRaw(n_joint, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joint; idx++)
    {
        refs[idx]=castToMapper(helper)->posE2A(buffValues[idx], buffJoints[idx]);
    }
    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}
/////////////////// End Implement PostionControl



// Stub interface

bool StubImplPositionControlRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        yError("%s: not yet implemented\n", func);
    else
        yError("Function not yet implemented\n");

    return false;
}
