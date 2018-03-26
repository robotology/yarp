/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/IPidControlImpl.h>
#include <yarp/dev/ControlBoardHelper.h>

#include <cmath>
#include <map>

using namespace yarp::dev;

class  yarp::dev::ImplementPidControl::PrivateUnitsHandler
{
public:
    PidUnits * PosPid_units;
    PidUnits* VelPid_units;
    PidUnits* CurPid_units;
    PidUnits* TrqPid_units;
    std::map<PidControlTypeEnum, PidUnits*>   pid_units;

    PrivateUnitsHandler(const PrivateUnitsHandler &) = delete;

    explicit PrivateUnitsHandler(int size)
    {
        PosPid_units = new PidUnits[size];
        yAssert(PosPid_units != nullptr);

        VelPid_units = new PidUnits[size];
        yAssert(VelPid_units != nullptr);

        TrqPid_units = new PidUnits[size];
        yAssert(TrqPid_units != nullptr);

        CurPid_units = new PidUnits[size];
        yAssert(CurPid_units != nullptr);

        pid_units[VOCAB_PIDTYPE_POSITION] = PosPid_units;
        pid_units[VOCAB_PIDTYPE_VELOCITY] = VelPid_units;
        pid_units[VOCAB_PIDTYPE_CURRENT] = CurPid_units;
        pid_units[VOCAB_PIDTYPE_TORQUE] = TrqPid_units;
    }

    ~PrivateUnitsHandler()
    {
        checkAndDestroy(PosPid_units);
        checkAndDestroy(VelPid_units);
        checkAndDestroy(TrqPid_units);
        checkAndDestroy(CurPid_units);
    }
};

