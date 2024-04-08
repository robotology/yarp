/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DeviceBundler.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace {
YARP_LOG_COMPONENT(DEVICEBUNDLER, "yarp.device.DeviceBundler")
}

DeviceBundler::DeviceBundler()
{
}

DeviceBundler::~DeviceBundler()
{
}

bool DeviceBundler::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    std::string s = config.toString();

    bool ret = true;

    //open wrapper device
    yCInfo(DEVICEBUNDLER, "Opening device: %s", m_wrapper_device.c_str());
    yarp::os::Property config_wrap (config.toString().c_str());
    config_wrap.unput("device");
    config_wrap.unput("wrapper_device");
    config_wrap.unput("attached_device");
    config_wrap.unput("wrapping_enabled");
    config_wrap.put("device", m_wrapper_device);
    std::string sw = config_wrap.toString();
    ret = m_pdev_wrapper.open(config_wrap);
    if (!ret || !m_pdev_wrapper.isValid())
    {
        yCError(DEVICEBUNDLER, "Unable to open device:%s", m_wrapper_device.c_str());
        return false;
    }

    //open secondary device
    yCInfo(DEVICEBUNDLER, "Opening device: %s", m_attached_device.c_str());
    yarp::os::Property config_sub(config.toString().c_str());
    config_sub.unput("device");
    config_sub.unput("wrapper_device");
    config_sub.unput("attached_device");
    config_sub.unput("wrapping_enabled");
    config_sub.put("device", m_attached_device);
    std::string ss = config_sub.toString();
    ret = m_pdev_subdevice.open(config_sub);
    if (!ret || !m_pdev_subdevice.isValid())
    {
        yCError(DEVICEBUNDLER, "Unable to open subdevice:%s", m_attached_device.c_str());
        return false;
    }

    if (m_doNotAttach)
    {
        yCInfo(DEVICEBUNDLER, "doNotAttach option found. Device will not be attached.");
        return true;
    }

    //Attach operations below
    yCInfo(DEVICEBUNDLER, "Attaching devices %s and %s.", m_wrapper_device.c_str(), m_attached_device.c_str());
    ret = m_pdev_wrapper.view(m_iWrapper);
    if (!ret)
    {
        yCError(DEVICEBUNDLER, "Unable to open iWrapper interface. Maybe %s is not a wrapper device?", m_wrapper_device.c_str());
        return false;
    }

    ret = m_pdev_wrapper.view(m_iService);
    if (ret)
    {
        yCInfo(DEVICEBUNDLER, "The device implements the IService iterface");
    }

    ret = m_iWrapper->attach(&m_pdev_subdevice);
    if (!ret)
    {
        yCError(DEVICEBUNDLER, "Unable to attach the two devices: %s and %s", m_wrapper_device.c_str(), m_attached_device.c_str());
        return false;
    }

    yCInfo(DEVICEBUNDLER, "Attach operation between %s and %s completed.", m_wrapper_device.c_str(), m_attached_device.c_str());
    return true;
}

bool DeviceBundler::close()
{
    if (m_iWrapper)
    {
        m_iWrapper->detach();
        m_iWrapper = nullptr;
        m_iService = nullptr;
    }

    if (m_pdev_wrapper.isValid())
    {
        m_pdev_wrapper.close();
    }

    if (m_pdev_subdevice.isValid())
    {
        m_pdev_subdevice.close();
    }

    return true;
}

bool DeviceBundler::startService()
{
    if (m_iService)
    {
        return m_iService->startService();
    }
    return true; //If not implemented, emulate running in background
}

bool DeviceBundler::updateService()
{
    if (m_iService)
    {
        return m_iService->updateService();
    }
    return false;
}

bool DeviceBundler::stopService()
{
    if (m_iService)
    {
        return m_iService->stopService();
    }
    return false;
}
