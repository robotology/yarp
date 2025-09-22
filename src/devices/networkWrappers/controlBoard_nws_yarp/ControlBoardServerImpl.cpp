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

return_getDesiredVelocityOne ControlBoardRPCd::getDesiredVelocityOneRPC(const std::int32_t j) const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getDesiredVelocityOne ret;
    if (!m_iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.ret = m_iVelocityDirect->getDesiredVelocity(j, ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getDesiredVelocity() failed");
    }
    return ret;
}

return_getDesiredVelocityAll ControlBoardRPCd::getDesiredVelocityAllRPC() const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getDesiredVelocityAll ret;
    if (!m_iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.ret = m_iVelocityDirect->getDesiredVelocity(ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getDesiredVelocity() failed");
    }
    return ret;
}

return_getDesiredVelocityGroup ControlBoardRPCd::getDesiredVelocityGroupRPC(const std::vector<std::int32_t>& jnts) const
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getDesiredVelocityGroup ret;
    if (!m_iVelocityDirect) {
        yCError(CB_RPC, "Invalid IVelocityDirect interface");
        ret.ret = yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }
    ret.ret = m_iVelocityDirect->getDesiredVelocity(jnts, ret.vel);
    if (!ret.ret) {
        yCError(CB_RPC, "getDesiredVelocity() failed");
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