void ImplementPidControl::convert_pid_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_raw, int j_raw,  Pid &out_usr, int &k_usr)
{
    double* output_conversion_units = 0;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    k_usr = cb_helper->toUser(j_raw);
    out_usr = in_raw;

    switch (pidtype)
    {
    case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION:
        if (mPriv->pid_units[VOCAB_PIDTYPE_POSITION][k_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_usr.kp = cb_helper->posE2A(out_usr.kp, j_raw);
            out_usr.ki = cb_helper->posE2A(out_usr.ki, j_raw);
            out_usr.kd = cb_helper->posE2A(out_usr.kd, j_raw);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY:
        if (mPriv->pid_units[VOCAB_PIDTYPE_VELOCITY][k_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_usr.kp = cb_helper->velE2A(out_usr.kp, j_raw);
            out_usr.ki = cb_helper->velE2A(out_usr.ki, j_raw);
            out_usr.kd = cb_helper->velE2A(out_usr.kd, j_raw);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE:
        if (mPriv->pid_units[VOCAB_PIDTYPE_TORQUE][k_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_usr.kp = cb_helper->trqS2N(out_usr.kp, j_raw);
            out_usr.ki = cb_helper->trqS2N(out_usr.ki, j_raw);
            out_usr.kd = cb_helper->trqS2N(out_usr.kd, j_raw);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT:
        if (mPriv->pid_units[VOCAB_PIDTYPE_CURRENT][k_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_usr.kp = cb_helper->ampereS2A(out_usr.kp, j_raw);
            out_usr.ki = cb_helper->ampereS2A(out_usr.ki, j_raw);
            out_usr.kd = cb_helper->ampereS2A(out_usr.kd, j_raw);
        }
        break;
    default:
        break;
    }

    get_output_conversion_units(pidtype, j_raw, output_conversion_units);
    out_usr.kp = cb_helper->raw2user(out_usr.kp, j_raw, output_conversion_units);
    out_usr.ki = cb_helper->raw2user(out_usr.ki, j_raw, output_conversion_units);
    out_usr.kd = cb_helper->raw2user(out_usr.kd, j_raw, output_conversion_units);
    out_usr.max_output = cb_helper->raw2user(out_usr.max_output, j_raw, output_conversion_units);
    out_usr.max_int = cb_helper->raw2user(out_usr.max_int, j_raw, output_conversion_units);
    out_usr.stiction_up_val = cb_helper->raw2user(out_usr.stiction_up_val, j_raw, output_conversion_units);
    out_usr.stiction_down_val = cb_helper->raw2user(out_usr.stiction_down_val, j_raw, output_conversion_units);
    out_usr.offset = cb_helper->raw2user(out_usr.offset, j_raw, output_conversion_units);
}

void ImplementPidControl::convert_pid_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_usr, int j_usr,  Pid &out_raw, int &k_raw)
{
    double* output_conversion_units = 0;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    k_raw = castToMapper(helper)->toHw(j_usr);
    out_raw = in_usr;

    switch (pidtype)
    {
    case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION:
        if (mPriv->pid_units[VOCAB_PIDTYPE_POSITION][j_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_raw.kp = castToMapper(helper)->posA2E(out_raw.kp, j_usr);
            out_raw.ki = castToMapper(helper)->posA2E(out_raw.ki, j_usr);
            out_raw.kd = castToMapper(helper)->posA2E(out_raw.kd, j_usr);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY:
        if (mPriv->pid_units[VOCAB_PIDTYPE_VELOCITY][j_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_raw.kp = castToMapper(helper)->velA2E(out_raw.kp, j_usr);
            out_raw.ki = castToMapper(helper)->velA2E(out_raw.ki, j_usr);
            out_raw.kd = castToMapper(helper)->velA2E(out_raw.kd, j_usr);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE:
        if (mPriv->pid_units[VOCAB_PIDTYPE_TORQUE][j_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_raw.kp = castToMapper(helper)->trqN2S(out_raw.kp, j_usr);
            out_raw.ki = castToMapper(helper)->trqN2S(out_raw.ki, j_usr);
            out_raw.kd = castToMapper(helper)->trqN2S(out_raw.kd, j_usr);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT:
        if (mPriv->pid_units[VOCAB_PIDTYPE_CURRENT][j_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_raw.kp = castToMapper(helper)->ampereA2S(out_raw.kp, j_usr);
            out_raw.ki = castToMapper(helper)->ampereA2S(out_raw.ki, j_usr);
            out_raw.kd = castToMapper(helper)->ampereA2S(out_raw.kd, j_usr);
        }
        break;
    default:
        break;
    }

    get_output_conversion_units(pidtype, j_usr, output_conversion_units);
    out_raw.kp = cb_helper->user2raw(out_raw.kp, j_usr, output_conversion_units);
    out_raw.ki = cb_helper->user2raw(out_raw.ki, j_usr, output_conversion_units);
    out_raw.kd = cb_helper->user2raw(out_raw.kd, j_usr, output_conversion_units);
    out_raw.max_output = cb_helper->user2raw(out_raw.max_output, j_usr, output_conversion_units);
    out_raw.max_int = cb_helper->user2raw(out_raw.max_int, j_usr, output_conversion_units);
    out_raw.stiction_up_val = cb_helper->user2raw(out_raw.stiction_up_val, j_usr, output_conversion_units);
    out_raw.stiction_down_val = cb_helper->user2raw(out_raw.stiction_down_val, j_usr, output_conversion_units);
    out_raw.offset = cb_helper->user2raw(out_raw.offset, j_usr, output_conversion_units);
}

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
    helper = nullptr;
    temp=nullptr;
    tmpPids=nullptr;
    mPriv = nullptr;
}

ImplementPidControl::~ImplementPidControl()
{
    uninitialize();
}

bool ImplementPidControl::setConversion(const PidControlTypeEnum& pidtype, const PidFeedbackUnitsEnum fbk_conv_units, const PidOutputUnitsEnum out_conv_units)
{
    int nj = castToMapper(helper)->axes();
    for (size_t i = 0; i < nj; i++)
    {
        mPriv->pid_units[pidtype][i].fbk_units = fbk_conv_units;
        mPriv->pid_units[pidtype][i].out_units = out_conv_units;
    }
    return true;
}

bool ImplementPidControl:: initialize (int size, const int *amap, const double *enc, const double *zos, const double* newtons, const double* amps, const double* dutys)
{
    if (helper!=nullptr)
        return false;

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,newtons,amps,0,dutys));
    yAssert (helper != nullptr);
    temp=new double [size];
    yAssert (temp != nullptr);
    tmpPids=new Pid[size];
    yAssert (tmpPids != nullptr);

    mPriv = new PrivateUnitsHandler(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementPidControl::uninitialize ()
{
    if (helper!=nullptr)
        delete castToMapper(helper);
    helper=nullptr;

    checkAndDestroy(tmpPids);
    checkAndDestroy(temp);

    if (mPriv!= nullptr)
        delete mPriv;
    mPriv = nullptr;

    return true;
}

bool ImplementPidControl::setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid)
{
    Pid pid_machine;
    int k;
    this->convert_pid_to_machine(pidtype, pid, j, pid_machine, k);
    return iPid->setPidRaw(pidtype, k, pid_machine);
}

bool ImplementPidControl::setPids(const PidControlTypeEnum& pidtype,  const Pid *pids)
{
    int tmp=0;
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
    {
        Pid pid_machine;
        int k;
        this->convert_pid_to_machine(pidtype,  pids[j], j, pid_machine, k);
        tmpPids[k] = pid_machine;
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
    bool ret;
    int k_raw;
    double raw;
    k_raw = castToMapper(helper)->toHw(j);
    ret = iPid->getPidOutputRaw(pidtype, k_raw, &raw);

    double* output_conversion_units=0;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    get_output_conversion_units(pidtype, j, output_conversion_units);

    if (ret)
    {
        cb_helper->raw2user(raw, k_raw, *out, j, output_conversion_units);
        return true;
    }
    return false;
}

bool ImplementPidControl::getPidOutputs(const PidControlTypeEnum& pidtype,  double *outs)
{
    int nj = castToMapper(helper)->axes();
    bool ret=iPid->getPidOutputsRaw(pidtype, temp);
    
    for (int j = 0; j < nj; j++)
    {
        double* output_conversion_units=0;
        ControlBoardHelper* cb_helper = castToMapper(helper);
        get_output_conversion_units(pidtype, j, output_conversion_units);

        temp[j] = cb_helper->raw2user(temp[j],j, output_conversion_units);
    }
    castToMapper(helper)->toUser(temp, outs);

    return ret;
}

bool ImplementPidControl::getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid)
{
    int k_raw;
    k_raw=castToMapper(helper)->toHw(j);
    Pid rawPid;
    bool b =iPid->getPidRaw(pidtype, k_raw, &rawPid);
    if (b)
    {
        this->convert_pid_to_user(pidtype, rawPid, k_raw, *pid, j);
        return true;
    }
    return false;
}

bool ImplementPidControl::getPids(const PidControlTypeEnum& pidtype, Pid *pids)
{
    bool ret=iPid->getPidsRaw(pidtype, tmpPids);
    int nj=castToMapper(helper)->axes();

    for (int k_raw = 0; k_raw < nj; k_raw++)
    {
        int j_usr;
        Pid outpid;
        this->convert_pid_to_user(pidtype, tmpPids[k_raw], k_raw, outpid, j_usr);
        pids[j_usr] = outpid;
    }
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

bool ImplementPidControl::setPidOffset(const PidControlTypeEnum& pidtype, int j, double off)
{
    int k = 0;
    double rawoff;
    double* output_conversion_units=0;
    ControlBoardHelper* cb_helper = castToMapper(helper);
    get_output_conversion_units(pidtype,j,output_conversion_units);

    cb_helper->user2raw(off, j, rawoff, k, output_conversion_units);

    return iPid->setPidOffsetRaw(pidtype, k, rawoff);
}

bool ImplementPidControl::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled)
{
    int k=0;
    k=castToMapper(helper)->toHw(j);

    return iPid->isPidEnabledRaw(pidtype, k, enabled);
}

void ImplementPidControl::get_output_conversion_units(const yarp::dev::PidControlTypeEnum& pidtype, int j, double*&  output_conversion_units )
{
    ControlBoardHelper* cb_helper = castToMapper(helper);
    switch (mPriv->pid_units[pidtype][j].out_units)
    {
        case  PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT:  output_conversion_units = cb_helper->dutycycleToPWMs; break;
        case  PidOutputUnitsEnum::CURRENT_METRIC:  output_conversion_units = cb_helper->ampereToSensors; break;
        case  PidOutputUnitsEnum::POSITION_METRIC:  output_conversion_units = cb_helper->angleToEncoders; break;
        case  PidOutputUnitsEnum::VELOCITY_METRIC:  output_conversion_units = cb_helper->angleToEncoders; break;
        case  PidOutputUnitsEnum::TORQUE_METRIC:  output_conversion_units = cb_helper->newtonsToSensors; break;
        default: output_conversion_units = cb_helper->helper_ones; break;
    }
}