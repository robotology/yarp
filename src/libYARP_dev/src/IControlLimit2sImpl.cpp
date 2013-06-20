// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
* Author:  Alberto Cardellino
* email:   alberto.cardellino@iit.it
* website: www.robotcub.org
* Released under the terms the GNU General Public License, version 2 or any
* later version published by the Free Software Foundation.
*/


#include <stdio.h>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>

using namespace yarp::dev;


ImplementControlLimits2::ImplementControlLimits2(yarp::dev::IControlLimits2Raw *y)
{
    iLimits2 = y;
    helper = 0;
    temp_min = 0;
    temp_max = 0;
    temp_int = 0;
}


ImplementControlLimits2::~ImplementControlLimits2()
{
    uninitialize();
}


bool ImplementControlLimits2::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if(helper!=0)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,0));
    _YARP_ASSERT(helper != 0);
    temp_max=new double [size];
    _YARP_ASSERT(temp_max != 0);
    temp_min=new double [size];
    _YARP_ASSERT(temp_min != 0);
    temp_int=new int [size];
    _YARP_ASSERT(temp_int != 0);
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

/*
bool ImplementControlLimits2::setLimits(int n_axis, int *axes, double *min, double *max)
{
    for(int idx=0; idx<n_axis; idx++)
    {
        castToMapper(helper)->posA2E(min[idx], axes[idx], temp_min[idx], temp_int[idx]);
        castToMapper(helper)->posA2E(max[idx], axes[idx], temp_max[idx], temp_int[idx]);
        if( (max[idx] > min[idx]) && (temp_min[idx] > temp_max[idx])) //angle to encoder conversion factor is negative
        {
            double temp;   // exchange max and min limits
            temp = temp_min[idx];
            temp_min[idx] = temp_max[idx];
            temp_max[idx] = temp;
        }
    }
    return iLimits2->setLimitsRaw(n_axis, temp_int, temp_min, temp_max);
}

bool ImplementControlLimits2::setLimits(double *min, double *max)
{
    castToMapper(helper)->posA2E(min, temp_min);
    castToMapper(helper)->posA2E(max, temp_max);

    for(int idx=0; idx<castToMapper(helper)->nj; idx++)
    {
        if( (max > min) && (temp_min[idx] > temp_max[idx])) //angle to encoder conversion factor is negative
        {
            double temp;   // exchange max and min limits
            temp = temp_min[idx];
            temp_min[idx] = temp_max[idx];
            temp_max[idx] = temp;
        }
    }
    return iLimits2->setLimitsRaw(temp_min, temp_max);
}
*/

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

/*
bool ImplementControlLimits2::getLimits(int n_axis, int *axes, double *min, double *max)
{
    double minEnc=0;
    double maxEnc=0;
    bool ret = true;

    for(int idx=0; idx<n_axis; idx++)
    {
        temp_int[idx] = castToMapper(helper)->toHw(axes[idx]);
    }

    ret = ret && iLimits2->getLimitsRaw(n_axis, temp_int, temp_min, temp_max);

    for(int idx=0; idx<n_axis; idx++)
    {
        min[idx] = castToMapper(helper)->posE2A(temp_min[idx], temp_int[idx]);
        max[idx] = castToMapper(helper)->posE2A(temp_max[idx], temp_int[idx]);

        if( (temp_max[idx] < temp_min[idx]) && (minEnc < maxEnc)) //angle to encoder conversion factor is negative
        {
            double temp;   // exchange max and min limits
            temp = temp_min[idx];
            temp_min[idx] = temp_max[idx];
            temp_max[idx] = temp;
        }
    }
    return ret;
}

bool ImplementControlLimits2::getLimits(double *min, double *max)
{
    bool ret = iLimits2->getLimitsRaw(temp_min, temp_max);
    if(ret)
    {
        castToMapper(helper)->posE2A(temp_min, min);
        castToMapper(helper)->posE2A(temp_max, max);

        for(int idx=0; idx<castToMapper(helper)->nj; idx++)
        {
            if( (temp_max[idx] < temp_min[idx]) && (min[idx] < max[idx])) //angle to encoder conversion factor is negative
            {
                double temp;   // exchange max and min limits
                temp = temp_min[idx];
                temp_min[idx] = temp_max[idx];
                temp_max[idx] = temp;
            }
        }
    }
    return ret;
}
*/


