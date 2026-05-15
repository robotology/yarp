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

#if 1
#define LOCKMUTEXSERVER std::lock_guard<std::mutex> lg(m_mutex);
#else
#define LOCKMUTEXSERVER
#endif

//--------------------------------------
// IControlMode RPC methods

return_getAvailableControlModes ControlBoardRPCd::getAvailableControlModesRPC(const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getAvailableControlModes ret;
    if (!m_allInterfaces.iControlMode)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iControlMode->getAvailableControlModes(j, ret.avail);
    if (!ret.ret)
    {
        yCError(CB_RPC, "getAvailableControlModes() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setControlModeOneRPC(const std::int32_t j, const yarp::dev::SelectableControlModeEnum mod)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlMode->setControlMode(j, mod);
    if (!ret) {
        yCError(CB_RPC, "setControlMode() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setControlModeAllRPC(const std::vector<yarp::dev::SelectableControlModeEnum>& modes)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlMode->setControlModes(modes);

    if (!ret) {
        yCError(CB_RPC, "setControlModes() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setControlModeGroupRPC(const std::vector<std::int32_t>& j, const std::vector<yarp::dev::SelectableControlModeEnum>& modes)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlMode->setControlModes(j, modes);

    if (!ret) {
        yCError(CB_RPC, "setControlModes() failed");
    }
    return ret;
}

//--------------------------------------
// IJointBraked RPC methods

return_isJointBraked ControlBoardRPCd::isJointBrakedRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_isJointBraked ret;
    if (!m_allInterfaces.iJointBrake)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iJointBrake->isJointBraked(j, ret.isBraked);
    if (!ret.ret)
    {
        yCError(CB_RPC, "isJointBraked() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setManualBrakeActiveRPC(const std::int32_t j, const bool active)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iJointBrake) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iJointBrake->setManualBrakeActive(j, active);
    if (!ret) {
        yCError(CB_RPC, "setManualBrakeActive() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setAutoBrakeEnabledRPC(const std::int32_t j, const bool enabled)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iJointBrake) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iJointBrake->setAutoBrakeEnabled(j, enabled);
    if (!ret) {
        yCError(CB_RPC, "setAutoBrakeEnabled() failed");
    }
    return ret;
}

return_getAutoBrakeEnabled ControlBoardRPCd::getAutoBrakeEnabledRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getAutoBrakeEnabled ret;
    if (!m_allInterfaces.iJointBrake) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iJointBrake->getAutoBrakeEnabled(j, ret.enabled);
    if (!ret.ret) {
        yCError(CB_RPC, "getAutoBrakeEnabled() failed");
    }
    return ret;
}

//--------------------------------------
// IVelocityDirect RPC methods

return_getRefVelocityOne ControlBoardRPCd::getRefVelocityOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getRefVelocityOne ret;
    if (!m_allInterfaces.iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.ret = m_allInterfaces.iVelocityDirect->getRefVelocity(j, ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getRefVelocity() failed");
    }
    return ret;
}

return_getRefVelocityAll ControlBoardRPCd::getRefVelocityAllRPC() const
{
    LOCKMUTEXSERVER
    return_getRefVelocityAll ret;
    if (!m_allInterfaces.iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.ret = m_allInterfaces.iVelocityDirect->getRefVelocity(ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getRefVelocity() failed");
    }
    return ret;
}

return_getRefVelocityGroup ControlBoardRPCd::getRefVelocityGroupRPC(const std::vector<std::int32_t>& jnts) const
{
    LOCKMUTEXSERVER
    return_getRefVelocityGroup ret;
    if (!m_allInterfaces.iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.vel.resize(jnts.size());
    ret.ret = m_allInterfaces.iVelocityDirect->getRefVelocity(jnts, ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getRefVelocity() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setRefVelocityOneRPC(const std::int32_t j, const double vel)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iVelocityDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iVelocityDirect->setRefVelocity(j, vel);
    if (!ret) {
        yCError(CB_RPC, "setRefVelocity() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setRefVelocityAllRPC(const std::vector<double>& vel)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iVelocityDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iVelocityDirect->setRefVelocity(vel);
    if (!ret) {
        yCError(CB_RPC, "setRefVelocity() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setRefVelocityGroupRPC(const std::vector<std::int32_t>& j, const std::vector<double>& vel)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iVelocityDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iVelocityDirect->setRefVelocity(j, vel);
    if (!ret) {
        yCError(CB_RPC, "setRefVelocity() failed");
    }
    return ret;
}

return_getAxes ControlBoardRPCd::getAxesRPC() const
{
    LOCKMUTEXSERVER
    return_getAxes ret;
    if (!m_allInterfaces.iAxisInfo) {
        yCError(CB_RPC, "Invalid iAxisInfo interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    int axes=0;
    ret.ret = m_allInterfaces.iAxisInfo->getAxes(&axes);
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
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->setPid(pidtype, j, pid);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidRPC");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setPidsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<yarp::dev::Pid>& pids)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->setPids(pidtype, pids.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidsRPC");
    }
    return ret;
}

return_getAvailablePids ControlBoardRPCd::getAvailablePidsRPC(const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getAvailablePids ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getAvailablePids(j, ret.avail);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getAvailablePidsRPC");
    }
    return ret;
}

return_getPid ControlBoardRPCd::getPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPid ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getPid(pidtype, j, &ret.pid);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
    }
    return ret;
}

return_getPids ControlBoardRPCd::getPidsRPC(const yarp::dev::PidControlTypeEnum pidtype) const
{
    LOCKMUTEXSERVER
    return_getPids ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.pids.resize(m_njoints);
    ret.ret = m_allInterfaces.iPidControl->getPids(pidtype, ret.pids.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidsRPC");
    }
    return ret;
}

return_getPidExtraInfo ControlBoardRPCd::getPidExtraInfoRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPidExtraInfo ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getPidExtraInfo(pidtype, j, ret.info);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
    }
    return ret;
}

return_getPidExtraInfos ControlBoardRPCd::getPidExtraInfosRPC(const yarp::dev::PidControlTypeEnum pidtype) const
{
    LOCKMUTEXSERVER
    return_getPidExtraInfos ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.info.resize(m_njoints);
    ret.ret = m_allInterfaces.iPidControl->getPidExtraInfos(pidtype, ret.info);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidsRPC");
    }
    return ret;
}

return_getPidOffset ControlBoardRPCd::getPidOffsetRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPidOffset ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getPidOffset(pidtype, j, ret.offset);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
    }
    return ret;
}

return_getPidFeedforward ControlBoardRPCd::getPidFeedforwardRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPidFeedforward ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getPidFeedforward(pidtype, j, ret.feedforward);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::enablePidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->enablePid(pidtype, j);
    if (!ret) {
        yCError(CB_RPC, "Unable to enablePid");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::disablePidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->disablePid(pidtype, j);
    if (!ret) {
        yCError(CB_RPC, "Unable to disablePid");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::resetPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->resetPid(pidtype, j);
    if (!ret) {
        yCError(CB_RPC, "Unable to resetPid");
    }
    return ret;
}

return_isPidEnabled ControlBoardRPCd::isPidEnabledRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_isPidEnabled ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->isPidEnabled(pidtype, j, ret.isEnabled);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidRPC");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPidErrorLimitRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double lim)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->setPidErrorLimit(pidtype, j, lim);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidErrorLimitRPC");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPidErrorLimitsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<double>& limits)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->setPidErrorLimits(pidtype, limits.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidErrorLimitsRPC");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setPidOffsetOneRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double value)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->setPidOffset(pidtype, j, value);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidOffset");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setPidFeedforwardOneRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double value)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->setPidFeedforward(pidtype, j, value);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidFeedforward");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPidReferenceRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double ref)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->setPidReference(pidtype, j, ref);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidReferenceRPC");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPidReferencesRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPidControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPidControl->setPidReferences(pidtype, refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidRPC");
    }
    return ret;
}

return_getPidError ControlBoardRPCd::getPidErrorRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPidError ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getPidError(pidtype, j, &ret.err);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidErrorRPC");
    }
    return ret;
}

return_getPidErrors ControlBoardRPCd::getPidErrorsRPC(const yarp::dev::PidControlTypeEnum pidtype) const
{
    LOCKMUTEXSERVER
    return_getPidErrors ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.errs.resize(m_njoints);
    ret.ret = m_allInterfaces.iPidControl->getPidErrors(pidtype, ret.errs.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidErrorsRPC");
    }
    return ret;
}

return_getPidReference ControlBoardRPCd::getPidReferenceRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPidReference ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getPidReference(pidtype, j, &ret.ref);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidReferenceRPC");
    }
    return ret;
}

