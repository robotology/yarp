/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/os/Log.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;

ImplementPositionDirect::ImplementPositionDirect(IPositionDirectRaw *y):
    iPDirect(y),
    helper(nullptr),
    intBuffManager(nullptr),
    doubleBuffManager(nullptr)
{;}


ImplementPositionDirect::~ImplementPositionDirect()
{
    uninitialize();
}

bool ImplementPositionDirect::initialize(int size, const int *amap, const double *enc, const double *zos)
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

    return true;
}

bool ImplementPositionDirect::uninitialize()
{
    if(helper!=nullptr)
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

ReturnValue ImplementPositionDirect::getAxes(int *axes)
{
    (*axes)=castToMapper(helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementPositionDirect::setPosition(int j, double ref)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    double enc;
    castToMapper(helper)->posA2E(ref, j, enc, k);
    return iPDirect->setPositionRaw(k, enc);
}

ReturnValue ImplementPositionDirect::setPositions(const int n_joints, const int *joints, const double *refs)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    if (n_joints > (int)intBuffManager->getBufferSize()) {
        return ReturnValue::return_code::return_value_error_generic;
    }

    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints.setValue(idx, castToMapper(helper)->toHw(joints[idx]));
        buffValues.setValue(idx, castToMapper(helper)->posA2E(refs[idx], joints[idx]));
    }

    ReturnValue ret = iPDirect->setPositionsRaw(n_joints, buffJoints.getData(), buffValues.getData());

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);

    return ret;
}

ReturnValue ImplementPositionDirect::setPositions(const double *refs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->posA2E(refs, buffValues.getData());
    ReturnValue ret = iPDirect->setPositionsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementPositionDirect::getRefPosition(const int j, double* ref)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    double tmp;
    k=castToMapper(helper)->toHw(j);

    ReturnValue ret = iPDirect->getRefPositionRaw(k, &tmp);

    *ref=(castToMapper(helper)->posE2A(tmp, k));
    return ret;
}

ReturnValue ImplementPositionDirect::getRefPositions(const int n_joints, const int* joints, double* refs)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iPDirect->getRefPositionsRaw(n_joints, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joints; idx++)
    {
        refs[idx]=castToMapper(helper)->posE2A(buffValues[idx], buffJoints[idx]);
    }

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);

    return ret;
}

ReturnValue ImplementPositionDirect::getRefPositions(double* refs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iPDirect->getRefPositionsRaw(buffValues.getData());
    castToMapper(helper)->posE2A(buffValues.getData(), refs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}
