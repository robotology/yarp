/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ROBOTDESCRIPTIONSERVER_H
#define YARP_DEV_ROBOTDESCRIPTIONSERVER_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <mutex>
#include <yarp/os/Time.h>
#include <string>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/IRobotDescription.h>

/**
* @ingroup dev_impl_wrapper
*
* \brief `robotDescriptionServer`: This device is a storage which contains a list of the currently opened device drivers.
*
* yarprobotinterfaces adds/removes devices to the storage using attachAll()/detachAll() methods.
* A robotDescriptionClient devices can bused by the user to retrieve information about the currently opened devices.
*
* Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | local          |      -         | string  | -              |   -           | Yes                         | prefix of the port opened by the device, like /descriptionServer  | MUST start with a '/' character. /rpc is automatically appended. |
*/

class RobotDescriptionServer :
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader,
        public yarp::dev::IMultipleWrapper
{
protected:
    std::mutex                                m_external_mutex;
    std::mutex                                m_internal_mutex;
    yarp::os::Port                            m_rpc_port;
    std::string                               m_local_name;
    std::vector<yarp::dev::DeviceDescription> m_robot_devices;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    bool detachAll() override;
    bool attachAll(const yarp::dev::PolyDriverList &l) override;
    bool read(yarp::os::ConnectionReader& connection) override;

private:
    bool add_device(yarp::dev::DeviceDescription dev);
    bool remove_device(yarp::dev::DeviceDescription dev);
};

#endif // YARP_DEV_ROBOTDESCRIPTIONSERVER_H