return_getPidReferences ControlBoardRPCd::getPidReferencesRPC(const yarp::dev::PidControlTypeEnum pidtype) const
{
    LOCKMUTEXSERVER
    return_getPidReferences ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.refs.resize(m_njoints);
    ret.ret = m_allInterfaces.iPidControl->getPidReferences(pidtype, ret.refs.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidReferencesRPC");
    }
    return ret;
}

return_getPidErrorLimit ControlBoardRPCd::getPidErrorLimitRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPidErrorLimit ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getPidErrorLimit(pidtype, j, &ret.lim);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidErrorLimitRPC");
    }
    return ret;
}

return_getPidErrorLimits ControlBoardRPCd::getPidErrorLimitsRPC(const yarp::dev::PidControlTypeEnum pidtype) const
{
    LOCKMUTEXSERVER
    return_getPidErrorLimits ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.lims.resize(m_njoints);
    ret.ret = m_allInterfaces.iPidControl->getPidErrorLimits(pidtype, ret.lims.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidErrorLimitsRPC");
    }
    return ret;
}

return_getPidOutput ControlBoardRPCd::getPidOutputRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPidOutput ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPidControl->getPidReference(pidtype, j, &ret.out);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidOutputRPC");
    }
    return ret;
}

return_getPidOutputs ControlBoardRPCd::getPidOutputsRPC(const yarp::dev::PidControlTypeEnum pidtype) const
{
    LOCKMUTEXSERVER
    return_getPidOutputs ret;
    if (!m_allInterfaces.iPidControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.outs.resize(m_njoints);
    ret.ret = m_allInterfaces.iPidControl->getPidReferences(pidtype, ret.outs.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPidOutputsRPC");
    }
    return ret;
}


//-------------------


return_getPosLimits ControlBoardRPCd::getPosLimitsRPC(const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getPosLimits ret;
    if (!m_allInterfaces.iControlLimits)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iControlLimits->getPosLimits(j, &ret.min, &ret.max);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPosLimitsRPC");
    }
    return ret;
}

return_getVelLimits ControlBoardRPCd::getVelLimitsRPC(const std::int16_t j) const
{
    LOCKMUTEXSERVER
    return_getVelLimits ret;
    if (!m_allInterfaces.iControlLimits)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iControlLimits->getVelLimits(j, &ret.min, &ret.max);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getVelLimitsRPC");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPosLimitsRPC(const std::int16_t j, const double min, const double max)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlLimits) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlLimits->setPosLimits(j, min,max);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidReferenceRPC");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setVelLimitsRPC(const std::int16_t j, const double min, const double max)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlLimits) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlLimits->setVelLimits(j, min,max);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPidReferenceRPC");
    }
    return ret;
}

//------- IImpedanceControl

return_getImpedance ControlBoardRPCd::getImpedanceRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getImpedance ret;
    if (!m_allInterfaces.iImpedanceControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iImpedanceControl->getImpedance(j, &ret.stiffness, &ret.damping);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getImpedance");
    }
    return ret;
}

return_getImpedanceOffset ControlBoardRPCd::getImpedanceOffsetRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getImpedanceOffset ret;
    if (!m_allInterfaces.iImpedanceControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iImpedanceControl->getImpedanceOffset(j, &ret.offset);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getImpedanceOffset");
    }
    return ret;
}

return_getCurrentImpedanceLimit ControlBoardRPCd::getCurrentImpedanceLimitRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getCurrentImpedanceLimit ret;
    if (!m_allInterfaces.iImpedanceControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iImpedanceControl->getCurrentImpedanceLimit(j, &ret.min_stiffness, &ret.max_stiffness, &ret.min_damping, &ret.max_damping);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getCurrentImpedanceLimit");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setImpedanceRPC(const std::int32_t j, const double stiffness, const double damping)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iImpedanceControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iImpedanceControl->setImpedance(j,stiffness, damping);
    if (!ret) {
        yCError(CB_RPC, "Unable to setImpedance");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setImpedanceOffsetRPC(const std::int32_t j, const double offset)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iImpedanceControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iImpedanceControl->setImpedanceOffset(j, offset);
    if (!ret) {
        yCError(CB_RPC, "Unable to setImpedanceOffset");
    }
    return ret;
}

//------- iAmplifierControl

return_getAmpStatusAll ControlBoardRPCd::getAmpStatusAllRPC() const
{
    LOCKMUTEXSERVER
    return_getAmpStatusAll ret;
    if (!m_allInterfaces.iAmplifierControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iAmplifierControl->getAmpStatus(ret.val.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getAmpStatus");
    }
    return ret;
}

return_getAmpStatusOne ControlBoardRPCd::getAmpStatusOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getAmpStatusOne ret;
    if (!m_allInterfaces.iAmplifierControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAmplifierControl->getAmpStatus(j, &ret.val);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getAmpStatus");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::enableAmpRPC(const std::int32_t m)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iAmplifierControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iAmplifierControl->enableAmp(m);
    if (!ret) {
        yCError(CB_RPC, "Unable to enableAmp");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::disableAmpRPC(const std::int32_t m)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iAmplifierControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iAmplifierControl->disableAmp(m);
    if (!ret) {
        yCError(CB_RPC, "Unable to disableAmp");
    }
    return ret;
}

return_getPWM ControlBoardRPCd::getPWMRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getPWM ret;
    if (!m_allInterfaces.iAmplifierControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAmplifierControl->getPWM(j, &ret.val);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPWM");
    }
    return ret;
}

return_getPWMLimit ControlBoardRPCd::getPWMLimitRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getPWMLimit ret;
    if (!m_allInterfaces.iAmplifierControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAmplifierControl->getPWMLimit(m, &ret.val);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPWMLimit");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPWMLimitRPC(const std::int32_t m, const double val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iAmplifierControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iAmplifierControl->setPWMLimit(m,val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPWMLimit");
    }
    return ret;
}

