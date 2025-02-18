/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include "Rangefinder2DServerImpl.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

namespace {
YARP_LOG_COMPONENT(RANGEFINDER2D_RPC, "yarp.device.rangefinder2D_nws_yarp.IRangeFinder2DRPCd")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(MAP2D_RPC, "Invalid interface"); return false;}}



ReturnValue IRangefinder2DRPCd::setScanRate_RPC(const double rate)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    if (m_irf == nullptr)
    {
        yCError(RANGEFINDER2D_RPC, "Invalid interface");
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    auto rr = m_irf->setScanRate(rate);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to setScanRate");
        return rr;
    }
    return rr;
}

ReturnValue IRangefinder2DRPCd::setHorizontalResolution_RPC(const double resolution)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return ReturnValue::return_code::return_value_error_not_ready;
        }
    }

    auto rr = m_irf->setHorizontalResolution(resolution);
    if (!rr) {
        yCError(RANGEFINDER2D_RPC, "Unable to setHorizontalResolution");
        return rr;
    }
    return rr;
}

ReturnValue IRangefinder2DRPCd::setScanLimits_RPC(const double min, const double max)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return ReturnValue::return_code::return_value_error_not_ready;
        }
    }

    auto rr = m_irf->setScanLimits(min, max);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to setScanLimits");
        return rr;
    }
    return rr;
}

ReturnValue IRangefinder2DRPCd::setDistanceRange_RPC(const double min, const double max)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return ReturnValue::return_code::return_value_error_not_ready;
        }
    }

    auto rr = m_irf->setDistanceRange(min, max);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to setDistanceRange");
        return rr;
    }
    return rr;
}

return_getDeviceStatus IRangefinder2DRPCd::getDeviceStatus_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getDeviceStatus ret;
    if (m_irf == nullptr)
    {
        yCError(RANGEFINDER2D_RPC, "Invalid interface");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    yarp::dev::IRangefinder2D::Device_status status;
    auto rr = m_irf->getDeviceStatus(status);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to getDeviceStatus_RPC");
        ret.retval = rr;
    }
    else
    {
        ret.retval = rr;
        ret.status = status;
    }
    return ret;
}

return_getDistanceRange IRangefinder2DRPCd::getDistanceRange_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getDistanceRange ret;
    if (m_irf == nullptr)
    {
        yCError(RANGEFINDER2D_RPC, "Invalid interface");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    double min, max;
    auto rr = m_irf->getDistanceRange(min, max);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to getDistanceRange_RPC");
        ret.retval = rr;
    }
    else
    {
        ret.retval = rr;
        ret.min = min;
        ret.max = max;
    }
    return ret;
}

return_getScanLimits IRangefinder2DRPCd::getScanLimits_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getScanLimits ret;
    if (m_irf == nullptr)
    {
        yCError(RANGEFINDER2D_RPC, "Invalid interface");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    double min, max;
    auto rr = m_irf->getScanLimits(min, max);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to getScanLimits_RPC");
        ret.retval = rr;
    }
    else
    {
        ret.retval = rr;
        ret.min = min;
        ret.max = max;
    }
    return ret;
}

return_getHorizontalResolution IRangefinder2DRPCd::getHorizontalResolution_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getHorizontalResolution ret;
    if (m_irf == nullptr)
    {
        yCError(RANGEFINDER2D_RPC, "Invalid interface");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    double step;
    auto rr = m_irf->getHorizontalResolution(step);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to getHorizontalResolution_RPC");
        ret.retval = rr;
    }
    else
    {
        ret.retval = rr;
        ret.step = step;
    }
    return ret;
}

return_getScanRate IRangefinder2DRPCd::getScanRate_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getScanRate ret;
    if (m_irf == nullptr)
    {
        yCError(RANGEFINDER2D_RPC, "Invalid interface");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    double rate;
    auto rr = m_irf->getScanRate(rate);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to getScanRate_RPC");
        ret.retval = rr;
    }
    else
    {
        ret.retval = rr;
        ret.rate = rate;
    }
    return ret;
}

return_getDeviceInfo IRangefinder2DRPCd::getDeviceInfo_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getDeviceInfo ret;
    if (m_irf == nullptr)
    {
        yCError(RANGEFINDER2D_RPC, "Invalid interface");
        ret.retval = ReturnValue::return_code::return_value_error_not_ready;
        return ret;
    }

    std::string info;
    auto rr = m_irf->getDeviceInfo(info);
    if (!rr)
    {
        yCError(RANGEFINDER2D_RPC, "Unable to getDeviceInfo_RPC");
        ret.retval = rr;
    }
    else
    {
        ret.retval = rr;
        ret.device_info = info;
    }
    return ret;
}
