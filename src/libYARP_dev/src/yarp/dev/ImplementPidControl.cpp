/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementPidControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cmath>

using namespace yarp::dev;
using namespace yarp::os;

//////////////////// Implement PidControl interface
ImplementPidControl::ImplementPidControl(IPidControlRaw *y):
m_helper(nullptr)
{
    m_iraw= dynamic_cast<IPidControlRaw *> (y);
}

ImplementPidControl::~ImplementPidControl()
{
    uninitialize();
}

bool ImplementPidControl:: initialize (int size, const int *amap, const double *enc, const double *zos, const double* newtons, const double* amps, const double* dutys)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,newtons,amps,nullptr,dutys));
    yAssert (m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);
    m_buffer_pids.resize(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementPidControl::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

     return true;
}

ReturnValue ImplementPidControl::getAvailablePids(int j, std::vector<PidControlTypeEnum>& avail)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    Pid pid_machine;
    int k=castToMapper(m_helper)->toHw(j);
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    return m_iraw->getAvailablePidsRaw(k, avail);
}

ReturnValue ImplementPidControl::setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    Pid pid_machine;
    int k;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    cb_helper->convert_pid_to_machine(pidtype, pid, j, pid_machine, k);
    return m_iraw->setPidRaw(pidtype, k, pid_machine);
}

ReturnValue ImplementPidControl::setPids(const PidControlTypeEnum& pidtype,  const Pid *pids)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(pids)

    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    int nj= cb_helper->axes();

    for(int j=0;j<nj;j++)
    {
        Pid pid_machine;
        int k;
        cb_helper->convert_pid_to_machine(pidtype,  pids[j], j, pid_machine, k);
        m_buffer_pids[k] = pid_machine;
    }

    auto ret = m_iraw->setPidsRaw(pidtype, m_buffer_pids.data());

    return ret;
}

ReturnValue ImplementPidControl::setPidReference(const PidControlTypeEnum& pidtype,  int j, double ref)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=0;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    cb_helper->convert_pidunits_to_machine(pidtype,ref,j,raw,k);
    return m_iraw->setPidReferenceRaw(pidtype, k, raw);
}

ReturnValue ImplementPidControl::setPidReferences(const PidControlTypeEnum& pidtype,  const double *refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(refs)

    ControlBoardHelper* cb_helper = castToMapper(m_helper);

    cb_helper->convert_pidunits_to_machine(pidtype,refs,m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setPidReferencesRaw(pidtype, m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPidControl::setPidErrorLimit(const PidControlTypeEnum& pidtype,  int j, double limit)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    cb_helper->convert_pidunits_to_machine(pidtype,limit,j,raw,k);
    return m_iraw->setPidErrorLimitRaw(pidtype, k, raw);
}

ReturnValue ImplementPidControl::setPidErrorLimits(const PidControlTypeEnum& pidtype,  const double *limits)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(limits)

    ControlBoardHelper* cb_helper = castToMapper(m_helper);

    cb_helper->convert_pidunits_to_machine(pidtype,limits,m_buffer_doubles.data());
    ReturnValue ret = m_iraw->setPidErrorLimitsRaw(pidtype, m_buffer_doubles.data());

    return ret;
}

ReturnValue ImplementPidControl::getPidError(const PidControlTypeEnum& pidtype, int j, double *err)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(err)

    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    k=castToMapper(m_helper)->toHw(j);

    ReturnValue ret=m_iraw->getPidErrorRaw(pidtype, k, &raw);

    cb_helper->convert_pidunits_to_user(pidtype,raw,err,k);
    return ret;
}

ReturnValue ImplementPidControl::getPidErrors(const PidControlTypeEnum& pidtype,  double *errs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(errs)

    ReturnValue ret;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);

    ret=m_iraw->getPidErrorsRaw(pidtype, m_buffer_doubles.data());
    cb_helper->convert_pidunits_to_user(pidtype,m_buffer_doubles.data(),errs);

    return ret;
}

