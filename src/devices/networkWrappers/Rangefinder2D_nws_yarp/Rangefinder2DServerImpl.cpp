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



bool IRangefinder2DRPCd::setScanRate_RPC(const double rate)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_irf == nullptr) { yCError(RANGEFINDER2D_RPC, "Invalid interface"); return false; }}

    if (!m_irf->setScanRate(rate))
    {
        yCError(RANGEFINDER2D_RPC, "Unable to setScanRate");
        return false;
    }
    return true;
}

bool IRangefinder2DRPCd::setHorizontalResolution_RPC(const double resolution)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return false;
        }
    }

    if (!m_irf->setHorizontalResolution(resolution)) {
        yCError(RANGEFINDER2D_RPC, "Unable to setHorizontalResolution");
        return false;
    }
    return true;
}

bool IRangefinder2DRPCd::setScanLimits_RPC(const double min, const double max)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return false;
        }
    }

    if (!m_irf->setScanLimits(min, max)) {
        yCError(RANGEFINDER2D_RPC, "Unable to setScanLimits");
        return false;
    }
    return true;
}

bool IRangefinder2DRPCd::setDistanceRange_RPC(const double min, const double max)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return false;
        }
    }

    if (!m_irf->setDistanceRange(min, max)) {
        yCError(RANGEFINDER2D_RPC, "Unable to setDistanceRange");
        return false;
    }
    return true;
}

return_getDeviceStatus IRangefinder2DRPCd::getDeviceStatus_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    return_getDeviceStatus ret;
    {if (m_irf == nullptr) { yCError(RANGEFINDER2D_RPC, "Invalid interface"); return ret; }}
    yarp::dev::IRangefinder2D::Device_status status;
    if (!m_irf->getDeviceStatus(status))
    {
        yCError(RANGEFINDER2D_RPC, "Unable to getDeviceStatus_RPC");
        ret.retval = false;
    }
    else
    {
        ret.retval = true;
        ret.status = status;
    }
    return ret;
}

return_getDistanceRange IRangefinder2DRPCd::getDistanceRange_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getDistanceRange ret;
    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return ret;
        }
    }
    double min, max;
    if (!m_irf->getDistanceRange(min, max)) {
        yCError(RANGEFINDER2D_RPC, "Unable to getDistanceRange_RPC");
        ret.retval = false;
    } else {
        ret.retval = true;
        ret.min = min;
        ret.max = max;
    }
    return ret;
}

return_getScanLimits IRangefinder2DRPCd::getScanLimits_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getScanLimits ret;
    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return ret;
        }
    }
    double min, max;
    if (!m_irf->getScanLimits(min, max)) {
        yCError(RANGEFINDER2D_RPC, "Unable to getScanLimits_RPC");
        ret.retval = false;
    } else {
        ret.retval = true;
        ret.min = min;
        ret.max = max;
    }
    return ret;
}

return_getHorizontalResolution IRangefinder2DRPCd::getHorizontalResolution_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getHorizontalResolution ret;
    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return ret;
        }
    }
    double step;
    if (!m_irf->getHorizontalResolution(step)) {
        yCError(RANGEFINDER2D_RPC, "Unable to getHorizontalResolution_RPC");
        ret.retval = false;
    } else {
        ret.retval = true;
        ret.step = step;
    }
    return ret;
}

return_getScanRate IRangefinder2DRPCd::getScanRate_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getScanRate ret;
    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return ret;
        }
    }
    double rate;
    if (!m_irf->getScanRate(rate)) {
        yCError(RANGEFINDER2D_RPC, "Unable to getScanRate_RPC");
        ret.retval = false;
    } else {
        ret.retval = true;
        ret.rate = rate;
    }
    return ret;
}

return_getDeviceInfo IRangefinder2DRPCd::getDeviceInfo_RPC()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    return_getDeviceInfo ret;
    {
        if (m_irf == nullptr) {
            yCError(RANGEFINDER2D_RPC, "Invalid interface");
            return ret;
        }
    }
    std::string info;
    if (!m_irf->getDeviceInfo(info)) {
        yCError(RANGEFINDER2D_RPC, "Unable to getDeviceInfo_RPC");
        ret.retval = false;
    } else {
        ret.retval = true;
        ret.device_info = info;
    }
    return ret;
}
