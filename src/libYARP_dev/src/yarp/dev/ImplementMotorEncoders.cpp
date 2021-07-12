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

#define JOINTIDCHECK if (m >= castToMapper(helper)->axes()){yError("motor id out of bound"); return false;}

////////////////////////
// Encoder Interface Timed Implementation
ImplementMotorEncoders::ImplementMotorEncoders(IMotorEncodersRaw *y):
    iMotorEncoders(y),
    helper(nullptr),
    buffManager(nullptr)
{;}

ImplementMotorEncoders::~ImplementMotorEncoders()
{
    uninitialize();
}

bool ImplementMotorEncoders:: initialize (int size, const int *amap, const double *enc, const double *zos)
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
bool ImplementMotorEncoders::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    if(buffManager)
    {
        delete buffManager;
        buffManager=nullptr;
    }

    return true;
}

bool ImplementMotorEncoders::getNumberOfMotorEncoders(int *num)
{
    (*num)=castToMapper(helper)->axes();
    return true;
}

bool ImplementMotorEncoders::resetMotorEncoder(int m)
{
    JOINTIDCHECK
    int k;
    k=castToMapper(helper)->toHw(m);

    return iMotorEncoders->resetMotorEncoderRaw(k);
}

bool ImplementMotorEncoders::resetMotorEncoders()
{
    return iMotorEncoders->resetMotorEncodersRaw();
}

bool ImplementMotorEncoders::setMotorEncoder(int m, const double val)
{
    JOINTIDCHECK
    int k;
    double enc;

    castToMapper(helper)->posA2E(val, m, enc, k);

    return iMotorEncoders->setMotorEncoderRaw(k, enc);
}

bool ImplementMotorEncoders::getMotorEncoderCountsPerRevolution(int m, double* cpr)
{
    JOINTIDCHECK
    bool ret;
    int k=castToMapper(helper)->toHw(m);

    ret=iMotorEncoders->getMotorEncoderCountsPerRevolutionRaw(k, cpr);

    return ret;
}

bool ImplementMotorEncoders::setMotorEncoderCountsPerRevolution(int m, double cpr)
{
    JOINTIDCHECK
    int k;

    k=castToMapper(helper)->toHw(m);

    return iMotorEncoders->setMotorEncoderCountsPerRevolutionRaw(k, cpr);
}

bool ImplementMotorEncoders::setMotorEncoders(const double *val)
{
    yarp::dev::impl::Buffer<double> buffValues = buffManager->getBuffer();
    castToMapper(helper)->posA2E(val, buffValues.getData());

    bool ret = iMotorEncoders->setMotorEncodersRaw(buffValues.getData());
    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementMotorEncoders::getMotorEncoder(int m, double *v)
{
    JOINTIDCHECK
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(m);

    ret=iMotorEncoders->getMotorEncoderRaw(k, &enc);

    *v=castToMapper(helper)->posE2A(enc, k);

    return ret;
}

bool ImplementMotorEncoders::getMotorEncoders(double *v)
{
    yarp::dev::impl::Buffer<double> buffValues = buffManager->getBuffer();
    bool ret=iMotorEncoders->getMotorEncodersRaw(buffValues.getData());
    castToMapper(helper)->posE2A(buffValues.getData(), v);
    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementMotorEncoders::getMotorEncoderSpeed(int m, double *v)
{
    JOINTIDCHECK
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(m);

    ret=iMotorEncoders->getMotorEncoderSpeedRaw(k, &enc);

    *v=castToMapper(helper)->velE2A(enc, k);

    return ret;
}

bool ImplementMotorEncoders::getMotorEncoderSpeeds(double *v)
{
    yarp::dev::impl::Buffer<double> buffValues = buffManager->getBuffer();
    bool ret=iMotorEncoders->getMotorEncoderSpeedsRaw(buffValues.getData());
    castToMapper(helper)->velE2A(buffValues.getData(), v);
    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementMotorEncoders::getMotorEncoderAcceleration(int m, double *v)
{
    JOINTIDCHECK
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(m);

    ret=iMotorEncoders->getMotorEncoderAccelerationRaw(k, &enc);

    *v=castToMapper(helper)->accE2A(enc, k);

    return ret;
}

bool ImplementMotorEncoders::getMotorEncoderAccelerations(double *v)
{
    yarp::dev::impl::Buffer<double> buffValues = buffManager->getBuffer();
    bool ret=iMotorEncoders->getMotorEncoderAccelerationsRaw(buffValues.getData());
    castToMapper(helper)->accE2A(buffValues.getData(), v);
    buffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementMotorEncoders::getMotorEncoderTimed(int m, double *v, double *t)
{
    JOINTIDCHECK
    int k;
    double enc;
    bool ret;

    k=castToMapper(helper)->toHw(m);

    ret=iMotorEncoders->getMotorEncoderTimedRaw(k, &enc, t);

    *v=castToMapper(helper)->posE2A(enc, k);

    return ret;
}


bool ImplementMotorEncoders::getMotorEncodersTimed(double *v, double *t)
{
    yarp::dev::impl::Buffer<double>b_v = buffManager->getBuffer();
    yarp::dev::impl::Buffer<double>b_t = buffManager->getBuffer();
    bool ret=iMotorEncoders->getMotorEncodersTimedRaw(b_v.getData(), b_t.getData());
    castToMapper(helper)->posE2A(b_v.getData(), v);
    castToMapper(helper)->toUser(b_t.getData(), t);
    buffManager->releaseBuffer(b_v);
    buffManager->releaseBuffer(b_t);
    return ret;
}
