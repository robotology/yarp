/*
 * Copyright (C) 2012 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <stdio.h>
using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK if (joints[idx] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

////////////////////////
// Encoder Interface Timed Implementation
ImplementEncodersTimed::ImplementEncodersTimed(IEncodersTimedRaw *y)
{
    iEncoders=y;
    helper = 0;
    temp=0;
    temp2=0;
}

ImplementEncodersTimed::~ImplementEncodersTimed()
{
    uninitialize();
}

bool ImplementEncodersTimed:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,0));
    yAssert (helper != 0);
    temp=new double [size];
    yAssert (temp != 0);
    temp2=new double [size];
    yAssert (temp2 != 0);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementEncodersTimed::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
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
    castToMapper(helper)->axes();

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
