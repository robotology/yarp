/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementPidControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cmath>

using namespace yarp::dev;
using namespace yarp::os;
#define JOINTIDCHECK if (j >= castToMapper(helper)->axes()){yError("joint id out of bound"); return false;}

//////////////////// Implement PidControl interface
ImplementPidControl::ImplementPidControl(IPidControlRaw *y):
helper(nullptr),
doubleBuffManager(nullptr),
pidBuffManager(nullptr)
{
    iPid= dynamic_cast<IPidControlRaw *> (y);
}

ImplementPidControl::~ImplementPidControl()
{
    uninitialize();
}

bool ImplementPidControl:: initialize (int size, const int *amap, const double *enc, const double *zos, const double* newtons, const double* amps, const double* dutys)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,newtons,amps,nullptr,dutys));
    yAssert (helper != nullptr);

    doubleBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<double> (size);
    yAssert (doubleBuffManager != nullptr);

    pidBuffManager = new yarp::dev::impl::FixedSizeBuffersManager<Pid> (size, 1);
    yAssert (pidBuffManager != nullptr);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementPidControl::uninitialize ()
{
    if (helper!=nullptr)
    {
        delete castToMapper(helper);
        helper=nullptr;
    }

    if(doubleBuffManager)
    {
        delete doubleBuffManager;
        doubleBuffManager=nullptr;
    }

    if(pidBuffManager)
    {
        delete pidBuffManager;
        pidBuffManager=nullptr;
    }

     return true;
}

bool ImplementPidControl::setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid)
{
    JOINTIDCHECK
    Pid pid_machine;
    int k;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    cb_helper->convert_pid_to_machine(pidtype, pid, j, pid_machine, k);
    return iPid->setPidRaw(pidtype, k, pid_machine);
}

