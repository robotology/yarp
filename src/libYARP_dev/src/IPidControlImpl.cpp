/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale <lorenzo.natale@iit.it>, Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/IPidControlImpl.h>
#include <yarp/dev/ControlBoardHelper.h>

#include <cmath>

using namespace yarp::dev;

//////////////////// Implement PidControl interface
ImplementPidControl::ImplementPidControl(IPidControlRaw *y)
{
    iPid= dynamic_cast<IPidControlRaw *> (y);
    helper = 0;
    temp=0;
    tmpPids=0;
}

ImplementPidControl::~ImplementPidControl()
{
    uninitialize();
}

bool ImplementPidControl:: initialize (int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,0));
    yAssert (helper != 0);
    temp=new double [size];
    yAssert (temp != 0);
    tmpPids=new Pid[size];
    yAssert (tmpPids != 0);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementPidControl::uninitialize ()
{
    if (helper!=0)
        delete castToMapper(helper);
    helper=0;

    checkAndDestroy(tmpPids);
    checkAndDestroy(temp);

    return true;
}

bool ImplementPidControl::setPid(int j, const Pid &pid)
{
    int k=castToMapper(helper)->toHw(j);
    return iPid->setPidRaw(k,pid);
}

bool ImplementPidControl::setPids(const Pid *pids)
{
    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toHw(j);
        tmpPids[tmp]=pids[j];
    }

    return iPid->setPidsRaw(tmpPids);
}

bool ImplementPidControl::setReference(int j, double ref)
{
    int k=0;
    double enc;
    castToMapper(helper)->posA2E(ref, j, enc, k);

    return iPid->setReferenceRaw(k, enc);
}

bool ImplementPidControl::setReferences(const double *refs)
{
    castToMapper(helper)->posA2E(refs, temp);

    return iPid->setReferencesRaw(temp);
}

bool ImplementPidControl::setErrorLimit(int j, double limit)
{
    int k;
    double enc;
    castToMapper(helper)->posA2E(limit, j, enc, k);

    return iPid->setErrorLimitRaw(k, enc);
}

bool ImplementPidControl::setErrorLimits(const double *limits)
{
    castToMapper(helper)->posA2E(limits, temp);

    return iPid->setErrorLimitsRaw(temp);
}


bool ImplementPidControl::getError(int j, double *err)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    bool ret=iPid->getErrorRaw(k, &enc);

    *err=castToMapper(helper)->velE2A(enc, k);

    return ret;
}

bool ImplementPidControl::getErrors(double *errs)
{
    bool ret;
    ret=iPid->getErrorsRaw(temp);

    castToMapper(helper)->velE2A(temp, errs);

    return ret;
}

bool ImplementPidControl::getOutput(int j, double *out)
{
    int k;

    k=castToMapper(helper)->toHw(j);

    bool ret=iPid->getOutputRaw(k, out);

    return ret;
}

bool ImplementPidControl::getOutputs(double *outs)
{
    bool ret=iPid->getOutputsRaw(temp);

    castToMapper(helper)->toUser(temp, outs);

    return ret;
}

bool ImplementPidControl::getPid(int j, Pid *pid)
{
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPid->getPidRaw(k, pid);
}

bool ImplementPidControl::getPids(Pid *pids)
{
    bool ret=iPid->getPidsRaw(tmpPids);
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
        pids[castToMapper(helper)->toUser(j)]=tmpPids[j];

    return ret;
}

bool ImplementPidControl::getReference(int j, double *ref)
{
    bool ret;
    int k;
    double enc;

    k=castToMapper(helper)->toHw(j);

    ret=iPid->getReferenceRaw(k, &enc);

    *ref=castToMapper(helper)->posE2A(enc, k);
    return ret;
}

bool ImplementPidControl::getReferences(double *refs)
{
    bool ret;
    ret=iPid->getReferencesRaw(temp);

    castToMapper(helper)->posE2A(temp, refs);
    return ret;
}

bool ImplementPidControl::getErrorLimit(int j, double *ref)
{
    bool ret;
    int k;
    double enc;

    k=castToMapper(helper)->toHw(j);

    ret=iPid->getErrorLimitRaw(k, &enc);

    *ref=castToMapper(helper)->posE2A(enc, k);
    return ret;
}

bool ImplementPidControl::getErrorLimits(double *refs)
{
    bool ret;
    ret=iPid->getErrorLimitsRaw(temp);

    castToMapper(helper)->posE2A(temp, refs);
    return ret;
}

bool ImplementPidControl::resetPid(int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->resetPidRaw(k);
}

bool ImplementPidControl::enablePid(int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->enablePidRaw(k);
}

bool ImplementPidControl::disablePid(int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->disablePidRaw(k);
}

bool ImplementPidControl::setOffset(int j, double v)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->setOffsetRaw(k, v);
}


