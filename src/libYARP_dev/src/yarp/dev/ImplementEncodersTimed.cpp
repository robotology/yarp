/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

////////////////////////
// Encoder Interface Timed Implementation
ImplementEncodersTimed::ImplementEncodersTimed(IEncodersTimedRaw *y):
    iEncoders(y),
    helper(nullptr),
    buffManager(nullptr)
{;}

ImplementEncodersTimed::~ImplementEncodersTimed()
{
    uninitialize();
}

bool ImplementEncodersTimed:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert (helper != nullptr);

    buffManager = new yarp::dev::impl::FixedSizeBuffersManager<double> (size);
    yAssert (buffManager != nullptr);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementEncodersTimed::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    if (buffManager!=nullptr)
    {
        delete buffManager;
        buffManager=nullptr;
    }
    return true;
}

bool ImplementEncodersTimed::getAxes(int *ax)
{
    (*ax)=castToMapper(helper)->axes();
    return true;
}

bool ImplementEncodersTimed::resetEncoder(int j)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(j);

    return iEncoders->resetEncoderRaw(k);
}

bool ImplementEncodersTimed::resetEncoders()
{
    return iEncoders->resetEncodersRaw();
}

bool ImplementEncodersTimed::setEncoder(int j, double val)
{
    JOINTIDCHECK
    int k;
    double enc;

    castToMapper(helper)->posA2E(val, j, enc, k);

    return iEncoders->setEncoderRaw(k, enc);
}

bool ImplementEncodersTimed::setEncoders(const double *val)
{
    yarp::dev::impl::Buffer<double> buffValues = buffManager->getBuffer();
    castToMapper(helper)->posA2E(val, buffValues.getData());
    bool ret = iEncoders->setEncodersRaw(buffValues.getData());
    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementEncodersTimed::getEncoder(int j, double *v)
{
    JOINTIDCHECK
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderRaw(k, &enc);

    *v=castToMapper(helper)->posE2A(enc, k);

    return ret;
}

bool ImplementEncodersTimed::getEncoders(double *v)
{
    yarp::dev::impl::Buffer<double> buffValues =buffManager->getBuffer();
    bool ret = iEncoders->getEncodersRaw(buffValues.getData());
    castToMapper(helper)->posE2A(buffValues.getData(), v);
    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementEncodersTimed::getEncoderSpeed(int j, double *v)
{
    JOINTIDCHECK
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderSpeedRaw(k, &enc);

    *v=castToMapper(helper)->velE2A(enc, k);

    return ret;
}

bool ImplementEncodersTimed::getEncoderSpeeds(double *v)
{
    yarp::dev::impl::Buffer<double> buffValues = buffManager->getBuffer();
    bool ret=iEncoders->getEncoderSpeedsRaw(buffValues.getData());
    castToMapper(helper)->velE2A(buffValues.getData(), v);
    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementEncodersTimed::getEncoderAcceleration(int j, double *v)
{
    JOINTIDCHECK
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderAccelerationRaw(k, &enc);

    *v=castToMapper(helper)->accE2A(enc, k);

    return ret;
}

bool ImplementEncodersTimed::getEncoderAccelerations(double *v)
{
    yarp::dev::impl::Buffer<double> buffValues = buffManager->getBuffer();
    bool ret = iEncoders->getEncoderAccelerationsRaw(buffValues.getData());
    castToMapper(helper)->accE2A(buffValues.getData(), v);
    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementEncodersTimed::getEncoderTimed(int j, double *v, double *t)
{
    JOINTIDCHECK
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderTimedRaw(k, &enc, t);

    *v=castToMapper(helper)->posE2A(enc, k);

    return ret;
}


bool ImplementEncodersTimed::getEncodersTimed(double *v, double *t)
{
    yarp::dev::impl::Buffer<double> b_v = buffManager->getBuffer();
    yarp::dev::impl::Buffer<double> b_t = buffManager->getBuffer();
    bool ret=iEncoders->getEncodersTimedRaw(b_v.getData(), b_t.getData());

    castToMapper(helper)->posE2A(b_v.getData(), v);
    castToMapper(helper)->toUser(b_t.getData(), t);

    buffManager->releaseBuffer(b_v);
    buffManager->releaseBuffer(b_t);
    return ret;
}
