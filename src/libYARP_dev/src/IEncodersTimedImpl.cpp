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
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK if (joints[idx] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

////////////////////////
// Encoder Interface Timed Implementation
ImplementEncodersTimed::ImplementEncodersTimed(IEncodersTimedRaw *y)
{
    iEncoders=y;
    helper = nullptr;
    temp=nullptr;
    temp2=nullptr;
}

ImplementEncodersTimed::~ImplementEncodersTimed()
{
    uninitialize();
}

bool ImplementEncodersTimed:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,nullptr));
    yAssert (helper != nullptr);
    temp=new double [size];
    yAssert (temp != nullptr);
    temp2=new double [size];
    yAssert (temp2 != nullptr);
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

    checkAndDestroy(temp);
    checkAndDestroy(temp2);

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
    castToMapper(helper)->posA2E(val, temp);

    return iEncoders->setEncodersRaw(temp);
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
    bool ret;

    ret=iEncoders->getEncodersRaw(temp);

    castToMapper(helper)->posE2A(temp, v);

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
    bool ret;
    ret=iEncoders->getEncoderSpeedsRaw(temp);

    castToMapper(helper)->velE2A(temp, v);

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
    bool ret;
    ret=iEncoders->getEncoderAccelerationsRaw(temp);

    castToMapper(helper)->accE2A(temp, v);

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
    bool ret;
    ret=iEncoders->getEncodersTimedRaw(temp, temp2);

    castToMapper(helper)->posE2A(temp, v);
    castToMapper(helper)->toUser(temp2, t);

    return ret;
}