return_getPowerSupplyVoltage ControlBoardRPCd::getPowerSupplyVoltageRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getPowerSupplyVoltage ret;
    if (!m_allInterfaces.iAmplifierControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAmplifierControl->getPowerSupplyVoltage(m, &ret.val);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPowerSupplyVoltage");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setPeakCurrentRPC(const std::int32_t m, const double val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iAmplifierControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iAmplifierControl->setPeakCurrent(m,val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPeakCurrent");
    }
    return ret;
}

return_getPeakCurrent ControlBoardRPCd::getPeakCurrentRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getPeakCurrent ret;
    if (!m_allInterfaces.iAmplifierControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAmplifierControl->getPeakCurrent(m, &ret.val);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPeakCurrent");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setNominalCurrentRPC(const std::int32_t m, const double val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iAmplifierControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iAmplifierControl->setNominalCurrent(m,val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setNominalCurrent");
    }
    return ret;
}

return_getNominalCurrent ControlBoardRPCd::getNominalCurrentRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getNominalCurrent ret;
    if (!m_allInterfaces.iAmplifierControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAmplifierControl->getNominalCurrent(m, &ret.val);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getNominalCurrent");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setMaxCurrentRPC(const std::int32_t m, const double val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iAmplifierControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iAmplifierControl->setMaxCurrent(m,val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setMaxCurrent");
    }
    return ret;
}

return_getMaxCurrent ControlBoardRPCd::getMaxCurrentRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getMaxCurrent ret;
    if (!m_allInterfaces.iAmplifierControl)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAmplifierControl->getMaxCurrent(m, &ret.val);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getPeakCurrent");
    }
    return ret;
}

//------- iCalibration

ReturnValue ControlBoardRPCd::calibrateSingleJointRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteCalibrator) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteCalibrator->calibrateSingleJoint(j);
    if (!ret) {
        yCError(CB_RPC, "Unable to calibrateSingleJoint");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::calibrateWholePartRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteCalibrator) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteCalibrator->calibrateWholePart();
    if (!ret) {
        yCError(CB_RPC, "Unable to calibrateWholePart");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::homingSingleJointRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteCalibrator) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteCalibrator->homingSingleJoint(j);
    if (!ret) {
        yCError(CB_RPC, "Unable to homingSingleJoint");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::homingWholePartRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteCalibrator) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteCalibrator->homingWholePart();
    if (!ret) {
        yCError(CB_RPC, "Unable to homingWholePartRPC");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::parkSingleJointRPC(const std::int32_t j, const bool wait)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteCalibrator) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteCalibrator->parkSingleJoint(j, wait);
    if (!ret) {
        yCError(CB_RPC, "Unable to parkSingleJoint");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::parkWholePartRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteCalibrator) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteCalibrator->parkWholePart();
    if (!ret) {
        yCError(CB_RPC, "Unable to parkWholePart");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::quitCalibrateRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteCalibrator) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteCalibrator->quitCalibrate();
    if (!ret) {
        yCError(CB_RPC, "Unable to quitCalibrate");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::quitParkRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteCalibrator) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteCalibrator->quitPark();
    if (!ret) {
        yCError(CB_RPC, "Unable to quitPark");
    }
    return ret;
}

return_isCalibratorDevicePresent ControlBoardRPCd::isCalibratorDevicePresentRPC() const
{
    LOCKMUTEXSERVER
    return_isCalibratorDevicePresent ret;
    if (!m_allInterfaces.iRemoteCalibrator)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iRemoteCalibrator->isCalibratorDevicePresent(&ret.isPresent);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to isCalibratorDevicePresent");
    }
    return ret;
}

//IInteractionMode
return_getInteractionModeOne ControlBoardRPCd::getInteractionModeOneRPC(const std::int32_t axis) const
{
    LOCKMUTEXSERVER
    return_getInteractionModeOne ret;
    if (!m_allInterfaces.iInteractionMode)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iInteractionMode->getInteractionMode(axis, ret.mode);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getInteractionMode");
    }
    return ret;
}

return_getInteractionModeGroup ControlBoardRPCd::getInteractionModesGroupRPC(const std::vector<std::int32_t>& joints) const
{
    LOCKMUTEXSERVER
    return_getInteractionModeGroup ret;
    if (!m_allInterfaces.iInteractionMode)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.modes.resize(joints.size());
    ret.ret = m_allInterfaces.iInteractionMode->getInteractionModes(joints, ret.modes);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getInteractionModes");
    }
    return ret;
}

return_getInteractionModeAll ControlBoardRPCd::getInteractionModesAllRPC() const
{
    LOCKMUTEXSERVER
    return_getInteractionModeAll ret;
    if (!m_allInterfaces.iInteractionMode)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.modes.resize(m_njoints);
    ret.ret = m_allInterfaces.iInteractionMode->getInteractionModes(ret.modes);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getInteractionModes");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setInteractionModeOneRPC(const std::int32_t axis, const yarp::dev::InteractionModeEnum mode)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iInteractionMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iInteractionMode->setInteractionMode(axis, mode);
    if (!ret) {
        yCError(CB_RPC, "Unable to setInteractionModeOne");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setInteractionModesGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iInteractionMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iInteractionMode->setInteractionModes(joints, modes);
    if (!ret) {
        yCError(CB_RPC, "Unable to setInteractionModes");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setInteractionModesAllRPC(const std::vector<yarp::dev::InteractionModeEnum>& modes)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iInteractionMode) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iInteractionMode->setInteractionModes(modes);
    if (!ret) {
        yCError(CB_RPC, "Unable to setInteractionModes");
    }
    return ret;
}

//IAxis

return_getAxisName ControlBoardRPCd::getAxisNameRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    return_getAxisName ret;
    if (!m_allInterfaces.iAxisInfo)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAxisInfo->getAxisName(j, ret.name);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getAxisName");
    }
    return ret;
}

