/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ROBOTDESCRIPTIONSTORAGE_H
#define YARP_DEV_ROBOTDESCRIPTIONSTORAGE_H

#include <mutex>
#include <string>

#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/IRobotDescription.h>

class RobotDescriptionStorage : public yarp::dev::IRobotDescription
{
    private:
    std::mutex                                m_mutex;
    std::vector<yarp::dev::DeviceDescription> m_devices;

    private:
    bool add_device(yarp::dev::DeviceDescription dev);
    bool remove_device(const std::string& device_name);

    public:
    RobotDescriptionStorage( )= default;
    virtual ~RobotDescriptionStorage() = default;

    public:
    yarp::dev::ReturnValue getAllDevices(std::vector<yarp::dev::DeviceDescription>& dev_list) override;
    yarp::dev::ReturnValue getAllDevicesByType(const std::string &type, std::vector<yarp::dev::DeviceDescription>& dev_list) override;
    yarp::dev::ReturnValue registerDevice(const yarp::dev::DeviceDescription& dev) override;
    yarp::dev::ReturnValue unregisterDevice(const std::string& device_name) override;
    yarp::dev::ReturnValue unregisterAll() override;
};

#endif // YARP_DEV_ROBOTDESCRIPTIONSTORAGE_H
