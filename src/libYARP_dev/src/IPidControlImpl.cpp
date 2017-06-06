/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale <lorenzo.natale@iit.it>, Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/IPidControlImpl.h>
#include <yarp/dev/ControlBoardHelper.h>

#include <cmath>

using namespace yarp::dev;

void ImplementPidControl::convert_units_to_machine (const yarp::dev::PidControlTypeEnum& pidtype, double userval, int j, double &machineval, int &k)
{
    switch (pidtype)
    {
        case yarp::dev::VOCAB_PIDTYPE_POSITION:
            castToMapper(helper)->posA2E(userval,j, machineval,k);
        break;
        case yarp::dev::VOCAB_PIDTYPE_VELOCITY:
            castToMapper(helper)->velA2E(userval,j, machineval,k);
        break;
        case yarp::dev::VOCAB_PIDTYPE_TORQUE:
            castToMapper(helper)->trqN2S(userval,j, machineval,k);
        break;
        case yarp::dev::VOCAB_PIDTYPE_CURRENT:
            castToMapper(helper)->ampereA2S(userval,j, machineval,k);
        break;
        default:
            yError() << "convert_units_to_machine: invalid pidtype";
        break;
    }
}

void ImplementPidControl::convert_units_to_machine (const yarp::dev::PidControlTypeEnum& pidtype, const double* userval, double* machineval)
{
    switch (pidtype)
    {
        case yarp::dev::VOCAB_PIDTYPE_POSITION:
            castToMapper(helper)->posA2E(userval, machineval);
        break;
        case yarp::dev::VOCAB_PIDTYPE_VELOCITY:
            castToMapper(helper)->velA2E(userval, machineval);
        break;
        case yarp::dev::VOCAB_PIDTYPE_TORQUE:
            castToMapper(helper)->trqN2S(userval, machineval);
        break;
        case yarp::dev::VOCAB_PIDTYPE_CURRENT:
            castToMapper(helper)->ampereA2S(userval, machineval);
        break;
        default:
            yError() << "convert_units_to_machine: invalid pidtype";
        break;
    }
}

void ImplementPidControl::convert_units_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double machineval, double* userval, int k)
{
    switch (pidtype)
    {
        case yarp::dev::VOCAB_PIDTYPE_POSITION:
            *userval = castToMapper(helper)->posE2A(machineval, k);
        break;
        case yarp::dev::VOCAB_PIDTYPE_VELOCITY:
            *userval = castToMapper(helper)->velE2A(machineval, k);
        break;
        case yarp::dev::VOCAB_PIDTYPE_TORQUE:
            *userval = castToMapper(helper)->trqS2N(machineval,k);
        break;
        case yarp::dev::VOCAB_PIDTYPE_CURRENT:
            *userval = castToMapper(helper)->ampereS2A(machineval,k);
        break;
        default:
            yError() << "convert_units_to_machine: invalid pidtype";
        break;
    }
}

void ImplementPidControl::convert_units_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double* machineval, double* userval)
{
    switch (pidtype)
    {
        case yarp::dev::VOCAB_PIDTYPE_POSITION:
            castToMapper(helper)->posE2A(machineval, userval);
        break;
        case yarp::dev::VOCAB_PIDTYPE_VELOCITY:
            castToMapper(helper)->velE2A(machineval, userval);
        break;
        case yarp::dev::VOCAB_PIDTYPE_TORQUE:
            castToMapper(helper)->trqS2N(machineval,userval);
        break;
        case yarp::dev::VOCAB_PIDTYPE_CURRENT:
            castToMapper(helper)->ampereS2A(machineval,userval);
        break;
        default:
            yError() << "convert_units_to_machine: invalid pidtype";
        break;
    }
}

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

bool ImplementPidControl:: initialize (int size, const int *amap, const double *enc, const double *zos, const double* newtons, const double* amps)
{
    if (helper!=0)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,newtons,amps));
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

bool ImplementPidControl::setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid)
{
    int k=castToMapper(helper)->toHw(j);
    return iPid->setPidRaw(pidtype, k,pid);
}

bool ImplementPidControl::setPids(const PidControlTypeEnum& pidtype,  const Pid *pids)
{
    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        tmp=castToMapper(helper)->toHw(j);
        tmpPids[tmp]=pids[j];
    }

    return iPid->setPidsRaw(pidtype, tmpPids);
}

