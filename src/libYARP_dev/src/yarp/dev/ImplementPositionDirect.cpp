/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

using namespace yarp::dev;
using namespace yarp::os;

#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

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
    if(helper != nullptr)
        return false;

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

bool ImplementPositionDirect::getAxes(int *axes)
{
    (*axes)=castToMapper(helper)->axes();
    return true;
}

bool ImplementPositionDirect::setPosition(int j, double ref)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->posA2E(ref, j, enc, k);
    return iPDirect->setPositionRaw(k, enc);
}

bool ImplementPositionDirect::setPositions(const int n_joint, const int *joints, const double *refs)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();

    if(n_joint > (int)intBuffManager->getBufferSize())
        return false;

    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints.setValue(idx, castToMapper(helper)->toHw(joints[idx]));
        buffValues.setValue(idx, castToMapper(helper)->posA2E(refs[idx], joints[idx]));
    }

    bool ret = iPDirect->setPositionsRaw(n_joint, buffJoints.getData(), buffValues.getData());

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);

    return ret;
}

bool ImplementPositionDirect::setPositions(const double *refs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->posA2E(refs, buffValues.getData());
    bool ret = iPDirect->setPositionsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPositionDirect::getRefPosition(const int j, double* ref)
{
    JOINTIDCHECK
    int k;
    double tmp;
    k=castToMapper(helper)->toHw(j);

    bool ret = iPDirect->getRefPositionRaw(k, &tmp);

    *ref=(castToMapper(helper)->posE2A(tmp, k));
    return ret;
}

bool ImplementPositionDirect::getRefPositions(const int n_joint, const int* joints, double* refs)
{
    if(!castToMapper(helper)->checkAxesIds(n_joint, joints))
        return false;

    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();

    for(int idx=0; idx<n_joint; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
    }

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = iPDirect->getRefPositionsRaw(n_joint, buffJoints.getData(), buffValues.getData());

    for(int idx=0; idx<n_joint; idx++)
    {
        refs[idx]=castToMapper(helper)->posE2A(buffValues[idx], buffJoints[idx]);
    }

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);

    return ret;
}

bool ImplementPositionDirect::getRefPositions(double* refs)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = iPDirect->getRefPositionsRaw(buffValues.getData());
    castToMapper(helper)->posE2A(buffValues.getData(), refs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}


// Stub impl

bool StubImplPositionDirectRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        yError("%s: not yet implemented\n", func);
    else
        yError("Function not yet implemented\n");

    return false;
}