return_getJointType  ControlBoardRPCd::getJointTypeRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    return_getJointType ret;
    if (!m_allInterfaces.iAxisInfo)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iAxisInfo->getJointType(j, ret.joint);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getJointType");
    }
    return ret;
}

// iControlCalibration

ReturnValue ControlBoardRPCd::calibrateRobotRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlCalibration) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlCalibration->calibrateRobot();
    if (!ret) {
        yCError(CB_RPC, "Unable to calibrateRobot");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::abortCalibrationRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlCalibration) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlCalibration->abortCalibration();
    if (!ret) {
        yCError(CB_RPC, "Unable to abortCalibration");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::abortParkRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlCalibration) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlCalibration->abortPark();
    if (!ret) {
        yCError(CB_RPC, "Unable to abortPark");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::parkRPC(const bool wait)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlCalibration) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlCalibration->park(wait);
    if (!ret) {
        yCError(CB_RPC, "Unable to park");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::calibrateAxisWithParamsRPC(const std::int32_t j, const std::int32_t ui, const double v1, const double v2, const double v3)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlCalibration) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlCalibration->calibrateAxisWithParams(j, ui, v1, v2, v3);
    if (!ret) {
        yCError(CB_RPC, "Unable to calibrateAxisWithParams");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setCalibrationParametersRPC(const std::int32_t j, const yCalibrationParameters& yparams)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlCalibration) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    yarp::dev::CalibrationParameters params;
    params.param1 = yparams.param1;
    params.param2 = yparams.param2;
    params.param3 = yparams.param3;
    params.param4 = yparams.param4;
    params.param5 = yparams.param5;
    params.paramZero = yparams.paramZero;
    ret = m_allInterfaces.iControlCalibration->setCalibrationParameters(j, params);
    if (!ret) {
        yCError(CB_RPC, "Unable to setCalibrationParameters");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::calibrationDoneRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iControlCalibration) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iControlCalibration->calibrationDone(j);
    if (!ret) {
        yCError(CB_RPC, "Unable to calibrationDone");
    }
    return ret;
}

//------- IJointFault

return_getLastJointFault ControlBoardRPCd::getLastJointFaultRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getLastJointFault ret;
    if (!m_allInterfaces.iJointFault)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iJointFault->getLastJointFault(j, ret.fault, ret.message);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getLastJointFault");
    }
    return ret;
}

//-------  IRemoteVariable

return_getRemoteVariable ControlBoardRPCd::getRemoteVariableRPC(const std::string& key) const
{
    LOCKMUTEXSERVER
    return_getRemoteVariable ret;
    if (!m_allInterfaces.iRemoteVariables)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iRemoteVariables->getRemoteVariable(key, ret.val);
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getRemoteVariable");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setRemoteVariableRPC(const std::string& key, const yarp::os::Bottle& val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iRemoteVariables) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iRemoteVariables->setRemoteVariable(key, val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setRemoteVariable");
    }
    return ret;
}

return_getRemoteVariablesList ControlBoardRPCd::getRemoteVariablesListRPC() const
{
    LOCKMUTEXSERVER
    return_getRemoteVariablesList ret;
    if (!m_allInterfaces.iRemoteVariables)
    {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iRemoteVariables->getRemoteVariablesList(ret.listOfKeys.data());
    if (!ret.ret)
    {
        yCError(CB_RPC, "Unable to getRemoteVariablesList");
    }
    return ret;
}

//-------  ICurrentControl

return_getRefCurrentAll ControlBoardRPCd::getRefCurrentAllRPC() const
{
    LOCKMUTEXSERVER
    return_getRefCurrentAll ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid iCurrentControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.refs.resize(m_njoints);
    ret.ret = m_allInterfaces.iCurrentControl->getRefCurrents(ret.refs.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getRefCurrents() failed");
    }
    return ret;
}

return_getRefCurrentOne ControlBoardRPCd::getRefCurrentOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getRefCurrentOne ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iCurrentControl->getRefCurrent(j, &ret.ref);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefCurrent");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setRefCurrentAllRPC(const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iCurrentControl->setRefCurrents(refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setRefCurrents");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setRefCurrentOneRPC(const std::int32_t j, const double ref)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iCurrentControl->setRefCurrent(j, ref);
    if (!ret) {
        yCError(CB_RPC, "Unable to setRefCurrent");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setRefCurrentGroupRPC(const std::vector<std::int32_t>& j, const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iCurrentControl->setRefCurrents(j.size(), j.data(), refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setRefCurrents");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

return_getCurrentAll ControlBoardRPCd::getCurrentAllRPC() const
{
    LOCKMUTEXSERVER
    return_getCurrentAll ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid iCurrentControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.currs.resize(m_njoints);
    ret.ret = m_allInterfaces.iCurrentControl->getRefCurrents(ret.currs.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getRefCurrents() failed");
    }
    return ret;
}

return_getCurrentOne ControlBoardRPCd::getCurrentOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getCurrentOne ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iCurrentControl->getCurrent(j, &ret.curr);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getCurrent");
    }
    return ret;
}

return_getCurrentRangeOne ControlBoardRPCd::getCurrentRangeOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getCurrentRangeOne ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iCurrentControl->getCurrentRange(j, &ret.min, &ret.max);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getCurrentRange");
    }
    return ret;
}

return_getCurrentRangeAll ControlBoardRPCd::getCurrentRangeAllRPC() const
{
    LOCKMUTEXSERVER
    return_getCurrentRangeAll ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid iCurrentControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.mins.resize(m_njoints);
    ret.maxs.resize(m_njoints);
    ret.ret = m_allInterfaces.iCurrentControl->getCurrentRanges(ret.mins.data(), ret.maxs.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getRefCurrents() failed");
    }
    return ret;
}

//-------  IPWMControl