bool ImplementControlLimits2::setVelLimits(int axis, double min, double max)
{
    double minEnc=0;
    double maxEnc=0;

    int k=0;
    castToMapper(helper)->velA2E(min, axis, minEnc, k);
    castToMapper(helper)->velA2E(max, axis, maxEnc, k);

    if( (max > min) && (minEnc > maxEnc)) //angle to encoder conversion factor is negative
    {
        double temp;   // exchange max and min limits
        temp = minEnc;
        minEnc = maxEnc;
        maxEnc = temp;
    }

    return iLimits2->setVelLimitsRaw(k, minEnc, maxEnc);
}

/*
bool ImplementControlLimits2::setVelLimits(int n_axis, int *axes, double *min, double *max)
{
    for(int idx=0; idx<n_axis; idx++)
    {
        castToMapper(helper)->velA2E(min[idx], axes[idx], temp_min[idx], temp_int[idx]);
        castToMapper(helper)->velA2E(max[idx], axes[idx], temp_max[idx], temp_int[idx]);
        if( (max[idx] > min[idx]) && (temp_min[idx] > temp_max[idx])) //angle to encoder conversion factor is negative
        {
            double temp;   // exchange max and min limits
            temp = temp_min[idx];
            temp_min[idx] = temp_max[idx];
            temp_max[idx] = temp;
        }
    }
    return iLimits2->setVelLimitsRaw(n_axis, temp_int, temp_min, temp_max);
}

bool ImplementControlLimits2::setVelLimits(double *min, double *max)
{
    castToMapper(helper)->velA2E(min, temp_min);
    castToMapper(helper)->velA2E(max, temp_max);

    for(int idx=0; idx<castToMapper(helper)->nj; idx++)
    {
        if( (max[idx] > min[idx]) && (temp_min[idx] > temp_max[idx])) //angle to encoder conversion factor is negative
        {
            double temp;   // exchange max and min limits
            temp = temp_min[idx];
            temp_min[idx] = temp_max[idx];
            temp_max[idx] = temp;
        }
    }
    return iLimits2->setVelLimitsRaw(temp_min, temp_max);
}
*/

bool ImplementControlLimits2::getVelLimits(int axis, double *min, double *max)
{
    double minEnc=0;
    double maxEnc=0;

    int k=castToMapper(helper)->toHw(axis);
    bool ret=iLimits2->getVelLimitsRaw(k, &minEnc, &maxEnc);

    *min=castToMapper(helper)->velE2A(minEnc, k);
    *max=castToMapper(helper)->velE2A(maxEnc, k);

    if( (*max < *min) && (minEnc < maxEnc)) //angle to encoder conversion factor is negative
    {
        double temp;   // exchange max and min limits
        temp = *min;
        *min = *max;
        *max = temp;
    }
    return ret;
}

/*
bool ImplementControlLimits2::getVelLimits(int n_axis, int *axes, double *min, double *max)
{
    bool ret = true;

    for(int idx=0; idx<n_axis; idx++)
    {
        temp_int[idx] = castToMapper(helper)->toHw(axes[idx]);
    }

    ret = ret && iLimits2->getVelLimitsRaw(n_axis, temp_int, temp_min, temp_max);

    for(int idx=0; idx<n_axis; idx++)
    {
        min[idx] = castToMapper(helper)->velE2A(temp_min[idx], temp_int[idx]);
        max[idx] = castToMapper(helper)->velE2A(temp_max[idx], temp_int[idx]);

        if( (temp_max[idx] < temp_min[idx]) && (min[idx] < max[idx])) //angle to encoder conversion factor is negative
        {
            double temp;   // exchange max and min limits
            temp = temp_min[idx];
            temp_min[idx] = temp_max[idx];
            temp_max[idx] = temp;
        }
    }
    return ret;
}

bool ImplementControlLimits2::getVelLimits(double *min, double *max)
{
    bool ret = iLimits2->getVelLimitsRaw(temp_min, temp_max);
    if(ret)
    {
        castToMapper(helper)->velE2A(temp_min, min);
        castToMapper(helper)->velE2A(temp_max, max);

        for(int idx=0; idx<castToMapper(helper)->nj; idx++)
        {
            if( (temp_max[idx] < temp_min[idx]) && (min[idx] < max[idx])) //angle to encoder conversion factor is negative
            {
                double temp;   // exchange max and min limits
                temp = temp_min[idx];
                temp_min[idx] = temp_max[idx];
                temp_max[idx] = temp;
            }
        }
    }
    return ret;
}
*/
