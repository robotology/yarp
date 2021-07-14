/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ImplementVelocityControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementVelocityControl::ImplementVelocityControl(IVelocityControlRaw *y) :
    iVelocity(y),
    helper(nullptr),
    intBuffManager(nullptr),
    doubleBuffManager(nullptr)
{;}

ImplementVelocityControl::~ImplementVelocityControl()
{
    uninitialize();
}

bool ImplementVelocityControl::initialize(int size, const int *axis_map, const double *enc, const double *zeros)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, axis_map, enc, zeros));
    yAssert (helper != nullptr);

    intBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<int> (size);
    yAssert (intBuffManager != nullptr);

    doubleBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<double> (size);
    yAssert (doubleBuffManager != nullptr);

    return true;
}

bool ImplementVelocityControl::uninitialize()
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

    return true;
}

bool ImplementVelocityControl::getAxes(int *ax)
{
    (*ax)=castToMapper(helper)->axes();
    return true;
}

bool ImplementVelocityControl::velocityMove(int j, double sp)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->velA2E(sp, j, enc, k);
    return iVelocity->velocityMoveRaw(k, enc);
}

bool ImplementVelocityControl::velocityMove(const int n_joint, const int *joints, const double *spds)
{
    if (!castToMapper(helper)->checkAxesIds(n_joint, joints)) {
        return false;
    }

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
        buffValues[idx] = castToMapper(helper)->velA2E(spds[idx], joints[idx]);
    }
    bool ret = iVelocity->velocityMoveRaw(n_joint, buffJoints.getData(), buffValues.getData());

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

bool ImplementVelocityControl::velocityMove(const double *sp)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->velA2E(sp, buffValues.getData());
    bool ret = iVelocity->velocityMoveRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementVelocityControl::getRefVelocity(const int j, double* vel)
{
    JOINTIDCHECK
    int k;
    double tmp;
    k=castToMapper(helper)->toHw(j);
    bool ret = iVelocity->getRefVelocityRaw(k, &tmp);
    *vel=castToMapper(helper)->velE2A(tmp, k);
    return ret;
}

bool ImplementVelocityControl::getRefVelocities(double *vels)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret=iVelocity->getRefVelocitiesRaw(buffValues.getData());
    castToMapper(helper)->velE2A(buffValues.getData(), vels);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementVelocityControl::getRefVelocities(const int n_joint, const int *joints, double *vels)
{
    if (!castToMapper(helper)->checkAxesIds(n_joint, joints)) {
        return false;
    }

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iVelocity->getRefVelocitiesRaw(n_joint, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joint; idx++)
    {
        vels[idx]=castToMapper(helper)->velE2A(buffValues[idx], buffJoints[idx]);
    }

    intBuffManager->releaseBuffer(buffJoints);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementVelocityControl::setRefAcceleration(int j, double acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iVelocity->setRefAccelerationRaw(k, enc);
}

bool ImplementVelocityControl::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    if (!castToMapper(helper)->checkAxesIds(n_joint, joints)) {
        return false;
    }

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joint; idx++)
    {
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], buffValues[idx], buffJoints[idx]);
    }
    bool ret = iVelocity->setRefAccelerationsRaw(n_joint, buffJoints.getData(), buffValues.getData());

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);

    return ret;
}

bool ImplementVelocityControl::setRefAccelerations(const double *accs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->accA2E_abs(accs, buffValues.getData());
    bool ret = iVelocity->setRefAccelerationsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementVelocityControl::getRefAcceleration(int j, double *acc)
{
    JOINTIDCHECK
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iVelocity->getRefAccelerationRaw(k, &enc);
    *acc=castToMapper(helper)->accE2A_abs(enc, k);
    return ret;
}

bool ImplementVelocityControl::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    if (!castToMapper(helper)->checkAxesIds(n_joint, joints)) {
        return false;
    }

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iVelocity->getRefAccelerationsRaw(n_joint, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joint; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(buffValues[idx], buffJoints[idx]);
    }

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}


bool ImplementVelocityControl::getRefAccelerations(double *accs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret=iVelocity->getRefAccelerationsRaw(buffValues.getData());
    castToMapper(helper)->accE2A_abs(buffValues.getData(), accs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}


bool ImplementVelocityControl::stop(int j)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);
    return iVelocity->stopRaw(k);
}


bool ImplementVelocityControl::stop(const int n_joint, const int *joints)
{
    if (!castToMapper(helper)->checkAxesIds(n_joint, joints)) {
        return false;
    }

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    bool ret = iVelocity->stopRaw(n_joint, buffJoints.getData());
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}


bool ImplementVelocityControl::stop()
{
    return iVelocity->stopRaw();
}