yarp::dev::ReturnValue ControlBoardRPCd::setRefDutyCycleOneRPC(const std::int32_t j, const double v)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPWMControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPWMControl->setRefDutyCycle(j, v);
    if (!ret) {
        yCError(CB_RPC, "Unable to setRefDutyCycle");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setRefDutyCycleAllRPC(const std::vector<double>& v)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPWMControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPWMControl->setRefDutyCycles(v.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setRefDutyCycles");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

return_getRefDutyCycleOne ControlBoardRPCd::getRefDutyCycleOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getRefDutyCycleOne ret;
    if (!m_allInterfaces.iPWMControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPWMControl->getRefDutyCycle(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefDutyCycle");
    }
    return ret;
}

return_getRefDutyCycleAll ControlBoardRPCd::getRefDutyCycleAllRPC() const
{
    LOCKMUTEXSERVER
    return_getRefDutyCycleAll ret;
    if (!m_allInterfaces.iPWMControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iPWMControl->getRefDutyCycles(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefDutyCycle");
    }
    return ret;
}

return_getDutyCycleOne ControlBoardRPCd::getDutyCycleOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getDutyCycleOne ret;
    if (!m_allInterfaces.iPWMControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPWMControl->getDutyCycle(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getDutyCycle");
    }
    return ret;
}

return_getDutyCycleAll ControlBoardRPCd::getDutyCycleAllRPC() const
{
    LOCKMUTEXSERVER
    return_getDutyCycleAll ret;
    if (!m_allInterfaces.iPWMControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iPWMControl->getDutyCycles(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getDutyCycles");
    }
    return ret;
}

//-------  IEncoders

yarp::dev::ReturnValue ControlBoardRPCd::resetEncoderOneRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iEncodersTimed->resetEncoder(j);
    if (!ret) {
        yCError(CB_RPC, "Unable to resetEncoder");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::resetEncoderAllRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iEncodersTimed->resetEncoders();
    if (!ret) {
        yCError(CB_RPC, "Unable to resetEncoders");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setEncoderOneRPC(const std::int32_t j, const std::int32_t val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iEncodersTimed->setEncoder(j,val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setEncoder");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setEncoderAllRPC(const std::vector<double>& vals)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iEncodersTimed->setEncoders(vals.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setEncoders");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

return_getEncoderOne ControlBoardRPCd::getEncoderOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getEncoderOne ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iEncodersTimed->getEncoder(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getEncoder");
    }
    return ret;
}

return_getEncoderTimedOne ControlBoardRPCd::getEncoderTimedOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getEncoderTimedOne ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iEncodersTimed->getEncoderTimed(j, &ret.val, &ret.time);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getEncoderTimed");
    }
    return ret;
}

return_getEncoderAll ControlBoardRPCd::getEncoderAllRPC() const
{
    LOCKMUTEXSERVER
    return_getEncoderAll ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iEncodersTimed->getEncoders(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getEncoders");
    }
    return ret;
}

return_getEncoderTimedAll ControlBoardRPCd::getEncoderTimedAllRPC() const
{
    LOCKMUTEXSERVER
    return_getEncoderTimedAll ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.time.resize(m_njoints);
    ret.ret = m_allInterfaces.iEncodersTimed->getEncodersTimed(ret.val.data(), ret.time.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getEncoderTimed");
    }
    return ret;
}

return_getEncoderSpeedOne ControlBoardRPCd::getEncoderSpeedOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getEncoderSpeedOne ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iEncodersTimed->getEncoderSpeed(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getEncoderSpeed");
    }
    return ret;
}

return_getEncoderSpeedAll ControlBoardRPCd::getEncoderSpeedAllRPC() const
{
    LOCKMUTEXSERVER
    return_getEncoderSpeedAll ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iEncodersTimed->getEncoderSpeeds(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getEncodersSpeed");
    }
    return ret;
}

return_getEncoderAccelerationOne ControlBoardRPCd::getEncoderAccelerationOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getEncoderAccelerationOne ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iEncodersTimed->getEncoderAcceleration(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getEncoderAcceleration");
    }
    return ret;
}

return_getEncoderAccelerationAll ControlBoardRPCd::getEncoderAccelerationAllRPC() const
{
    LOCKMUTEXSERVER
    return_getEncoderAccelerationAll ret;
    if (!m_allInterfaces.iEncodersTimed) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iEncodersTimed->getEncoderAccelerations(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getEncoderAccelerations");
    }
    return ret;
}


//-------  IMotorEncoders

yarp::dev::ReturnValue ControlBoardRPCd::resetMotorEncoderRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iMotorEncoders->resetMotorEncoder(j);
    if (!ret) {
        yCError(CB_RPC, "Unable to resetMotorEncoder");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::resetMotorEncodersRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iMotorEncoders->resetMotorEncoders();
    if (!ret) {
        yCError(CB_RPC, "Unable to resetMotorEncoders");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setMotorEncoderRPC(const std::int32_t j, const double val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iMotorEncoders->setMotorEncoder(j, val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setMotorEncoder");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setMotorEncoderCountsPerRevolutionRPC(const std::int32_t j, const double val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iMotorEncoders->setMotorEncoderCountsPerRevolution(j, val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setMotorEncoderCountsPerRevolution");
    }
    return ret;
}

return_getMotorEncoderCountsPerRevolution ControlBoardRPCd::getMotorEncoderCountsPerRevolutionRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getMotorEncoderCountsPerRevolution ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotorEncoders->getMotorEncoderCountsPerRevolution(m, &ret.cpr);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getMotorEncoderCountsPerRevolution");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setMotorEncodersAllRPC(const std::vector<double>& vals)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iMotorEncoders->setMotorEncoders(vals.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setMotorEncoders");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

return_getMotorEncoderOne ControlBoardRPCd::getMotorEncoderOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getMotorEncoderOne ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotorEncoders->getMotorEncoder(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getMotorEncoder");
    }
    return ret;
}

return_getMotorEncoderAll ControlBoardRPCd::getMotorEncoderAllRPC() const
{
    return_getMotorEncoderAll ret;
    return ret;
}

return_getMotorEncoderTimedOne ControlBoardRPCd::getMotorEncoderTimedOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getMotorEncoderTimedOne ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotorEncoders->getMotorEncoderTimed(j, &ret.val, &ret.time);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getMotorEncoderTimed");
    }
    return ret;
}

return_getMotorEncoderTimedAll ControlBoardRPCd::getMotorEncodersTimedAllRPC() const
{
    return_getMotorEncoderTimedAll ret;
    return ret;
}

return_getMotorEncoderSpeedOne ControlBoardRPCd::getMotorEncoderSpeedOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getMotorEncoderSpeedOne ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotorEncoders->getMotorEncoderSpeed(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getMotorEncoderSpeed");
    }
    return ret;
}

return_getMotorEncoderSpeedAll ControlBoardRPCd::getMotorEncoderSpeedAllRPC() const
{
    LOCKMUTEXSERVER
    return_getMotorEncoderSpeedAll ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iMotorEncoders->getMotorEncoderSpeeds(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getMotorEncoderSpeeds");
    }
    return ret;
}

