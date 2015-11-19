// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Marco Randazo and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#include <yarp/dev/ImplementImpedanceControl.h>
#include <yarp/dev/ControlBoardHelper.h>

using namespace yarp::dev;

/////////////// implement ImplementImpedanceControl
ImplementImpedanceControl::ImplementImpedanceControl(IImpedanceControlRaw *r)
{
    iImpedanceRaw=r;
    helper=0;
}

bool ImplementImpedanceControl::initialize(int size, const int *amap, const double *enc, const double *zos, const double *nw)
{
    if (helper!=0)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos, nw));
    yAssert (helper != 0);

    return true;
}

ImplementImpedanceControl::~ImplementImpedanceControl()
{
    uninitialize();
}

bool ImplementImpedanceControl::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }

    return true;
}

bool ImplementImpedanceControl::getAxes(int *axes)
{
    return iImpedanceRaw->getAxes(axes);
}

bool ImplementImpedanceControl::setImpedance(int j, double stiffness, double damping)
{
    int k;
    double stiff;
    double damp;
    castToMapper(helper)->impN2S(stiffness,j,stiff,k);
    castToMapper(helper)->impN2S(damping,j,damp,k);
    return iImpedanceRaw->setImpedanceRaw(k, stiff, damp);
}

bool ImplementImpedanceControl::getImpedance(int j, double *stiffness, double *damping)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    bool ret=iImpedanceRaw->getImpedanceRaw(k, stiffness, damping);
    *stiffness = (castToMapper(helper)->impS2N(*stiffness, k));
    *damping   = (castToMapper(helper)->impS2N(*damping, k));
    //prevent negative stiffness
    *stiffness = fabs (*stiffness);
    *damping   = fabs (*damping);
    return ret;
}

bool ImplementImpedanceControl::setImpedanceOffset(int j, double offset)
{
    int k;
    double off;
    castToMapper(helper)->trqN2S(offset,j,off,k);
    return iImpedanceRaw->setImpedanceOffsetRaw(k, off);
}

bool ImplementImpedanceControl::getImpedanceOffset(int j, double *offset)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    bool ret = iImpedanceRaw->getImpedanceOffsetRaw(k, offset);
    *offset    = (castToMapper(helper)->trqS2N(*offset,k));
    return ret;
}

bool ImplementImpedanceControl::getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iImpedanceRaw->getCurrentImpedanceLimitRaw(k, min_stiff, max_stiff, min_damp, max_damp);
}
