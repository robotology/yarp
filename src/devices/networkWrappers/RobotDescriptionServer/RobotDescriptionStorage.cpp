/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RobotDescriptionStorage.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(ROBOTDESCRIPTIONSTORAGE, "yarp.device.robotDescriptionStorage")
}

yarp::dev::ReturnValue RobotDescriptionStorage::getAllDevices(std::vector<yarp::dev::DeviceDescription>& dev_list)
{
    dev_list = m_devices;
    return ReturnValue_ok;
}

yarp::dev::ReturnValue RobotDescriptionStorage::getAllDevicesByType(const std::string &type, std::vector<yarp::dev::DeviceDescription>& dev_list)
{
    dev_list.clear();
    for (auto it = m_devices.begin(); it != m_devices.end(); it++)
    {
        if (it->device_type == type)
        {
            dev_list.push_back(*it);
        }
    }
    return ReturnValue_ok;
}

yarp::dev::ReturnValue RobotDescriptionStorage::registerDevice(const yarp::dev::DeviceDescription& dev)
{
    if (add_device(dev)) {
        return ReturnValue_ok;
    }
    return ReturnValue::return_code::return_value_error_method_failed;
}

yarp::dev::ReturnValue RobotDescriptionStorage::unregisterDevice(const std::string& device_name)
{
    if (remove_device(device_name)) {
        return ReturnValue_ok;
    }
    return ReturnValue::return_code::return_value_error_method_failed;
}

yarp::dev::ReturnValue RobotDescriptionStorage::unregisterAll()
{
    m_devices.clear();
    return ReturnValue_ok;
}

bool RobotDescriptionStorage::add_device(DeviceDescription dev)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto& m_robot_device : m_devices)
    {
        if (dev.device_name == m_robot_device.device_name)
        {
            yCWarning(ROBOTDESCRIPTIONSTORAGE) << "add_device(): Device" << dev.device_name << "already exists, skipping";
            return false;
        }
    }
    m_devices.push_back(dev);
    return true;
}

bool RobotDescriptionStorage::remove_device(const std::string& device_name)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto it = m_devices.begin(); it != m_devices.end(); it++)
    {
        if (device_name == it->device_name)
        {
            m_devices.erase(it);
            return true;
        }
    }
    return false;
}