return_getMotorEncoderAccelerationOne ControlBoardRPCd::getMotorEncoderAccelerationOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getMotorEncoderAccelerationOne ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotorEncoders->getMotorEncoderAcceleration(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getMotorEncoderAcceleration");
    }
    return ret;
}

return_getMotorEncoderAccelerationAll ControlBoardRPCd::getMotorEncoderAccelerationAllRPC() const
{
    LOCKMUTEXSERVER
    return_getMotorEncoderAccelerationAll ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iMotorEncoders->getMotorEncoderAccelerations(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getMotorEncoderAccelerations");
    }
    return ret;
}

return_getNumberOfMotorEncoders ControlBoardRPCd::getNumberOfMotorEncodersRPC() const
{
    LOCKMUTEXSERVER
    return_getNumberOfMotorEncoders ret;
    if (!m_allInterfaces.iMotorEncoders) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotorEncoders->getNumberOfMotorEncoders(&ret.num);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getNumberOfMotorEncoders");
    }
    return ret;
}


//-------  ITorque
ReturnValue ControlBoardRPCd::setRefTorqueOneRPC(const std::int32_t j, const double vel)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iTorqueControl->setRefTorque(j, vel);
    if (!ret) {
        yCError(CB_RPC, "setRefTorque() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setRefTorqueAllRPC(const std::vector<double>& trq)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iTorqueControl->setRefTorques(trq.data());
    if (!ret) {
        yCError(CB_RPC, "setRefTorques() failed");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setRefTorqueGroupRPC(const std::vector<std::int32_t>& j, const std::vector<double>& trq)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iTorqueControl->setRefTorques(j.size(), j.data(), trq.data());
    if (!ret) {
        yCError(CB_RPC, "setRefTorques() failed");
    }
    return ret;
}

return_getRefTorqueOne ControlBoardRPCd::getRefTorqueOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getRefTorqueOne ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iTorqueControl->getRefTorque(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefTorque");
    }
    return ret;
}

return_getRefTorqueAll ControlBoardRPCd::getRefTorqueAllRPC() const
{
    LOCKMUTEXSERVER
    return_getRefTorqueAll ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iTorqueControl->getRefTorques(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefTorques");
    }
    return ret;
}

return_getRefTorqueGroup ControlBoardRPCd::getRefTorqueGroupRPC(const std::vector<std::int32_t>& j) const
{
    LOCKMUTEXSERVER
    return_getRefTorqueGroup ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(j.size());
    ret.ret = m_allInterfaces.iTorqueControl->getRefTorques(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefTorques");
    }
    return ret;
}

return_getTorqueRangeOne ControlBoardRPCd::getTorqueRangeOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getTorqueRangeOne ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iTorqueControl->getTorqueRange(j, &ret.min, &ret.max);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTorqueRange");
    }
    return ret;
}

return_getTorqueRangeAll ControlBoardRPCd::getTorqueRangeAllRPC() const
{
    LOCKMUTEXSERVER
    return_getTorqueRangeAll ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.mins.resize(m_njoints);
    ret.maxs.resize(m_njoints);
    ret.ret = m_allInterfaces.iTorqueControl->getTorqueRanges(ret.mins.data(),ret.maxs.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTorqueRanges");
    }
    return ret;
}

//-------  IPositionDirect

yarp::dev::ReturnValue    ControlBoardRPCd::setRefPositionOneRPC(const std::int32_t j, double ref)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionDirect->setPosition(j, ref);
    if (!ret) {
        yCError(CB_RPC, "Unable to setPosition");
    }
    return ret;
}

yarp::dev::ReturnValue    ControlBoardRPCd::setRefPositionAllRPC(const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionDirect->setPositions(refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setPosition");
    }
    return ret;
}

yarp::dev::ReturnValue    ControlBoardRPCd::setRefPositionGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionDirect->setPositions(joints.size(), joints.data(), refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setPositions");
    }
    return ret;
}


return_getRefPositionOne ControlBoardRPCd::getRefPositionOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getRefPositionOne ret;
    if (!m_allInterfaces.iPositionDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPositionDirect->getRefPosition(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefPosition");
    }
    return ret;
}

return_getRefPositionAll ControlBoardRPCd::getRefPositionAllRPC() const
{
    LOCKMUTEXSERVER
    return_getRefPositionAll ret;
    if (!m_allInterfaces.iPositionDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iPositionDirect->getRefPositions(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefPositions");
    }
    return ret;
}

return_getRefPositionGroup ControlBoardRPCd::getRefPositionGroupRPC(const std::vector<std::int32_t>& j) const
{
    LOCKMUTEXSERVER
    return_getRefPositionGroup ret;
    if (!m_allInterfaces.iPositionDirect) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(j.size());
    ret.ret = m_allInterfaces.iPositionDirect->getRefPositions(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getRefPositions");
    }
    return ret;
}

//-------  IPositionControl

yarp::dev::ReturnValue ControlBoardRPCd::positionMoveOneRPC(const std::int32_t j, const double ref)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->positionMove(j, ref);
    if (!ret) {
        yCError(CB_RPC, "Unable to positionMove");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::positionMoveGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->positionMove(joints.size(), joints.data(), refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to positionMove");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::positionMoveAllRPC(const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->positionMove(refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to positionMove");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::relativeMoveOneRPC(const std::int32_t j, const double delta)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->relativeMove(j, delta);
    if (!ret) {
        yCError(CB_RPC, "Unable to relativeMove");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::relativeMoveGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->relativeMove(joints.size(), joints.data(), refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to relativeMove");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::relativeMoveAllRPC(const std::vector<double>& deltas)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->relativeMove(deltas.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to relativeMove");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setTrajSpeedOneRPC(const std::int32_t j, const double sp)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->setTrajSpeed(j, sp);
    if (!ret) {
        yCError(CB_RPC, "Unable to setTrajSpeed");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setTrajSpeedGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& spds)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->setTrajSpeeds(joints.size(), joints.data(), spds.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setTrajSpeeds");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setTrajSpeedAllRPC(const std::vector<double>& spds)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->setTrajSpeeds(spds.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setTrajSpeeds");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setTrajAccelerationOneRPC(const std::int32_t j, const double acc) 
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->setTrajAcceleration(j, acc);
    if (!ret) {
        yCError(CB_RPC, "Unable to setTrajAcceleration");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setTrajAccelerationsGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& accs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->setTrajAccelerations(joints.size(), joints.data(), accs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setTrajAccelerations");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setTrajAccelerationsAllRPC(const std::vector<double>& accs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->setTrajAccelerations(accs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to setTrajAccelerations");
        return ReturnValue::return_code::return_value_error_generic;
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::stopOneRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->stop(j);
    if (!ret) {
        yCError(CB_RPC, "Unable to stop");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::stopGroupRPC(const std::vector<std::int32_t>& joints)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->stop(joints.size(),joints.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to stop");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::stopAllRPC()
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iPositionControl->stop();
    if (!ret) {
        yCError(CB_RPC, "Unable to stop");
    }
    return ret;
}

yarp::dev::ReturnValue    ControlBoardRPCd::velocityMoveOneRPC(const std::int32_t j, double ref)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iVelocityControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iVelocityControl->velocityMove(j, ref);
    if (!ret) {
        yCError(CB_RPC, "Unable to velocityMove");
    }
    return ret;
}

yarp::dev::ReturnValue    ControlBoardRPCd::velocityMoveGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iVelocityControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iVelocityControl->velocityMove(joints.size(), joints.data(), refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to velocityMove");
    }
    return ret;
}

yarp::dev::ReturnValue    ControlBoardRPCd::velocityMoveAllRPC(const std::vector<double>& refs)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iVelocityControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iVelocityControl->velocityMove(refs.data());
    if (!ret) {
        yCError(CB_RPC, "Unable to velocityMove");
    }
    return ret;
}


return_getTargetPositionOne ControlBoardRPCd::getTargetPositionOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getTargetPositionOne ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPositionControl->getTargetPosition(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTargetPosition");
    }
    return ret;
}

return_getTargetPositionGroup ControlBoardRPCd::getTargetPositionGroupRPC(const std::vector<std::int32_t>& joints) const
{
    LOCKMUTEXSERVER
    return_getTargetPositionGroup ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(joints.size());
    ret.ret = m_allInterfaces.iPositionControl->getTargetPositions(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTargetPositions");
    }
    return ret;
}

return_getTargetPositionAll ControlBoardRPCd::getTargetPositionAllRPC() const
{
    LOCKMUTEXSERVER
    return_getTargetPositionAll ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iPositionControl->getTargetPositions(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTargetPositions");
    }
    return ret;
}

return_getTrajSpeedOne ControlBoardRPCd::getTrajSpeedOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getTrajSpeedOne ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPositionControl->getTrajSpeed(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTrajSpeed");
    }
    return ret;
}

