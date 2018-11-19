/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <cstdio>
using namespace yarp::dev;
#define JOINTIDCHECK if (m >= castToMapper(helper)->axes()){yError("motor id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
////////////////////////
// Encoder Interface Timed Implementation
ImplementMotorEncoders::ImplementMotorEncoders(IMotorEncodersRaw *y):nj(0)
{
    iMotorEncoders=y;
    helper = nullptr;
}

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
    nj = size;
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
    auto* tmp = new double[nj];
    castToMapper(helper)->posA2E(val, tmp);

    bool ret = iMotorEncoders->setMotorEncodersRaw(tmp);
    delete [] tmp;
    
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
    bool ret;
    auto* tmp = new double[nj];
    ret=iMotorEncoders->getMotorEncodersRaw(tmp);

    castToMapper(helper)->posE2A(tmp, v);
    delete [] tmp;
    
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
    bool ret;
    auto* tmp = new double[nj];
    ret=iMotorEncoders->getMotorEncoderSpeedsRaw(tmp);

    castToMapper(helper)->velE2A(tmp, v);
    delete [] tmp;
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
    bool ret;
    auto* tmp = new double[nj];
    ret=iMotorEncoders->getMotorEncoderAccelerationsRaw(tmp);

    castToMapper(helper)->accE2A(tmp, v);
    delete [] tmp;
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
    bool ret;
    auto* tmp_v = new double[nj];
    auto* tmp_t = new double[nj];
    ret=iMotorEncoders->getMotorEncodersTimedRaw(tmp_v, tmp_t);

    castToMapper(helper)->posE2A(tmp_v, v);
    castToMapper(helper)->toUser(tmp_t, t);
    delete [] tmp_t;
    delete [] tmp_v;

    return ret;
}
