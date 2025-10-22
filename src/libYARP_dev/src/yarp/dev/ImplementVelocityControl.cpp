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

ReturnValue ImplementVelocityControl::getAxes(int *ax)
{
    (*ax)=castToMapper(helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementVelocityControl::velocityMove(int j, double sp)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    double enc;
    castToMapper(helper)->velA2E(sp, j, enc, k);
    return iVelocity->velocityMoveRaw(k, enc);
}

ReturnValue ImplementVelocityControl::velocityMove(const int n_joints, const int *joints, const double *spds)
{
    JOINTSIDCHECK
    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
        buffValues[idx] = castToMapper(helper)->velA2E(spds[idx], joints[idx]);
    }
    ReturnValue ret = iVelocity->velocityMoveRaw(n_joints, buffJoints.getData(), buffValues.getData());

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementVelocityControl::velocityMove(const double *sp)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->velA2E(sp, buffValues.getData());
    ReturnValue ret = iVelocity->velocityMoveRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementVelocityControl::getTargetVelocity(const int j, double* vel)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    double tmp;
    k=castToMapper(helper)->toHw(j);
    ReturnValue ret = iVelocity->getTargetVelocityRaw(k, &tmp);
    *vel=castToMapper(helper)->velE2A(tmp, k);
    return ret;
}

ReturnValue ImplementVelocityControl::getTargetVelocities(double *vels)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret=iVelocity->getTargetVelocitiesRaw(buffValues.getData());
    castToMapper(helper)->velE2A(buffValues.getData(), vels);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementVelocityControl::getTargetVelocities(const int n_joints, const int *joints, double *vels)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    ReturnValue ret = iVelocity->getTargetVelocitiesRaw(n_joints, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joints; idx++)
    {
        vels[idx]=castToMapper(helper)->velE2A(buffValues[idx], buffJoints[idx]);
    }

    intBuffManager->releaseBuffer(buffJoints);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementVelocityControl::setTrajAcceleration(int j, double acc)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    double enc;
    castToMapper(helper)->accA2E_abs(acc, j, enc, k);
    return iVelocity->setTrajAccelerationRaw(k, enc);
}

ReturnValue ImplementVelocityControl::setTrajAccelerations(const int n_joints, const int *joints, const double *accs)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], buffValues[idx], buffJoints[idx]);
    }
    ReturnValue ret = iVelocity->setTrajAccelerationsRaw(n_joints, buffJoints.getData(), buffValues.getData());

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);

    return ret;
}

ReturnValue ImplementVelocityControl::setTrajAccelerations(const double *accs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->accA2E_abs(accs, buffValues.getData());
    ReturnValue ret = iVelocity->setTrajAccelerationsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementVelocityControl::getTrajAcceleration(int j, double *acc)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    ReturnValue ret = iVelocity->getTrajAccelerationRaw(k, &enc);
    *acc=castToMapper(helper)->accE2A_abs(enc, k);
    return ret;
}

ReturnValue ImplementVelocityControl::getTrajAccelerations(const int n_joints, const int *joints, double *accs)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    ReturnValue ret = iVelocity->getTrajAccelerationsRaw(n_joints, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joints; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(buffValues[idx], buffJoints[idx]);
    }

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}


ReturnValue ImplementVelocityControl::getTrajAccelerations(double *accs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret=iVelocity->getTrajAccelerationsRaw(buffValues.getData());
    castToMapper(helper)->accE2A_abs(buffValues.getData(), accs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}


ReturnValue ImplementVelocityControl::stop(int j)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    k=castToMapper(helper)->toHw(j);
    return iVelocity->stopRaw(k);
}


ReturnValue ImplementVelocityControl::stop(const int n_joints, const int *joints)
{
    JOINTSIDCHECK
    yarp::dev::impl::Buffer<int> buffJoints =  intBuffManager->getBuffer();
    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }
    ReturnValue ret = iVelocity->stopRaw(n_joints, buffJoints.getData());
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}


ReturnValue ImplementVelocityControl::stop()
{
    return iVelocity->stopRaw();
}