return_getTrajSpeedsGroup ControlBoardRPCd::getTrajSpeedsGroupRPC(const std::vector<std::int32_t>& joints) const
{
    LOCKMUTEXSERVER
    return_getTrajSpeedsGroup ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid iPositionControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.val.resize(joints.size());
    ret.ret = m_allInterfaces.iPositionControl->getTrajSpeeds(joints.size(), joints.data(), ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getTrajSpeeds() failed");
    }
    return ret;
}

return_getTrajSpeedsAll ControlBoardRPCd::getTrajSpeedsAllRPC() const
{
    LOCKMUTEXSERVER
    return_getTrajSpeedsAll ret;
    if (!m_allInterfaces.iCurrentControl) {
        yCError(CB_RPC, "Invalid iPositionControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iPositionControl->getTrajSpeeds(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getTrajSpeeds() failed");
    }
    return ret;
}

return_getTrajAccelerationOne ControlBoardRPCd::getTrajAccelerationOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getTrajAccelerationOne ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPositionControl->getTrajAcceleration(j, &ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTrajSpeed");
    }
    return ret;
}

return_getTrajAccelerationGroup ControlBoardRPCd::getTrajAccelerationGroupRPC(const std::vector<std::int32_t>& joints) const
{
    LOCKMUTEXSERVER
    return_getTrajAccelerationGroup ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid iPositionControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.val.resize(joints.size());
    ret.ret = m_allInterfaces.iPositionControl->getTrajAccelerations(joints.size(), joints.data(), ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getTrajAccelerations() failed");
    }
    return ret;
}

return_getTrajAccelerationAll ControlBoardRPCd::getTrajAccelerationAllRPC() const
{
    LOCKMUTEXSERVER
    return_getTrajAccelerationAll ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid iPositionControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iPositionControl->getTrajAccelerations(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getTrajAccelerations() failed");
    }
    return ret;
}

return_checkMotionDoneOne ControlBoardRPCd::checkMotionDoneOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_checkMotionDoneOne ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPositionControl->checkMotionDone(j, ret.flag);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to checkMotionDone");
    }
    return ret;
}

return_checkMotionDoneGroup ControlBoardRPCd::checkMotionDoneGroupRPC(const std::vector<std::int32_t>& joints) const
{
    LOCKMUTEXSERVER
    return_checkMotionDoneGroup ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid iPositionControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPositionControl->checkMotionDone(joints, ret.flag);
    if (!ret.ret) {
        yCError(CB_RPC, "checkMotionDone() failed");
    }
    return ret;
}

return_checkMotionDoneAll ControlBoardRPCd::checkMotionDoneAllRPC() const
{
    LOCKMUTEXSERVER
    return_checkMotionDoneAll ret;
    if (!m_allInterfaces.iPositionControl) {
        yCError(CB_RPC, "Invalid iPositionControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iPositionControl->checkMotionDone(ret.flag);
    if (!ret.ret) {
        yCError(CB_RPC, "checkMotionDone() failed");
    }
    return ret;
}

//-------  IMotorControl

return_getNumberOfMotors ControlBoardRPCd::getNumberOfMotorsRPC() const
{
    LOCKMUTEXSERVER
    return_getNumberOfMotors ret;
    if (!m_allInterfaces.iMotor) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotor->getNumberOfMotors(&ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getNumberOfMotors");
    }
    return ret;
}

return_getTemperatureOne ControlBoardRPCd::getTemperatureOneRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getTemperatureOne ret;
    if (!m_allInterfaces.iMotor) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotor->getTemperature(m,&ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTemperature");
    }
    return ret;
}

return_getTemperatureAll ControlBoardRPCd::getTemperatureAllRPC() const
{
    LOCKMUTEXSERVER
    return_getTemperatureAll ret;
    if (!m_allInterfaces.iMotor) {
        yCError(CB_RPC, "Invalid iMotor interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iMotor->getTemperatures(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getTemperatures() failed");
    }
    return ret;
}

return_getTemperatureLimit ControlBoardRPCd::getTemperatureLimitRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getTemperatureLimit ret;
    if (!m_allInterfaces.iMotor) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotor->getTemperatureLimit(m,&ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTemperatureLimit");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setTemperatureLimitRPC(const std::int32_t m, const double val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iMotor) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iMotor->setTemperatureLimit(m, val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setTemperatureLimit");
    }
    return ret;
}

return_getGearboxRatio ControlBoardRPCd::getGearboxRatioRPC(const std::int32_t m) const
{
    LOCKMUTEXSERVER
    return_getGearboxRatio ret;
    if (!m_allInterfaces.iMotor) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iMotor->getGearboxRatio(m,&ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getGearboxRatio");
    }
    return ret;
}

