// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include "yarp/dev/ControlBoardInterfacesImpl.inl" //ControlBoardHelper

#include <stdio.h>
using namespace yarp::dev;

bool StubImplPositionControlRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        fprintf(stderr, "%s:", func);
    else
        fprintf(stderr, "Function ");

    fprintf(stderr, "not yet implemented\n");

    return false;
}

bool StubImplEncodersRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        fprintf(stderr, "%s:", func);
    else
        fprintf(stderr, "Function ");

    fprintf(stderr, "not yet implemented\n");

    return false;
}

bool StubImplPidControlRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        fprintf(stderr, "%s:", func);
    else
        fprintf(stderr, "Function ");

    fprintf(stderr, "not yet implemented\n");

    return false;
}

ImplementTorqueControl::ImplementTorqueControl(ITorqueControlRaw *tq)
{
    iTorqueRaw = tq;
    helper=0;
    temp=0;
}

ImplementTorqueControl::~ImplementTorqueControl()
{
    uninitialize();
}

bool ImplementTorqueControl::initialize(int size, const int *amap, const double *enc, const double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    ACE_ASSERT (helper != 0);
    temp=new double [size];
    ACE_ASSERT (temp != 0);

    tmpPids=new Pid[size];
    ACE_ASSERT (tmpPids!=0);

    return true;
}

bool ImplementTorqueControl::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    checkAndDestroy(temp);
    checkAndDestroy(tmpPids);

    return true;
}

bool ImplementTorqueControl::getAxes(int *axes)
{
    return iTorqueRaw->getAxes(axes);
}

bool ImplementTorqueControl::setTorqueMode()
{
    return iTorqueRaw->setTorqueModeRaw();
}

double ImplementTorqueControl::getRefTorque(int j)
{
    int k;
    k=castToMapper(helper)->toHw(j);

    return iTorqueRaw->getRefTorqueRaw(k);
}

void ImplementTorqueControl::getRefTorques(double *t)
{
    iTorqueRaw->getRefTorquesRaw(temp);
    castToMapper(helper)->toUser(temp,t);
}

bool ImplementTorqueControl::setTorques(const double *t)
{
    castToMapper(helper)->toHw(t, temp);
    return iTorqueRaw->setTorquesRaw(temp);
}

void ImplementTorqueControl::setTorque(int j, double t)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->setTorqueRaw(k, t);
}

bool ImplementTorqueControl::setTorquePid(int j, const Pid &pid)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->setTorquePidRaw(k, pid);
}

bool ImplementTorqueControl::setTorquePids(const Pid *pids)
{
    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toHw(j);
        tmpPids[tmp]=pids[j];
    }
    
    return iTorqueRaw->setTorquePidsRaw(tmpPids);
}

bool ImplementTorqueControl::setTorqueErrorLimit(int j, double limit)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->setTorqueErrorLimitRaw(k, limit);
}

bool ImplementTorqueControl::setTorqueErrorLimits(const double *limits)
{
    castToMapper(helper)->toHw(limits, temp);
    return iTorqueRaw->setTorqueErrorLimitsRaw(temp);
}

bool ImplementTorqueControl::getTorqueError(int j, double *err)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueErrorRaw(k, err);
}

bool ImplementTorqueControl::getTorqueErrors(double *errs)
{
    bool ret=iTorqueRaw->getTorqueErrorsRaw(temp);
    castToMapper(helper)->toUser(temp, errs);
    return ret;
}
    
bool ImplementTorqueControl::getOutput(int j, double *out)
{
    int k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getOutputRaw(k, out);
}

bool ImplementTorqueControl::getOutputs(double *outs)
{
    bool ret=iTorqueRaw->getOutputsRaw(temp);
    castToMapper(helper)->toUser(temp, outs);
    return ret;
}

bool ImplementTorqueControl::getTorquePid(int j, Pid *pid)
{
  int k=castToMapper(helper)->toHw(j);
  return iTorqueRaw->getTorquePidRaw(k, pid);
}

bool ImplementTorqueControl::getTorquePids(Pid *pids)
{
    int ret=iTorqueRaw->getTorquePidsRaw(tmpPids);

    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toUser(j);
        pids[tmp]=tmpPids[j];
    }

    return ret;
}

bool ImplementTorqueControl::getTorqueReference(int j, double *ref)
{
    int k=castToMapper(helper)->toHw(j);    
    return iTorqueRaw->getTorqueReferenceRaw(k, ref);
}

bool ImplementTorqueControl::getTorqueReferences(double *refs)
{
    int ret=iTorqueRaw->getTorqueReferencesRaw(temp);
    castToMapper(helper)->toUser(temp, refs);
    return ret;
}

bool ImplementTorqueControl::getTorqueErrorLimits(double *limits)
{
    int ret=iTorqueRaw->getTorqueErrorLimitsRaw(temp);
    castToMapper(helper)->toUser(temp, limits);
    return ret;
}

bool ImplementTorqueControl::resetTorquePid(int j)
{
    int k=castToMapper(helper)->toHw(j);    
    return iTorqueRaw->resetTorquePidRaw(k);
}

bool ImplementTorqueControl::disableTorquePid(int j)
{
    int k=castToMapper(helper)->toHw(j);    
    return iTorqueRaw->disableTorquePidRaw(k);
}

bool ImplementTorqueControl::enableTorquePid(int j)
{
    int k=castToMapper(helper)->toHw(j);    
    return iTorqueRaw->enableTorquePidRaw(k);
}
bool ImplementTorqueControl::setTorqueOffset(int j, double v)
{
    int k=castToMapper(helper)->toHw(j);    
    return iTorqueRaw->setTorqueOffsetRaw(k, v);
}
