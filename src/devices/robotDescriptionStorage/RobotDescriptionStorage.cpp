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
    std::lock_guard<std::recursive_mutex> guard(m_recmutex);
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
    std::lock_guard<std::recursive_mutex> guard(m_recmutex);
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

bool RobotDescriptionStorage::attachAll(const PolyDriverList &p)
{
    std::lock_guard<std::recursive_mutex> guard(m_recmutex);

    bool raise_error = false;
    for (int i = 0; i < p.size(); i++)
    {
        PolyDriverDescriptor* pd = (PolyDriverDescriptor*)p[i];

        DeviceDescription dev;
        dev.device_name = pd->key;
        dev.device_type = pd->poly->id();
        if (dev.device_name == "")
        {
            yCError(ROBOTDESCRIPTIONSTORAGE) << "Invalid device name for device type:" << dev.device_type << ", counter id:" << i;
            raise_error = true;
            continue;
        }
        if (dev.device_type.empty())
        {
            yCError(ROBOTDESCRIPTIONSTORAGE) << "Invalid device type for device name:" << dev.device_name << ", counter id:" << i;
            raise_error = true;
            continue;
        }

        auto ret = registerDevice(dev);
        if (!ret)
        {
            yCError(ROBOTDESCRIPTIONSTORAGE) << "Unable to register device";
            raise_error = true;
            continue;
        }
    }

    if (raise_error)
    {
        yCError(ROBOTDESCRIPTIONSTORAGE) << "attachAll() encountered one or more errors. Not all devices have been registered";
        //return false; //This was intentionally disabled. Yarprobotinterface will close if one attach fails.
    }
    return true;
}

bool RobotDescriptionStorage::detachAll()
{
    //Clear the storage
    unregisterAll();

    return true;
}
