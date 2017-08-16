/*
 * Copyright (C) 2013 Istituto Italiano di Tecnologia (IIT)
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>

using namespace yarp::dev;


ImplementControlLimits2::ImplementControlLimits2(yarp::dev::IControlLimits2Raw *y) :
    iLimits2(y),
    helper(nullptr),
    temp_int(nullptr),
    temp_max(nullptr),
    temp_min(nullptr)
{

}


ImplementControlLimits2::~ImplementControlLimits2()
{
    uninitialize();
}

/**
 * Clean up internal data and memory.
 * @return true if uninitialization is executed, false otherwise.
 */
bool ImplementControlLimits2::uninitialize()
{
    if(helper != nullptr)
    {
        delete castToMapper(helper);
        helper = nullptr;
    }
    checkAndDestroy(temp_int);
    checkAndDestroy(temp_min);
    checkAndDestroy(temp_max);

    return true;
}

bool ImplementControlLimits2::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if(helper != nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,nullptr));
    yAssert(helper != nullptr);
    temp_max=new double [size];
    yAssert(temp_max != nullptr);
    temp_min=new double [size];
    yAssert(temp_min != nullptr);
    temp_int=new int [size];
    yAssert(temp_int != nullptr);
    return true;
}


bool ImplementControlLimits2::setLimits(int axis, double min, double max)
{
    double minEnc=0;
    double maxEnc=0;

    int k=0;
    castToMapper(helper)->posA2E(min, axis, minEnc, k);
    castToMapper(helper)->posA2E(max, axis, maxEnc, k);

    if( (max > min) && (minEnc > maxEnc)) //angle to encoder conversion factor is negative
    {
        double temp;   // exchange max and min limits
        temp = minEnc;
        minEnc = maxEnc;
        maxEnc = temp;
    }

    return iLimits2->setLimitsRaw(k, minEnc, maxEnc);
}


bool ImplementControlLimits2::getLimits(int axis, double *min, double *max)
{
    double minEnc=0;
    double maxEnc=0;

    int k=castToMapper(helper)->toHw(axis);
    bool ret=iLimits2->getLimitsRaw(k, &minEnc, &maxEnc);

    *min=castToMapper(helper)->posE2A(minEnc, k);
    *max=castToMapper(helper)->posE2A(maxEnc, k);

    if( (*max < *min) && (minEnc < maxEnc)) //angle to encoder conversion factor is negative
    {
        double temp;   // exchange max and min limits
        temp = *min;
        *min = *max;
        *max = temp;
    }
    return ret;
}

bool ImplementControlLimits2::setVelLimits(int axis, double min, double max)
{
    double minEnc=0;
    double maxEnc=0;

    int k=0;
    castToMapper(helper)->velA2E_abs(min, axis, minEnc, k);
    castToMapper(helper)->velA2E_abs(max, axis, maxEnc, k);

    return iLimits2->setVelLimitsRaw(k, minEnc, maxEnc);
}

bool ImplementControlLimits2::getVelLimits(int axis, double *min, double *max)
{
    double minEnc=0;
    double maxEnc=0;

    int k=castToMapper(helper)->toHw(axis);
    bool ret=iLimits2->getVelLimitsRaw(k, &minEnc, &maxEnc);

    *min = castToMapper(helper)->velE2A_abs(minEnc, k);
    *max = castToMapper(helper)->velE2A_abs(maxEnc, k);

    return ret;
}
