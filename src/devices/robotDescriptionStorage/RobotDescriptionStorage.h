/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ROBOTDESCRIPTIONSTORAGE_H
#define YARP_DEV_ROBOTDESCRIPTIONSTORAGE_H

#include <mutex>
#include <string>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/IRobotDescription.h>
#include <yarp/dev/IMultipleWrapper.h>

/**
* @ingroup dev_impl_nws_yarp
*
* \brief `RobotDescription_nws_yarp`: This device is a storage which contains a list of the currently opened device drivers.
*
* yarprobotinterface might adds/removes devices to the storage using attachAll()/detachAll() methods.
*
* This device does not accepts parameters
*/
class RobotDescriptionStorage : public yarp::dev::DeviceDriver,
                                public yarp::dev::IRobotDescription,
                                public yarp::dev::IMultipleWrapper
{
    private:
    std::recursive_mutex                      m_recmutex;
    std::vector<yarp::dev::DeviceDescription> m_devices;

    private:
    bool add_device(yarp::dev::DeviceDescription dev);
    bool remove_device(const std::string& device_name);

    public:
    RobotDescriptionStorage( )= default;
    virtual ~RobotDescriptionStorage() = default;
    bool open(yarp::os::Searchable& params) override { return true; }
    bool close() override { return true; }

    public:
    bool detachAll() override;
    bool attachAll(const yarp::dev::PolyDriverList &l) override;

    public:
    yarp::dev::ReturnValue getAllDevices(std::vector<yarp::dev::DeviceDescription>& dev_list) override;
    yarp::dev::ReturnValue getAllDevicesByType(const std::string &type, std::vector<yarp::dev::DeviceDescription>& dev_list) override;
    yarp::dev::ReturnValue registerDevice(const yarp::dev::DeviceDescription& dev) override;
    yarp::dev::ReturnValue unregisterDevice(const std::string& device_name) override;
    yarp::dev::ReturnValue unregisterAll() override;
};

#endif // YARP_DEV_ROBOTDESCRIPTIONSTORAGE_H