yarp::dev::ReturnValue ControlBoardRPCd::setGearboxRatioRPC(const std::int32_t m, const double val)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iMotor) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret = m_allInterfaces.iMotor->setGearboxRatio(m, val);
    if (!ret) {
        yCError(CB_RPC, "Unable to setGearboxRatio");
    }
    return ret;
}

//-------  IMotorControl
yarp::dev::ReturnValue ControlBoardRPCd::setMotorTorqueParamsRPC(const std::int32_t j, const yMotorTorqueParameters& yparams)
{
    LOCKMUTEXSERVER
    ReturnValue ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    yarp::dev::MotorTorqueParameters params;
    params.bemf = yparams.bemf;
    params.bemf_scale = yparams.bemf_scale;
    params.coulombNeg = yparams.coulombNeg;
    params.coulombPos = yparams.coulombPos;
    params.ktau = yparams.ktau;
    params.ktau_scale = yparams.ktau_scale;
    params.velocityThres = yparams.velocityThres;
    params.viscousNeg = yparams.viscousNeg;
    params.viscousPos = yparams.viscousPos;
    ret = m_allInterfaces.iTorqueControl->setMotorTorqueParams(j, params);
    if (!ret) {
        yCError(CB_RPC, "Unable to setTemperatureLimit");
    }
    return ret;
}

return_getMotorTorqueParams ControlBoardRPCd::getMotorTorqueParamsRPC(const std::int32_t j)
{
    LOCKMUTEXSERVER
    return_getMotorTorqueParams ret;
    if (!m_allInterfaces.iTorqueControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    yarp::dev::MotorTorqueParameters params;
    ret.ret = m_allInterfaces.iTorqueControl->getMotorTorqueParams(j,&params);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getMotorTorqueParams");
    }
    ret.params.bemf = params.bemf;
    ret.params.bemf_scale = params.bemf_scale;
    ret.params.coulombNeg = params.coulombNeg;
    ret.params.coulombPos = params.coulombPos;
    ret.params.ktau = params.ktau;
    ret.params.ktau_scale = params.ktau_scale;
    ret.params.velocityThres = params.velocityThres;
    ret.params.viscousNeg = params.viscousNeg;
    ret.params.viscousPos = params.viscousPos;
    return ret;
}

//------- IVelocityControl
return_getTargetVelocityOne ControlBoardRPCd::getTargetVelocityOneRPC(const std::int32_t j) const
{
    LOCKMUTEXSERVER
    return_getTargetVelocityOne ret;
    if (!m_allInterfaces.iVelocityControl) {
        yCError(CB_RPC, "Invalid interface");
        ret.ret = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    ret.ret = m_allInterfaces.iVelocityControl->getTargetVelocity(j,&ret.val);
    if (!ret.ret) {
        yCError(CB_RPC, "Unable to getTargetVelocity");
    }
    return ret;
}

return_getTargetVelocityGroup ControlBoardRPCd::getTargetVelocityGroupRPC(const std::vector<std::int32_t>& joints) const
{
    LOCKMUTEXSERVER
    return_getTargetVelocityGroup ret;
    if (!m_allInterfaces.iVelocityControl) {
        yCError(CB_RPC, "Invalid iVelocityControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.val.resize(joints.size());
    ret.ret = m_allInterfaces.iVelocityControl->getTargetVelocities(joints.size(), joints.data(), ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getTargetVelocity() failed");
    }
    return ret;
}

return_getTargetVelocityAll ControlBoardRPCd::getTargetVelocityAllRPC() const
{
    LOCKMUTEXSERVER
    return_getTargetVelocityAll ret;
    if (!m_allInterfaces.iVelocityControl) {
        yCError(CB_RPC, "Invalid iVelocityControl interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.val.resize(m_njoints);
    ret.ret = m_allInterfaces.iVelocityControl->getTargetVelocities(ret.val.data());
    if (!ret.ret) {
        yCError(CB_RPC, "getTargetVelocity() failed");
    }
    return ret;
}

return_getDeviceInterfaces         ControlBoardRPCd::getDeviceInterfacesRPC() const
{
    return_getDeviceInterfaces ret;
    ret.ret = ReturnValue_ok;
    ret.interfaces["iAmplifierControl"] = m_allInterfaces.iAmplifierControl!=nullptr;
    ret.interfaces["iAxisInfo"] = m_allInterfaces.iAxisInfo!=nullptr;
    ret.interfaces["iControlCalibration"] = m_allInterfaces.iControlCalibration!=nullptr;
    ret.interfaces["iControlLimits"] = m_allInterfaces.iControlLimits!=nullptr;
    ret.interfaces["iControlMode"] = m_allInterfaces.iControlMode!=nullptr;
    ret.interfaces["iCurrentControl"] = m_allInterfaces.iCurrentControl!=nullptr;
    ret.interfaces["iEncodersTimed"] = m_allInterfaces.iEncodersTimed!=nullptr;
    ret.interfaces["iImpedanceControl"] = m_allInterfaces.iImpedanceControl!=nullptr;
    ret.interfaces["iInteractionMode"] = m_allInterfaces.iInteractionMode!=nullptr;
    ret.interfaces["iJointBrake"] = m_allInterfaces.iJointBrake!=nullptr;
    ret.interfaces["iJointFault"] = m_allInterfaces.iJointFault!=nullptr;
    ret.interfaces["iMotor"] = m_allInterfaces.iMotor!=nullptr;
    ret.interfaces["iMotorEncoders"] = m_allInterfaces.iMotorEncoders!=nullptr;
    ret.interfaces["iPidControl"] = m_allInterfaces.iPidControl!=nullptr;
    ret.interfaces["iPositionControl"] = m_allInterfaces.iPositionControl!=nullptr;
    ret.interfaces["iPositionDirect"] = m_allInterfaces.iPositionDirect!=nullptr;
    ret.interfaces["iPreciselyTimed"] = m_allInterfaces.iPreciselyTimed!=nullptr;
    ret.interfaces["iPWMControl"] = m_allInterfaces.iPWMControl!=nullptr;
    ret.interfaces["iRemoteCalibrator"] = m_allInterfaces.iRemoteCalibrator!=nullptr;
    ret.interfaces["iRemoteVariables"] = m_allInterfaces.iRemoteVariables!=nullptr;
    ret.interfaces["iTorqueControl"] = m_allInterfaces.iTorqueControl!=nullptr;
    ret.interfaces["iVelocityControl"] = m_allInterfaces.iVelocityControl!=nullptr;
    ret.interfaces["iVelocityDirect"] = m_allInterfaces.iVelocityDirect!=nullptr;
    return ret;
}