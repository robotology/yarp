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
#define MJOINTIDCHECK_DEL1(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete [] tmp_joints; return false;}
#define MJOINTIDCHECK_DEL2(i) if (joints[i] >= castToMapper(helper)->axes()){yError("joint id out of bound"); delete [] tmp_joints; delete [] tmp_refs;return false;}

ImplementPositionDirect::ImplementPositionDirect(IPositionDirectRaw *y) :
    iPDirect(y),
    helper(nullptr),
    nj(0)
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

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    yAssert(helper != nullptr);

    nj=size;
    return true;
}

bool ImplementPositionDirect::uninitialize()
{
    if(helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

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

bool ImplementPositionDirect::setPositions(const int n_joint, const int *joints, const double *refs)
{
    int *tmp_joints =  new int [nj];
    double *tmp_refs = new double [nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL2(idx)
        castToMapper(helper)->posA2E(refs[idx], joints[idx], tmp_refs[idx], tmp_joints[idx]);
    }
    bool ret = iPDirect->setPositionsRaw(n_joint, tmp_joints, tmp_refs);
    delete [] tmp_joints;
    delete [] tmp_refs;
    
    return ret;
}

bool ImplementPositionDirect::setPositions(const double *refs)
{
    double *tmp = new double[nj];
    castToMapper(helper)->posA2E(refs, tmp);

    bool ret = iPDirect->setPositionsRaw(tmp);
    
    delete [] tmp;
    return ret;
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
    int * tmp_joints = new int[nj];
    for(int idx=0; idx<n_joint; idx++)
    {
        MJOINTIDCHECK_DEL1(idx)
        tmp_joints[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    double *tmp_refs = new double[nj];
    bool ret = iPDirect->getRefPositionsRaw(n_joint, tmp_joints, tmp_refs);

    for(int idx=0; idx<n_joint; idx++)
    {
        refs[idx]=castToMapper(helper)->posE2A(tmp_refs[idx], tmp_joints[idx]);
    }
    
    delete [] tmp_joints;
    delete [] tmp_refs;
    
    return ret;
}

bool ImplementPositionDirect::getRefPositions(double* refs)
{
    double *tmp=new double[nj];
    bool ret = iPDirect->getRefPositionsRaw(tmp);
    castToMapper(helper)->posE2A(tmp, refs);
    delete [] tmp;
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