bool ImplementPidControl::setPidReference(const PidControlTypeEnum& pidtype,  int j, double ref)
{
    int k=0;
    double raw;
    this->convert_units_to_machine(pidtype,ref,j,raw,k);
    return iPid->setPidReferenceRaw(pidtype, k, raw);
}

bool ImplementPidControl::setPidReferences(const PidControlTypeEnum& pidtype,  const double *refs)
{
    this->convert_units_to_machine(pidtype,refs,temp);
    return iPid->setPidReferencesRaw(pidtype, temp);
}

bool ImplementPidControl::setPidErrorLimit(const PidControlTypeEnum& pidtype,  int j, double limit)
{
    int k;
    double raw;
    this->convert_units_to_machine(pidtype,limit,j,raw,k);
    return iPid->setPidErrorLimitRaw(pidtype, k, raw);
}

bool ImplementPidControl::setPidErrorLimits(const PidControlTypeEnum& pidtype,  const double *limits)
{
    this->convert_units_to_machine(pidtype,limits,temp);
    return iPid->setPidErrorLimitsRaw(pidtype, temp);
}


bool ImplementPidControl::getPidError(const PidControlTypeEnum& pidtype, int j, double *err)
{
    int k;
    double raw;
    k=castToMapper(helper)->toHw(j);

    bool ret=iPid->getPidErrorRaw(pidtype, k, &raw);

    this->convert_units_to_user(pidtype,raw,err,k);
    return ret;
}

bool ImplementPidControl::getPidErrors(const PidControlTypeEnum& pidtype,  double *errs)
{
    bool ret;
    ret=iPid->getPidErrorsRaw(pidtype, temp);

    this->convert_units_to_user(pidtype,temp,errs);
    return ret;
}

bool ImplementPidControl::getPidOutput(const PidControlTypeEnum& pidtype,  int j, double *out)
{
    int k;

    k=castToMapper(helper)->toHw(j);

    bool ret=iPid->getPidOutputRaw(pidtype, k, out);

    return ret;
}

bool ImplementPidControl::getPidOutputs(const PidControlTypeEnum& pidtype,  double *outs)
{
    bool ret=iPid->getPidOutputsRaw(pidtype, temp);

    castToMapper(helper)->toUser(temp, outs);

    return ret;
}

bool ImplementPidControl::getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid)
{
    int k;
    k=castToMapper(helper)->toHw(j);

    return iPid->getPidRaw(pidtype, k, pid);
}

bool ImplementPidControl::getPids(const PidControlTypeEnum& pidtype, Pid *pids)
{
    bool ret=iPid->getPidsRaw(pidtype, tmpPids);
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
        pids[castToMapper(helper)->toUser(j)]=tmpPids[j];

    return ret;
}

bool ImplementPidControl::getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    bool ret;
    int k;
    double raw;

    k=castToMapper(helper)->toHw(j);

    ret=iPid->getPidReferenceRaw(pidtype, k, &raw);

    this->convert_units_to_user(pidtype,raw,ref,k);
    return ret;
}

bool ImplementPidControl::getPidReferences(const PidControlTypeEnum& pidtype, double *refs)
{
    bool ret;
    ret=iPid->getPidReferencesRaw(pidtype, temp);

    this->convert_units_to_user(pidtype,temp,refs);
    return ret;
}

bool ImplementPidControl::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    bool ret;
    int k;
    double raw;

    k=castToMapper(helper)->toHw(j);

    ret=iPid->getPidErrorLimitRaw(pidtype, k, &raw);

    this->convert_units_to_user(pidtype,raw,ref,k);
    return ret;
}

bool ImplementPidControl::getPidErrorLimits(const PidControlTypeEnum& pidtype, double *refs)
{
    bool ret;
    ret=iPid->getPidErrorLimitsRaw(pidtype, temp);

    this->convert_units_to_user(pidtype,temp,refs);
    return ret;
}

bool ImplementPidControl::resetPid(const PidControlTypeEnum& pidtype, int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->resetPidRaw(pidtype, k);
}

bool ImplementPidControl::enablePid(const PidControlTypeEnum& pidtype, int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->enablePidRaw(pidtype, k);
}

bool ImplementPidControl::disablePid(const PidControlTypeEnum& pidtype, int j)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->disablePidRaw(pidtype, k);
}

bool ImplementPidControl::setPidOffset(const PidControlTypeEnum& pidtype, int j, double v)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->setPidOffsetRaw(pidtype, k, v);
}

bool ImplementPidControl::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->isPidEnabledRaw(pidtype, k, enabled);
}

