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
