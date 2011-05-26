// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

ImplementControlMode::ImplementControlMode(IControlModeRaw *r)
{
    raw=r;
    helper=0;
}

bool ImplementControlMode::initialize(int size, const int *amap)
{
    if (helper!=0)
        return false;
    
    double *dummy=new double [size];
    for(int k=0;k<size;k++)
        dummy[k]=0;

    helper=(void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy));
    _YARP_ASSERT (helper != 0);

    delete [] dummy;
    return true;
}

ImplementControlMode::~ImplementControlMode()
{
    uninitialize();
}

bool ImplementControlMode::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
 
    return true;
}

bool ImplementControlMode::setPositionMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setPositionModeRaw(k);
}

bool ImplementControlMode::setVelocityMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setVelocityModeRaw(k);
}

bool ImplementControlMode::setTorqueMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setTorqueModeRaw(k);
}

bool ImplementControlMode::setOpenLoopMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setOpenLoopModeRaw(k);
}

bool ImplementControlMode::setImpedancePositionMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setImpedancePositionModeRaw(k);
}

bool ImplementControlMode::setImpedanceVelocityMode(int j)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->setImpedanceVelocityModeRaw(k);
}

bool ImplementControlMode::getControlMode(int j, int *f)
{
    int k=castToMapper(helper)->toHw(j);
    return raw->getControlModeRaw(k, f);
}

bool ImplementControlMode::getControlModes(int *modes)
{
	int nj = castToMapper(helper)->axes();
    int *tmp=new int [nj];
    bool ret=raw->getControlModesRaw(tmp);
    castToMapper(helper)->toUser(tmp, modes);
	delete [] tmp;
    return ret;
}

ImplementTorqueControl::ImplementTorqueControl(ITorqueControlRaw *tq)
{
    iTorqueRaw = tq;
    helper=0;
    temp=0;
	temp2=0;
    tmpPids=0;
}

ImplementTorqueControl::~ImplementTorqueControl()
{
    uninitialize();
}

bool ImplementTorqueControl::initialize(int size, const int *amap, const double *enc, const double *zos, const double *nw)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos, nw));
    _YARP_ASSERT (helper != 0);
    temp=new double [size];
    _YARP_ASSERT (temp != 0);
    temp2=new double [size];
    _YARP_ASSERT (temp2 != 0);
    tmpPids=new Pid[size];
    _YARP_ASSERT (tmpPids!=0);

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
	checkAndDestroy(temp2);
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

bool ImplementTorqueControl::getRefTorque(int j, double *r)
{
    int k;
	bool ret;
	double torque;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->getRefTorqueRaw(k, &torque);
	*r=castToMapper(helper)->trqS2N(torque, k);
	return ret;
}

bool ImplementTorqueControl::getRefTorques(double *t)
{
    bool ret;
	ret = iTorqueRaw->getRefTorquesRaw(temp);
	castToMapper(helper)->trqS2N(temp,t);
	return ret;
}

bool ImplementTorqueControl::setRefTorques(const double *t)
{
    castToMapper(helper)->trqN2S(t, temp);
    return iTorqueRaw->setRefTorquesRaw(temp);
}

bool ImplementTorqueControl::setRefTorque(int j, double t)
{
    int k;
	double sens;
	castToMapper(helper)->trqN2S(t,j,sens,k);
    return iTorqueRaw->setRefTorqueRaw(k, sens);
}

bool ImplementTorqueControl::getTorques(double *t)
{
    bool ret = iTorqueRaw->getTorquesRaw(temp);
    castToMapper(helper)->toUser(temp, t);
    return ret;
}

bool ImplementTorqueControl::getTorque(int j, double *t)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueRaw(k, t);
}

bool ImplementTorqueControl::getTorqueRanges(double *min, double *max)
{
    bool ret = iTorqueRaw->getTorqueRangesRaw(temp,temp2);
    castToMapper(helper)->toUser(temp, min);
	castToMapper(helper)->toUser(temp2, max);
    return ret;
}

