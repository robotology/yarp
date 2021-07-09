/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ROBOTDESCRIPTION_CLIENT_H
#define YARP_DEV_ROBOTDESCRIPTION_CLIENT_H

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <mutex>
#include <string>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRobotDescription.h>

/**
* @ingroup dev_impl_network_clients
*
* \brief `robotDescriptionClient`: This client device is used to connect to a robotDescriptionServer and ask info about the currently opened devices.
*
* Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                                 | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:---------------------------------------------------------------------------:|:-----:|
* | local          |      -         | string  | -              |   -           | Yes                         | prefix of the port opened by the device, like /my_module/descriptionClient  | MUST start with a '/' character. /rpc is automatically appended. |
* | remote         |      -         | string  | -              |   -           | Yes                         | prefix of the port opened by the robotDescriptionServer, like /descriptionServer.  | MUST start with a '/' character. /rpc is automatically appended. |
*/

class RobotDescriptionClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRobotDescription
{
protected:
    std::mutex          m_mutex;
    yarp::os::Port      m_rpc_port;
    std::string         m_local_name;
    std::string         m_remote_name;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /**
    * Ask the complete list of all yarp device drivers registered by a robot description server.
    */
    bool getAllDevices(std::vector<yarp::dev::DeviceDescription>& dev_list) override;

    /**
    * Ask a list of all registered yarp device drivers whose type corresponds to the given param.
    */
    bool getAllDevicesByType(const std::string &type, std::vector<yarp::dev::DeviceDescription>& dev_list) override;

    /**
    * Register a new running yarp device into a robot description server.
    */
    bool registerDevice(const yarp::dev::DeviceDescription& dev) override;

    /**
    * Unregister a running yarp device from a robot description server.
    */
    bool unregisterDevice(const std::string& device_name) override;
};

#endif // YARP_DEV_ROBOTDESCRIPTION_CLIENT_H