ReturnValue ImplementPidControl::getPidOutput(const PidControlTypeEnum& pidtype,  int j, double *out)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(out)

    ReturnValue ret;
    int k_raw;
    double raw;
    k_raw = castToMapper(m_helper)->toHw(j);
    ret = m_iraw->getPidOutputRaw(pidtype, k_raw, &raw);
    if (ret)
    {
        ControlBoardHelper* cb_helper = castToMapper(m_helper);
        double output_conversion_units_user2raw = cb_helper->get_pidoutput_conversion_factor_user2raw(pidtype, j);
        *out = raw / output_conversion_units_user2raw;
        return ret;
    }
    return ret;
}

ReturnValue ImplementPidControl::getPidOutputs(const PidControlTypeEnum& pidtype, double *outs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(outs)

    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    int nj = cb_helper->axes();

    ReturnValue ret = m_iraw->getPidOutputsRaw(pidtype, m_buffer_doubles.data());
    if (ret)
    {
        castToMapper(cb_helper)->toUser(m_buffer_doubles.data(), outs);
        for (int j = 0; j < nj; j++)
        {
            double output_conversion_units_user2raw = cb_helper->get_pidoutput_conversion_factor_user2raw(pidtype, j);
            outs[j] = outs[j] / output_conversion_units_user2raw;
        }
    }

    return ret;
}

ReturnValue ImplementPidControl::getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(pid)

    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    int k_raw;
    k_raw=cb_helper->toHw(j);
    Pid rawPid;
    auto ret = m_iraw->getPidRaw(pidtype, k_raw, &rawPid);
    if (ret)
    {
        cb_helper->convert_pid_to_user(pidtype, rawPid, k_raw, *pid, j);
        return ret;
    }
    return ret;
}

ReturnValue ImplementPidControl::getPids(const PidControlTypeEnum& pidtype, Pid *pids)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(pids)

    auto ret = m_iraw->getPidsRaw(pidtype, m_buffer_pids.data());
    if(!ret)
    {
        return ret;
    }

    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    int nj=cb_helper->axes();

    for (int k_raw = 0; k_raw < nj; k_raw++)
    {
        int j_usr;
        Pid outpid;
        cb_helper->convert_pid_to_user(pidtype, m_buffer_pids[k_raw], k_raw, outpid, j_usr);
        pids[j_usr] = outpid;
    }

    return ret;
}

ReturnValue ImplementPidControl::getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(ref)

    ReturnValue ret;
    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    k=castToMapper(m_helper)->toHw(j);

    ret=m_iraw->getPidReferenceRaw(pidtype, k, &raw);

    cb_helper->convert_pidunits_to_user(pidtype,raw,ref,k);
    return ret;
}

ReturnValue ImplementPidControl::getPidReferences(const PidControlTypeEnum& pidtype, double *refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(refs)

    ReturnValue ret;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);

    ret=m_iraw->getPidReferencesRaw(pidtype, m_buffer_doubles.data());

    cb_helper->convert_pidunits_to_user(pidtype,m_buffer_doubles.data(),refs);

    return ret;
}

ReturnValue ImplementPidControl::getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(ref)

    ReturnValue ret;
    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    k=castToMapper(m_helper)->toHw(j);

    ret=m_iraw->getPidErrorLimitRaw(pidtype, k, &raw);

    cb_helper->convert_pidunits_to_user(pidtype,raw,ref,k);
    return ret;
}

ReturnValue ImplementPidControl::getPidErrorLimits(const PidControlTypeEnum& pidtype, double *refs)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(refs)

    ReturnValue ret;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);

    ret=m_iraw->getPidErrorLimitsRaw(pidtype, m_buffer_doubles.data());

    cb_helper->convert_pidunits_to_user(pidtype,m_buffer_doubles.data(),refs);
    return ret;
}

ReturnValue ImplementPidControl::resetPid(const PidControlTypeEnum& pidtype, int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=0;
    k=castToMapper(m_helper)->toHw(j);

    return m_iraw->resetPidRaw(pidtype, k);
}

