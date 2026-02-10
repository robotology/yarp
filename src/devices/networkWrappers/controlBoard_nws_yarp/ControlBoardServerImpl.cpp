/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "ControlBoardServerImpl.h"

/*! \file ControlBoardServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

namespace {
YARP_LOG_COMPONENT(CB_RPC, "yarp.device.controlBoard_nws_yarp")
}

//--------------------------------------
// IControlMode RPC methods

return_getAvailableControlModes ControlBoardRPCd::getAvailableControlModesRPC(const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getAvailableControlModes ret;
    if (!m_iControlMode)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iControlMode->getAvailableControlModes(j, ret.avail);
    if (!ret.ret)
    {
        yCError(CB_RPC, "getAvailableControlModes() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setControlModeOneRPC(const std::int32_t j, const yarp::dev::SelectableControlModeEnum mod)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iControlMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iControlMode->setControlMode(j, (int)(mod));
    if (!ret) {
        yCError(CB_RPC, "setControlMode() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setControlModeAllRPC(const std::vector<yarp::dev::SelectableControlModeEnum>& modes)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iControlMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iControlMode->setControlModes(modes);

    if (!ret) {
        yCError(CB_RPC, "setControlModes() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setControlModeGroupRPC(const std::vector<std::int32_t>& j, const std::vector<yarp::dev::SelectableControlModeEnum>& modes)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iControlMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iControlMode->setControlModes(j, modes);

    if (!ret) {
        yCError(CB_RPC, "setControlModes() failed");
    }
    return ret;
}

//--------------------------------------
// IJointBraked RPC methods

return_isJointBraked ControlBoardRPCd::isJointBrakedRPC(const std::int32_t j) const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_isJointBraked ret;
    if (!m_iJointBrake)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iJointBrake->isJointBraked(j, ret.isBraked);
    if (!ret.ret)
    {
        yCError(CB_RPC, "isJointBraked() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setManualBrakeActiveRPC(const std::int32_t j, const bool active)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iJointBrake) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iJointBrake->setManualBrakeActive(j, active);
    if (!ret) {
        yCError(CB_RPC, "setManualBrakeActive() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setAutoBrakeEnabledRPC(const std::int32_t j, const bool enabled)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iJointBrake) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iJointBrake->setAutoBrakeEnabled(j, enabled);
    if (!ret) {
        yCError(CB_RPC, "setAutoBrakeEnabled() failed");
    }
    return ret;
}

return_getAutoBrakeEnabled ControlBoardRPCd::getAutoBrakeEnabledRPC(const std::int32_t j) const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getAutoBrakeEnabled ret;
    if (!m_iJointBrake) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iJointBrake->getAutoBrakeEnabled(j, ret.enabled);
    if (!ret.ret) {
        yCError(CB_RPC, "getAutoBrakeEnabled() failed");
    }
    return ret;
}

//--------------------------------------
// IVelocityDirect RPC methods

return_getRefVelocityOne ControlBoardRPCd::getRefVelocityOneRPC(const std::int32_t j) const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getRefVelocityOne ret;
    if (!m_iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.ret = m_iVelocityDirect->getRefVelocity(j, ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getRefVelocity() failed");
    }
    return ret;
}

return_getRefVelocityAll ControlBoardRPCd::getRefVelocityAllRPC() const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getRefVelocityAll ret;
    if (!m_iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.ret = m_iVelocityDirect->getRefVelocity(ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getRefVelocity() failed");
    }
    return ret;
}

return_getRefVelocityGroup ControlBoardRPCd::getRefVelocityGroupRPC(const std::vector<std::int32_t>& jnts) const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getRefVelocityGroup ret;
    if (!m_iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.ret = m_iVelocityDirect->getRefVelocity(jnts, ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getRefVelocity() failed");
    }
    return ret;
}

return_getAxes ControlBoardRPCd::getAxesRPC() const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getAxes ret;
    if (!m_iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    size_t axes=0;
    ret.ret = m_iVelocityDirect->getAxes(axes);
    if (!ret.ret) {
        yCError(CB_RPC, "getAxes() failed");
    }
    ret.axes = axes;
    return ret;
}

//--------------------------------------
// IPidControl RPC methods

yarp::dev::ReturnValue ControlBoardRPCd::setPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const yarp::dev::Pid& pid)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->setPid(pidtype, j, pid);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidRPC");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setPidsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<yarp::dev::Pid>& pids)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->setPids(pidtype, pids.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidsRPC");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

return_getAvailablePids ControlBoardRPCd::getAvailablePidsRPC(const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getAvailablePids ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getAvailablePids(j, ret.avail);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getAvailablePidsRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPid ControlBoardRPCd::getPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPid ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getPid(pidtype, j, &ret.pid);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPids ControlBoardRPCd::getPidsRPC(const yarp::dev::PidControlTypeEnum pidtype)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPids ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.pids.resize(m_njoints);
    ret.ret = m_iPidControl->getPids(pidtype, ret.pids.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidsRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidExtraInfo ControlBoardRPCd::getPidExtraInfoRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidExtraInfo ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getPidExtraInfo(pidtype, j, ret.info);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidExtraInfos ControlBoardRPCd::getPidExtraInfosRPC(const yarp::dev::PidControlTypeEnum pidtype)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidExtraInfos ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.info.resize(m_njoints);
    ret.ret = m_iPidControl->getPidExtraInfos(pidtype, ret.info);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidsRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidOffset ControlBoardRPCd::getPidOffsetRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidOffset ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getPidOffset(pidtype, j, ret.offset);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidFeedforward ControlBoardRPCd::getPidFeedforwardRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidFeedforward ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getPidFeedforward(pidtype, j, ret.feedforward);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::enablePidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->enablePid(pidtype, j);
    if (!ret) {
        yCError(CB_RPC, "Unable to enablePid");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::disablePidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->disablePid(pidtype, j);
    if (!ret) {
        yCError(CB_RPC, "Unable to disablePid");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::resetPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->resetPid(pidtype, j);
    if (!ret) {
        yCError(CB_RPC, "Unable to resetPid");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

return_isPidEnabled ControlBoardRPCd::isPidEnabledRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_isPidEnabled ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->isPidEnabled(pidtype, j, ret.isEnabled);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPidErrorLimitRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double lim)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->setPidErrorLimit(pidtype, j, lim);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidErrorLimitRPC");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPidErrorLimitsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<double>& limits)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->setPidErrorLimits(pidtype, limits.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidErrorLimitsRPC");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPidReferenceRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double ref)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->setPidReference(pidtype, j, ref);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidReferenceRPC");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPidReferencesRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<double>& refs)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iPidControl->setPidReferences(pidtype, refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidRPC");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

return_getPidError ControlBoardRPCd::getPidErrorRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidError ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getPidError(pidtype, j, &ret.err);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidErrorRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidErrors ControlBoardRPCd::getPidErrorsRPC(const yarp::dev::PidControlTypeEnum pidtype)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidErrors ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.errs.resize(m_njoints);
    ret.ret = m_iPidControl->getPidErrors(pidtype, ret.errs.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidErrorsRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidReference ControlBoardRPCd::getPidReferenceRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidReference ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getPidReference(pidtype, j, &ret.ref);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidReferenceRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidReferences ControlBoardRPCd::getPidReferencesRPC(const yarp::dev::PidControlTypeEnum pidtype)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidReferences ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.refs.resize(m_njoints);
    ret.ret = m_iPidControl->getPidReferences(pidtype, ret.refs.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidReferencesRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidErrorLimit ControlBoardRPCd::getPidErrorLimitRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidErrorLimit ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getPidErrorLimit(pidtype, j, &ret.lim);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidErrorLimitRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidErrorLimits ControlBoardRPCd::getPidErrorLimitsRPC(const yarp::dev::PidControlTypeEnum pidtype)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidErrorLimits ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.lims.resize(m_njoints);
    ret.ret = m_iPidControl->getPidErrorLimits(pidtype, ret.lims.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidErrorLimitsRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidOutput ControlBoardRPCd::getPidOutputRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidOutput ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iPidControl->getPidReference(pidtype, j, &ret.out);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidOutputRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getPidOutputs ControlBoardRPCd::getPidOutputsRPC(const yarp::dev::PidControlTypeEnum pidtype)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPidOutputs ret;
    if (!m_iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.outs.resize(m_njoints);
    ret.ret = m_iPidControl->getPidReferences(pidtype, ret.outs.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidOutputsRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}


//-------------------


return_getPosLimits ControlBoardRPCd::getPosLimitsRPC(const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getPosLimits ret;
    if (!m_iLimits)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iLimits->getPosLimits(j, &ret.min, &ret.max);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPosLimitsRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

return_getVelLimits ControlBoardRPCd::getVelLimitsRPC(const std::int16_t j)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getVelLimits ret;
    if (!m_iLimits)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_iLimits->getVelLimits(j, &ret.min, &ret.max);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getVelLimitsRPC");
        ret.ret = ReturnValue::return_code::return_value_error_generic;
        return ret;
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPosLimitsRPC(const std::int16_t j, const double min, const double max)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iLimits) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iLimits->setPosLimits(j, min,max);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidReferenceRPC");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setVelLimitsRPC(const std::int16_t j, const double min, const double max)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    ReturnValue ret;
    if (!m_iLimits) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_iLimits->setVelLimits(j, min,max);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidReferenceRPC");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}
