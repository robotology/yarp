/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;

ImplementCurrentControl::ImplementCurrentControl(ICurrentControlRaw *tq):
    iCurrentRaw(tq),
    helper(nullptr),
    intBuffManager(nullptr),
    doubleBuffManager(nullptr)
{;}

ImplementCurrentControl::~ImplementCurrentControl()
{
    uninitialize();
}

bool ImplementCurrentControl::initialize(int size, const int *amap, const double* ampsToSens)
{
    if (helper != nullptr) {
        return false;
    }

    intBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<int> (size);
    yAssert (intBuffManager != nullptr);

    doubleBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<double> (size);
    yAssert (doubleBuffManager != nullptr);

    helper = (void *)(new ControlBoardHelper(size, amap, nullptr, nullptr, nullptr, ampsToSens, nullptr, nullptr));
    yAssert (helper != nullptr);

    return true;
}

bool ImplementCurrentControl::uninitialize()
{
    if (helper!=nullptr)
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

ReturnValue  ImplementCurrentControl::getNumberOfMotors(int *axes)
{
    return iCurrentRaw->getNumberOfMotorsRaw(axes);
}

ReturnValue ImplementCurrentControl::getRefCurrent(int j, double *r)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    ReturnValue ret;
    double current;
    k=castToMapper(helper)->toHw(j);
    ret = iCurrentRaw->getRefCurrentRaw(k, &current);
    *r = castToMapper(helper)->ampereS2A(current, k);
    return ret;
}

ReturnValue ImplementCurrentControl::getRefCurrents(double *t)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iCurrentRaw->getRefCurrentsRaw(buffValues.getData());
    castToMapper(helper)->ampereS2A(buffValues.getData(),t);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementCurrentControl::setRefCurrents(const double *t)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    castToMapper(helper)->ampereA2S(t, buffValues.getData());
    ReturnValue ret = iCurrentRaw->setRefCurrentsRaw(buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementCurrentControl::setRefCurrent(int j, double t)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    double sens;
    castToMapper(helper)->ampereA2S(t,j,sens,k);
    return iCurrentRaw->setRefCurrentRaw(k, sens);
}

ReturnValue ImplementCurrentControl::getCurrents(double *t)
{
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ReturnValue ret = iCurrentRaw->getCurrentsRaw(buffValues.getData());
    castToMapper(helper)->ampereS2A(buffValues.getData(), t);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

ReturnValue ImplementCurrentControl::setRefCurrents(const int n_joints, const int *joints, const double *t)
{
    JOINTSIDCHECK

    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<int> buffJoints = intBuffManager->getBuffer();

    for(int idx=0; idx<n_joints; idx++)
    {
        buffJoints[idx] = castToMapper(helper)->toHw(joints[idx]);
        buffValues[idx] = castToMapper(helper)->ampereA2S(t[idx], joints[idx]);
    }

    ReturnValue ret = iCurrentRaw->setRefCurrentsRaw(n_joints, buffJoints.getData(), buffValues.getData());

    doubleBuffManager->releaseBuffer(buffValues);
    intBuffManager->releaseBuffer(buffJoints);
    return ret;
}

ReturnValue ImplementCurrentControl::getCurrent(int j, double *t)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    ReturnValue ret;
    double current;
    k=castToMapper(helper)->toHw(j);
    ret = iCurrentRaw->getCurrentRaw(k, &current);
    *t = castToMapper(helper)->ampereS2A(current, k);
    return ret;
}

ReturnValue ImplementCurrentControl::getCurrentRanges(double *min, double *max)
{
    yarp::dev::impl::Buffer<double> b_min = doubleBuffManager->getBuffer();
    yarp::dev::impl::Buffer<double> b_max = doubleBuffManager->getBuffer();
    ReturnValue ret = iCurrentRaw->getCurrentRangesRaw(b_min.getData(), b_max.getData());
    castToMapper(helper)->ampereS2A(b_min.getData(), min);
    castToMapper(helper)->ampereS2A(b_max.getData(), max);
    doubleBuffManager->releaseBuffer(b_min);
    doubleBuffManager->releaseBuffer(b_max);
    return ret;
}

ReturnValue ImplementCurrentControl::getCurrentRange(int j, double *min, double *max)
{
    JOINTIDCHECK(MAPPER_MAXID)
    int k;
    k=castToMapper(helper)->toHw(j);
    double min_t, max_t;
    ReturnValue ret = iCurrentRaw->getCurrentRangeRaw(k, &min_t, &max_t);
    *min = castToMapper(helper)->ampereS2A(min_t, k);
    *max = castToMapper(helper)->ampereS2A(max_t, k);
    return ret;
}
