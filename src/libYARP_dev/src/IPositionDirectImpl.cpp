/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/Log.h>

using namespace yarp::dev;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define MJOINTIDCHECK(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}
#define PJOINTIDCHECK(j) if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

ImplementPositionDirect::ImplementPositionDirect(IPositionDirectRaw *y) :
    iPDirect(y),
    helper(nullptr),
    temp_int(nullptr),
    temp_double(nullptr)
{
}

ImplementPositionDirect::~ImplementPositionDirect()
{
    uninitialize();
}

bool ImplementPositionDirect::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if(helper != nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,nullptr, nullptr, nullptr, nullptr));
    yAssert(helper != nullptr);

    temp_double=new double [size];
    yAssert(temp_double != nullptr);

    temp_int=new int [size];
    yAssert(temp_int != nullptr);
    return true;
}

bool ImplementPositionDirect::uninitialize()
{
    if(helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    checkAndDestroy(temp_double);
    checkAndDestroy(temp_int);

    return true;
}

bool ImplementPositionDirect::getAxes(int *axes)
{
    (*axes)=castToMapper(helper)->axes();
    return true;
}

bool ImplementPositionDirect::setPosition(int j, double ref)
{
    JOINTIDCHECK
    int k;
    double enc;
    castToMapper(helper)->posA2E(ref, j, enc, k);
    return iPDirect->setPositionRaw(k, enc);
}

bool ImplementPositionDirect::setPositions(const int n_joint, const int *joints, double *refs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        castToMapper(helper)->posA2E(refs[idx], joints[idx], temp_double[idx], temp_int[idx]);
    }
    return iPDirect->setPositionsRaw(n_joint, temp_int, temp_double);
}

bool ImplementPositionDirect::setPositions(const double *refs)
{
    castToMapper(helper)->posA2E(refs, temp_double);

    return iPDirect->setPositionsRaw(temp_double);
}

bool ImplementPositionDirect::getRefPosition(const int j, double* ref)
{
    JOINTIDCHECK
    int k;
    double tmp;
    k=castToMapper(helper)->toHw(j);

    bool ret = iPDirect->getRefPositionRaw(k, &tmp);

    *ref=(castToMapper(helper)->posE2A(tmp, k));
    return ret;
}

bool ImplementPositionDirect::getRefPositions(const int n_joint, const int* joints, double* refs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK(idx)
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iPDirect->getRefPositionsRaw(n_joint, temp_int, temp_double);

    for(int idx=0; idx<n_joint; idx++)
    {
        refs[idx]=castToMapper(helper)->posE2A(temp_double[idx], temp_int[idx]);
    }
    return ret;
}

bool ImplementPositionDirect::getRefPositions(double* refs)
{
    bool ret = iPDirect->getRefPositionsRaw(temp_double);
    castToMapper(helper)->posE2A(temp_double, refs);
    return ret;
}


// Stub impl

bool StubImplPositionDirectRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        yError("%s: not yet implemented\n", func);
    else
        yError("Function not yet implemented\n");

    return false;
}