bool ImplementTorqueControl::getTorqueRange(int j, double *min, double *max)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueRangeRaw(k, min, max);
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

bool ImplementTorqueControl::getTorqueErrorLimit(int j, double *limit)
{
	int k;
    k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorqueErrorLimitRaw(k, limit);
}

bool ImplementTorqueControl::setTorqueErrorLimits(const double *limits)
{
    castToMapper(helper)->toHw(limits, temp);
    return iTorqueRaw->setTorqueErrorLimitsRaw(temp);
}

bool ImplementTorqueControl::getTorqueError(int j, double *err)
{
    int k;
	bool ret;
	double temp;
    k=castToMapper(helper)->toHw(j);
    ret = iTorqueRaw->getTorqueErrorRaw(k, &temp);
	*err=castToMapper(helper)->trqS2N(temp, k);
	return ret;
}

bool ImplementTorqueControl::getTorqueErrors(double *errs)
{
    bool ret;
	ret = iTorqueRaw->getTorqueErrorsRaw(temp);
	castToMapper(helper)->trqS2N(temp, errs);
	return ret;
}
    
bool ImplementTorqueControl::getTorquePidOutput(int j, double *out)
{
    int k=castToMapper(helper)->toHw(j);
    return iTorqueRaw->getTorquePidOutputRaw(k, out);
}

bool ImplementTorqueControl::getTorquePidOutputs(double *outs)
{
    bool ret=iTorqueRaw->getTorquePidOutputsRaw(temp);
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
    bool ret=iTorqueRaw->getTorquePidsRaw(tmpPids);

    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toUser(j);
        pids[tmp]=tmpPids[j];
    }

    return ret;
}

bool ImplementTorqueControl::getTorqueErrorLimits(double *limits)
{
    bool ret=iTorqueRaw->getTorqueErrorLimitsRaw(temp);
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
    _YARP_ASSERT (helper != 0);

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

//DEPRECATED
bool ImplementImpedanceControl::setImpedance(int j, double stiffness, double damping, double offset)
{
    int k;
	double stiff;
	double damp;
	castToMapper(helper)->impN2S(stiffness,j,stiff,k);
	castToMapper(helper)->impN2S(damping,j,damp,k);
    return iImpedanceRaw->setImpedanceRaw(k, stiff, damp);
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

//DEPRECATED
bool ImplementImpedanceControl::getImpedance(int j, double *stiffness, double *damping, double *offset)
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
/////////////// implement ImplementOpenLoopControl
ImplementOpenLoopControl::ImplementOpenLoopControl(IOpenLoopControlRaw *r)
{
    raw=r;
    helper=0;
}

bool ImplementOpenLoopControl::initialize(int size, const int *amap)
{
    if (helper!=0)
        return false;
    
    double *dummy=new double [size];
    for(int k=0;k<size;k++)
        dummy[k]=0;

    helper=(void *)(new ControlBoardHelper(size, amap, dummy, dummy, dummy));
    _YARP_ASSERT(helper != 0);

    delete [] dummy;
    return true;
}

ImplementOpenLoopControl::~ImplementOpenLoopControl()
{
    uninitialize();
}

bool ImplementOpenLoopControl::uninitialize ()
{
    if (helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
 
    return true;
}

bool ImplementOpenLoopControl::setOutput(int j, double v)
{
    int k=castToMapper(helper)->toHw(j);

    return raw->setOutputRaw(k, v);
}

bool ImplementOpenLoopControl::setOutputs(const double *v)
{
    castToMapper(helper)->toHw(v, dummy);

    return raw->setOutputsRaw(dummy);
}

bool ImplementOpenLoopControl::setOpenLoopMode()
{
    return raw->setOpenLoopModeRaw();
}

bool ImplementOpenLoopControl::getOutputs(double *v)
{
    bool ret=raw->getOutputsRaw(dummy);

    castToMapper(helper)->toUser(dummy, v);
    return ret;
}

bool ImplementOpenLoopControl::getOutput(int j, double *v)
{
    int k=castToMapper(helper)->toHw(j);

    return raw->getOutputRaw(k, v);
}

