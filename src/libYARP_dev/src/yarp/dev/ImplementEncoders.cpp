/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementEncoders.h>
#include <yarp/dev/ControlBoardHelper.h>

#include <cmath>

// Be careful: this file contains template implementations and is included by translation
// units that use the template (e.g. .cpp files). Avoid putting here non-template functions to
// avoid multiple definitions.

using namespace yarp::dev;

////////////////////////
// Encoder Interface Implementation
ImplementEncoders::ImplementEncoders(yarp::dev::IEncodersRaw  *y)
{
    iEncoders= y;
    helper = nullptr;
    temp=nullptr;
}

ImplementEncoders::~ImplementEncoders()
{
    uninitialize();
}

bool ImplementEncoders:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert (helper != nullptr);
    temp=new double [size];
    yAssert (temp != nullptr);
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementEncoders::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    checkAndDestroy(temp);

    return true;
}

ReturnValue ImplementEncoders::getAxes(int *ax)
{
    (*ax)=castToMapper(helper)->axes();
    return ReturnValue_ok;
}

ReturnValue ImplementEncoders::resetEncoder(int j)
{
    int k;
    k=castToMapper(helper)->toHw(j);

    return iEncoders->resetEncoderRaw(k);
}


ReturnValue ImplementEncoders::resetEncoders()
{
    return iEncoders->resetEncodersRaw();
}

ReturnValue ImplementEncoders::setEncoder(int j, double val)
{
    int k;
    double enc;

    castToMapper(helper)->posA2E(val, j, enc, k);

    return iEncoders->setEncoderRaw(k, enc);
}

ReturnValue ImplementEncoders::setEncoders(const double *val)
{
    castToMapper(helper)->posA2E(val, temp);

    return iEncoders->setEncodersRaw(temp);
}

ReturnValue ImplementEncoders::getEncoder(int j, double *v)
{
    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderRaw(k, &enc);

    *v=castToMapper(helper)->posE2A(enc, k);

    return ret;
}

ReturnValue ImplementEncoders::getEncoders(double *v)
{
    ReturnValue ret;
    castToMapper(helper)->axes();

    ret=iEncoders->getEncodersRaw(temp);

    castToMapper(helper)->posE2A(temp, v);

    return ret;
}

ReturnValue ImplementEncoders::getEncoderSpeed(int j, double *v)
{
    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderSpeedRaw(k, &enc);

    *v=castToMapper(helper)->velE2A(enc, k);

    return ret;
}

ReturnValue ImplementEncoders::getEncoderSpeeds(double *v)
{
    ReturnValue ret;
    ret=iEncoders->getEncoderSpeedsRaw(temp);

    castToMapper(helper)->velE2A(temp, v);

    return ret;
}

ReturnValue ImplementEncoders::getEncoderAcceleration(int j, double *v)
{
    int k;
    double enc;
    ReturnValue ret;

    k=castToMapper(helper)->toHw(j);

    ret=iEncoders->getEncoderAccelerationRaw(k, &enc);

    *v=castToMapper(helper)->accE2A(enc, k);

    return ret;
}

ReturnValue ImplementEncoders::getEncoderAccelerations(double *v)
{
    ReturnValue ret;
    ret=iEncoders->getEncoderAccelerationsRaw(temp);

    castToMapper(helper)->accE2A(temp, v);

    return ret;
}