ReturnValue ImplementPidControl::enablePid(const PidControlTypeEnum& pidtype, int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=0;
    k=castToMapper(m_helper)->toHw(j);

    return m_iraw->enablePidRaw(pidtype, k);
}

ReturnValue ImplementPidControl::disablePid(const PidControlTypeEnum& pidtype, int j)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=0;
    k=castToMapper(m_helper)->toHw(j);

    return m_iraw->disablePidRaw(pidtype, k);
}

ReturnValue ImplementPidControl::setPidOffset(const PidControlTypeEnum& pidtype, int j, double off)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k = 0;
    k=castToMapper(m_helper)->toHw(j);
    double rawoff;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    double output_conversion_units_user2raw = cb_helper->get_pidoutput_conversion_factor_user2raw(pidtype,j);
    rawoff = off * output_conversion_units_user2raw;
    return m_iraw->setPidOffsetRaw(pidtype, k, rawoff);
}

ReturnValue ImplementPidControl::setPidFeedforward(const PidControlTypeEnum& pidtype, int j, double off)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k = 0;
    k=castToMapper(m_helper)->toHw(j);
    double rawoff;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    double output_conversion_units_user2raw = cb_helper->get_pidoutput_conversion_factor_user2raw(pidtype,j);
    rawoff = off * output_conversion_units_user2raw;
    return m_iraw->setPidFeedforwardRaw(pidtype, k, rawoff);
}

ReturnValue ImplementPidControl::getPidOffset(const PidControlTypeEnum& pidtype, int j, double& off)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    ReturnValue ret;
    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    k=castToMapper(m_helper)->toHw(j);

    ret=m_iraw->getPidOffsetRaw(pidtype, k, raw);

    cb_helper->convert_pidunits_to_user(pidtype,raw,&off,k);
    return ret;
}

ReturnValue ImplementPidControl::getPidFeedforward(const PidControlTypeEnum& pidtype, int j, double& ffd)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    ReturnValue ret;
    int k;
    double raw;
    ControlBoardHelper* cb_helper = castToMapper(m_helper);
    k=castToMapper(m_helper)->toHw(j);

    ret=m_iraw->getPidFeedforwardRaw(pidtype, k, raw);

    cb_helper->convert_pidunits_to_user(pidtype,raw,&ffd,k);
    return ret;
}

ReturnValue ImplementPidControl::isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool& enabled)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=0;
    k=castToMapper(m_helper)->toHw(j);

    return m_iraw->isPidEnabledRaw(pidtype, k, enabled);
}

bool ImplementPidControl::setConversionUnits(const PidControlTypeEnum& pidtype, const PidFeedbackUnitsEnum fbk_conv_units, const PidOutputUnitsEnum out_conv_units)
{
    std::lock_guard lock(m_imp_mutex);

    castToMapper(m_helper)->set_pid_conversion_units(pidtype, fbk_conv_units, out_conv_units);
    return true;
}

yarp::dev::ReturnValue ImplementPidControl::getPidExtraInfo(const PidControlTypeEnum& pidtype, int j, yarp::dev::PidExtraInfo& units)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=0;
    k=castToMapper(m_helper)->toHw(j);
    return m_iraw->getPidExtraInfoRaw(pidtype, k, units);
}

yarp::dev::ReturnValue ImplementPidControl::getPidExtraInfos(const PidControlTypeEnum& pidtype, std::vector<yarp::dev::PidExtraInfo>& units)
{
    std::lock_guard lock(m_imp_mutex);
    VECCHECK_GET_ALL(units);

    ReturnValue ret;
    std::vector<yarp::dev::PidExtraInfo> tmp_infos;
    tmp_infos.resize(units.size());
    ret = m_iraw->getPidExtraInfosRaw(pidtype, tmp_infos);

    for (size_t i = 0; i < tmp_infos.size(); i++)
    {
        int o = castToMapper(m_helper)->toHw(i);
        units[o] = tmp_infos[i];
    }

    return ret;
}