bool ImplementPidControl::setPids(const PidControlTypeEnum& pidtype,  const Pid *pids)
{
    ControlBoardHelper* cb_helper = castToMapper(helper);
    int nj= cb_helper->axes();
    yarp::dev::impl::Buffer<Pid> buffValues = pidBuffManager->getBuffer();
    for(int j=0;j<nj;j++)
    {
        Pid pid_machine;
        int k;
        cb_helper->convert_pid_to_machine(pidtype,  pids[j], j, pid_machine, k);
        buffValues[k] = pid_machine;
    }


    bool ret = iPid->setPidsRaw(pidtype, buffValues.getData());
    pidBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPidControl::setPidReference(const PidControlTypeEnum& pidtype,  int j, double ref)
{
    JOINTIDCHECK
    int k=0;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    cb_helper->convert_pidunits_to_machine(pidtype,ref,j,raw,k);
    return iPid->setPidReferenceRaw(pidtype, k, raw);
}

bool ImplementPidControl::setPidReferences(const PidControlTypeEnum& pidtype,  const double *refs)
{
    ControlBoardHelper* cb_helper = castToMapper(helper);
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    cb_helper->convert_pidunits_to_machine(pidtype,refs,buffValues.getData());
    bool ret = iPid->setPidReferencesRaw(pidtype, buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPidControl::setPidErrorLimit(const PidControlTypeEnum& pidtype,  int j, double limit)
{
    JOINTIDCHECK
    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    cb_helper->convert_pidunits_to_machine(pidtype,limit,j,raw,k);
    return iPid->setPidErrorLimitRaw(pidtype, k, raw);
}

bool ImplementPidControl::setPidErrorLimits(const PidControlTypeEnum& pidtype,  const double *limits)
{
    ControlBoardHelper* cb_helper = castToMapper(helper);
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    cb_helper->convert_pidunits_to_machine(pidtype,limits,buffValues.getData());
    bool ret = iPid->setPidErrorLimitsRaw(pidtype, buffValues.getData());
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}


bool ImplementPidControl::getPidError(const PidControlTypeEnum& pidtype, int j, double *err)
{
    JOINTIDCHECK
    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    k=castToMapper(helper)->toHw(j);

    bool ret=iPid->getPidErrorRaw(pidtype, k, &raw);

    cb_helper->convert_pidunits_to_user(pidtype,raw,err,k);
    return ret;
}

bool ImplementPidControl::getPidErrors(const PidControlTypeEnum& pidtype,  double *errs)
{
    bool ret;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ret=iPid->getPidErrorsRaw(pidtype, buffValues.getData());
    cb_helper->convert_pidunits_to_user(pidtype,buffValues.getData(),errs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPidControl::getPidOutput(const PidControlTypeEnum& pidtype,  int j, double *out)
{
    JOINTIDCHECK
    bool ret;
    int k_raw;
    double raw;
    k_raw = castToMapper(helper)->toHw(j);
    ret = iPid->getPidOutputRaw(pidtype, k_raw, &raw);
    if (ret)
    {
        ControlBoardHelper* cb_helper = castToMapper(helper);
        double output_conversion_units_user2raw = cb_helper->get_pidoutput_conversion_factor_user2raw(pidtype, j);
        *out = raw / output_conversion_units_user2raw;
        return true;
    }
    return false;
}

bool ImplementPidControl::getPidOutputs(const PidControlTypeEnum& pidtype, double *outs)
{
    ControlBoardHelper* cb_helper = castToMapper(helper);
    int nj = cb_helper->axes();
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    bool ret = iPid->getPidOutputsRaw(pidtype, buffValues.getData());
    if (ret)
    {
        castToMapper(cb_helper)->toUser(buffValues.getData(), outs);
        for (int j = 0; j < nj; j++)
        {
            double output_conversion_units_user2raw = cb_helper->get_pidoutput_conversion_factor_user2raw(pidtype, j);
            outs[j] = outs[j] / output_conversion_units_user2raw;
        }
    }
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPidControl::getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid)
{
    JOINTIDCHECK
    ControlBoardHelper* cb_helper = castToMapper(helper);
    int k_raw;
    k_raw=cb_helper->toHw(j);
    Pid rawPid;
    bool ret = iPid->getPidRaw(pidtype, k_raw, &rawPid);
    if (ret)
    {
        cb_helper->convert_pid_to_user(pidtype, rawPid, k_raw, *pid, j);
        return true;
    }
    return false;
}

bool ImplementPidControl::getPids(const PidControlTypeEnum& pidtype, Pid *pids)
{
    yarp::dev::impl::Buffer<Pid> buffValues = pidBuffManager->getBuffer();
    if(!iPid->getPidsRaw(pidtype, buffValues.getData()))
    {
        pidBuffManager->releaseBuffer(buffValues);
        return false;
    }

    ControlBoardHelper* cb_helper = castToMapper(helper);
    int nj=cb_helper->axes();

    for (int k_raw = 0; k_raw < nj; k_raw++)
    {
        int j_usr;
        Pid outpid;
        cb_helper->convert_pid_to_user(pidtype, buffValues[k_raw], k_raw, outpid, j_usr);
        pids[j_usr] = outpid;
    }
    pidBuffManager->releaseBuffer(buffValues);
    return true;
}

bool ImplementPidControl::getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    JOINTIDCHECK
    bool ret;
    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    k=castToMapper(helper)->toHw(j);

    ret=iPid->getPidReferenceRaw(pidtype, k, &raw);

    cb_helper->convert_pidunits_to_user(pidtype,raw,ref,k);
    return ret;
}

bool ImplementPidControl::getPidReferences(const PidControlTypeEnum& pidtype, double *refs)
{
    bool ret;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    yarp::dev::impl::Buffer<double> buffValues = doubleBuffManager->getBuffer();
    ret=iPid->getPidReferencesRaw(pidtype, buffValues.getData());

    cb_helper->convert_pidunits_to_user(pidtype,buffValues.getData(),refs);
    doubleBuffManager->releaseBuffer(buffValues);
    return ret;
}

bool ImplementPidControl::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    JOINTIDCHECK
    bool ret;
    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    k=castToMapper(helper)->toHw(j);

    ret=iPid->getPidErrorLimitRaw(pidtype, k, &raw);

    cb_helper->convert_pidunits_to_user(pidtype,raw,ref,k);
    return ret;
}

bool ImplementPidControl::getPidErrorLimits(const PidControlTypeEnum& pidtype, double *refs)
{
    bool ret;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    yarp::dev::impl::Buffer<double > buffValues = doubleBuffManager->getBuffer();
    ret=iPid->getPidErrorLimitsRaw(pidtype, buffValues.getData());

    cb_helper->convert_pidunits_to_user(pidtype,buffValues.getData(),refs);
    return ret;
}

bool ImplementPidControl::resetPid(const PidControlTypeEnum& pidtype, int j)
{
    JOINTIDCHECK
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->resetPidRaw(pidtype, k);
}

bool ImplementPidControl::enablePid(const PidControlTypeEnum& pidtype, int j)
{
    JOINTIDCHECK
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->enablePidRaw(pidtype, k);
}

bool ImplementPidControl::disablePid(const PidControlTypeEnum& pidtype, int j)
{
    JOINTIDCHECK
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->disablePidRaw(pidtype, k);
}

bool ImplementPidControl::setPidOffset(const PidControlTypeEnum& pidtype, int j, double off)
{
    JOINTIDCHECK
    int k = 0;
    double rawoff;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    double output_conversion_units_user2raw = cb_helper->get_pidoutput_conversion_factor_user2raw(pidtype,j);
    rawoff = off * output_conversion_units_user2raw;
    return iPid->setPidOffsetRaw(pidtype, k, rawoff);
}

bool ImplementPidControl::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled)
{
    JOINTIDCHECK
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->isPidEnabledRaw(pidtype, k, enabled);
}

bool ImplementPidControl::setConversionUnits(const PidControlTypeEnum& pidtype, const PidFeedbackUnitsEnum fbk_conv_units, const PidOutputUnitsEnum out_conv_units)
{
    castToMapper(helper)->set_pid_conversion_units(pidtype, fbk_conv_units, out_conv_units);
    return true;
}
