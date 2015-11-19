// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>

#include <stdio.h>
using namespace yarp::dev;

////////////////////////
// Encoder Interface Timed Implementation
ImplementMotorEncoders::ImplementMotorEncoders(IMotorEncodersRaw *y)
{
    iMotorEncoders=y;
    helper = 0;
    temp=0;
    temp2=0;
}

ImplementMotorEncoders::~ImplementMotorEncoders()
{
    uninitialize();
}

bool ImplementMotorEncoders:: initialize (int size, const int *amap, const double *enc, const double *zos)
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
bool ImplementMotorEncoders::uninitialize ()
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

bool ImplementMotorEncoders::getNumberOfMotorEncoders(int *num)
{
    (*num)=castToMapper(helper)->axes();
    return true;
}

bool ImplementMotorEncoders::resetMotorEncoder(int m)
{
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
    int k;
    double enc;

    castToMapper(helper)->posA2E(val, m, enc, k);

    return iMotorEncoders->setMotorEncoderRaw(k, enc);
}

bool ImplementMotorEncoders::getMotorEncoderCountsPerRevolution(int m, double* cpr)
{
    bool ret;
    int k=castToMapper(helper)->toHw(m);

    ret=iMotorEncoders->getMotorEncoderCountsPerRevolutionRaw(k, cpr);

    return ret;
}

bool ImplementMotorEncoders::setMotorEncoderCountsPerRevolution(int m, double cpr)
{
    int k;

    k=castToMapper(helper)->toHw(m);

    return iMotorEncoders->setMotorEncoderCountsPerRevolutionRaw(k, cpr);
}

bool ImplementMotorEncoders::setMotorEncoders(const double *val)
{
    castToMapper(helper)->posA2E(val, temp);

    return iMotorEncoders->setMotorEncodersRaw(temp);
}

bool ImplementMotorEncoders::getMotorEncoder(int m, double *v)
{
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
    castToMapper(helper)->axes();

    ret=iMotorEncoders->getMotorEncodersRaw(temp);

    castToMapper(helper)->posE2A(temp, v);

    return ret;
}

bool ImplementMotorEncoders::getMotorEncoderSpeed(int m, double *v)
{
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
    ret=iMotorEncoders->getMotorEncoderSpeedsRaw(temp);

    castToMapper(helper)->velE2A(temp, v);

    return ret;
}

bool ImplementMotorEncoders::getMotorEncoderAcceleration(int m, double *v)
{
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
    ret=iMotorEncoders->getMotorEncoderAccelerationsRaw(temp);

    castToMapper(helper)->accE2A(temp, v);

    return ret;
}

bool ImplementMotorEncoders::getMotorEncoderTimed(int m, double *v, double *t)
{
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
    ret=iMotorEncoders->getMotorEncodersTimedRaw(temp, temp2);

    castToMapper(helper)->posE2A(temp, v);
    castToMapper(helper)->toUser(temp2, t);

    return ret;
}
