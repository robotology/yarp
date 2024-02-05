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
    yarp::os::Property config_wrap (config.toString().c_str());
    config_wrap.unput("device");
    config_wrap.put("device", m_wrapper_device);
    std::string sw = config_wrap.toString();
    ret = m_pdev_wrapper.open(config_wrap);
    if (!ret || !m_pdev_wrapper.isValid())
    {
        yCError(DEVICEBUNDLER, "Unable to open device:%s", m_wrapper_device);
        return false;
    }

    //open secondary device
    yarp::os::Property config_sub(config.toString().c_str());
    config_sub.unput("device");
    config_sub.put("device", m_attached_device);
    std::string ss = config_sub.toString();
    ret = m_pdev_subdevice.open(config_sub);
    if (!ret || !m_pdev_subdevice.isValid())
    {
        yCError(DEVICEBUNDLER, "Unable to open subdevice:%s", m_attached_device);
        return false;
    }

    if (m_doNotAttach)
    {
        yCInfo(DEVICEBUNDLER, "doNotAttach option found. Device will not be attached.");
        return true;
    }

    //Attach operations below
    ret = m_pdev_wrapper.view(m_iWrapper);
    if (!ret)
    {
        yCError(DEVICEBUNDLER, "Unable to open iWrapper interface. Maybe %s is not a wrapper device?", m_wrapper_device);
        return false;
    }

    ret = m_iWrapper->attach(&m_pdev_subdevice);
    if (!ret)
    {
        yCError(DEVICEBUNDLER, "Unable to attach the two devices: %s and %s", m_wrapper_device, m_attached_device);
        return false;
    }

    yCDebug(DEVICEBUNDLER, "Attach operation between %s and %s completed.", m_wrapper_device, m_attached_device);
    return true;
}

bool DeviceBundler::close()
{
    if (m_iWrapper)
    {
        m_iWrapper->detach();
        m_iWrapper = nullptr;
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
