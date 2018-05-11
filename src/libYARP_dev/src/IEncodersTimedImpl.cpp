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
ImplementEncodersTimed::ImplementEncodersTimed(IEncodersTimedRaw *y):nj(0)
{
    iEncoders=y;
    helper = nullptr;
}

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
    
    nj = size;
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
    double * tmp = new double[nj];
    castToMapper(helper)->posA2E(val, tmp);

    bool ret = iEncoders->setEncodersRaw(tmp);
    
    delete []tmp;
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
    bool ret;
    double *my_enc = new double[nj];
    ret=iEncoders->getEncodersRaw(my_enc);

    castToMapper(helper)->posE2A(my_enc, v);
    delete [] my_enc;
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
    double *tmp = new double[nj];
    ret=iEncoders->getEncoderSpeedsRaw(tmp);

    castToMapper(helper)->velE2A(tmp, v);
    delete [] tmp;
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
    double *tmp = new double[nj];
    ret=iEncoders->getEncoderAccelerationsRaw(tmp);

    castToMapper(helper)->accE2A(tmp, v);
    delete [] tmp;
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
    double *tmp_v = new double[nj];
    double *tmp_t = new double[nj];
    ret=iEncoders->getEncodersTimedRaw(tmp_v, tmp_t);

    castToMapper(helper)->posE2A(tmp_v, v);
    castToMapper(helper)->toUser(tmp_t, t);

    delete [] tmp_v;
    delete [] tmp_t;

    return ret;
}
