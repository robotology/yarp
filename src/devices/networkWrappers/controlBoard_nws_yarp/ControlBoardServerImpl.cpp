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

return_isJointBraked ControlBoardRPCd::isJointBraked_RPC(const std::int16_t j) const
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
        yCError(CB_RPC, "Unable to isJointBraked_RPC");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setManualBrakeActive_RPC(const std::int16_t j, const bool active)
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
        yCError(CB_RPC, "Unable to setManualBrakeActive_RPC");
    }
    return ret;
}

ReturnValue ControlBoardRPCd::setAutoBrakeEnabled_RPC(const std::int16_t j, const bool enabled)
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
        yCError(CB_RPC, "Unable to setAutoBrakeEnabled_RPC");
    }
    return ret;
}

return_getAutoBrakeEnabled ControlBoardRPCd::getAutoBrakeEnabled_RPC(const std::int16_t j) const
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
        yCError(CB_RPC, "Unable to getAutoBrakeEnabled_RPC");
    }
    return ret;
}