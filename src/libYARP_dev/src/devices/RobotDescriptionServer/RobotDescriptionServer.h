/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ROBOTDESCRIPTIONSERVER_H
#define YARP_DEV_ROBOTDESCRIPTIONSERVER_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/ConstString.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/IRobotDescription.h>

namespace yarp {
    namespace dev {
        class RobotDescriptionServer;
    }
}

/**
* @ingroup dev_impl_wrapper
* This device is a storage which contains a list of the currently opened device drivers.
* yarprobotinterfaces adds/removes devices to the storage using attachAll()/detachAll() methods.
* A robotDescriptionClient devices can bused by the user to retrieve information about the currently opened devices.
*
* Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | local          |      -         | string  | -              |   -           | Yes                         | prefix of the port opened by the device, like /descriptionServer  | MUST start with a '/' character. /rpc is automatically appended. |
*/

class yarp::dev::RobotDescriptionServer : public DeviceDriver, public yarp::os::PortReader, public yarp::dev::IMultipleWrapper

{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:

    yarp::os::Mutex                m_external_mutex;
    yarp::os::Mutex                m_internal_mutex;
    yarp::os::Port                 m_rpc_port;
    yarp::os::ConstString          m_local_name;
    std::vector<DeviceDescription> m_robot_devices;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    virtual bool detachAll() override;
    virtual bool attachAll(const yarp::dev::PolyDriverList &l) override;
    virtual bool read(yarp::os::ConnectionReader& connection) override;

private:
    bool add_device(DeviceDescription dev);
    bool remove_device(DeviceDescription dev);
};

#endif // YARP_DEV_ROBOTDESCRIPTIONSERVER_H
