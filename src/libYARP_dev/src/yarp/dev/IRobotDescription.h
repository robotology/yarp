/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IROBOTDESCRIPTION_H
#define YARP_DEV_IROBOTDESCRIPTION_H

#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>
#include <yarp/dev/DeviceDescriptionData.h>

#include <vector>
#include <string>

namespace yarp::dev {

class YARP_dev_API DeviceDescription final: public DeviceDescriptionData
{
 public:
    bool operator ==(DeviceDescription const& b) const
    {
        return this->device_name == b.device_name && this->device_type == b.device_type;
    }
};

/**
 * @ingroup dev_iface_motor
 *
 * This interface allows users to retrieve a list which contains the names and the types of the currently running devices.
 * This list is stored in a RobotDescriptionsServer class. The user can access to the data opening a RobotDescriptionsClient in its own module.
 */
class YARP_dev_API IRobotDescription
{
public:
    /**
     * Destructor.
     */
    virtual ~IRobotDescription();

    /**
     * Ask the complete list of all yarp device drivers registered by a robot description server.
     */
    virtual yarp::dev::ReturnValue getAllDevices(std::vector<DeviceDescription>& dev_list) = 0;

    /**
     * Ask a list of all registered yarp device drivers whose type corresponds to the given param.
     */
    virtual yarp::dev::ReturnValue getAllDevicesByType(const std::string &type, std::vector<DeviceDescription>& dev_list) = 0;

    /**
     * Register a new running yarp device into a robot description server.
     */
    virtual yarp::dev::ReturnValue registerDevice(const DeviceDescription& dev) = 0;

    /**
     * Unregister a running yarp device from a robot description server.
     */
    virtual yarp::dev::ReturnValue unregisterDevice(const std::string& device_name) = 0;

    /**
     * Unregister all the devices
     */
    virtual yarp::dev::ReturnValue unregisterAll() = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_IROBOTDESCRIPTION_H
