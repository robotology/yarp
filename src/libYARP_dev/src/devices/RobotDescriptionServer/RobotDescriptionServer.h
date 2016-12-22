/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

    yarp::os::Mutex               m_external_mutex;
    yarp::os::Mutex               m_internal_mutex;
    yarp::os::Port                m_rpc_port;
    yarp::os::ConstString         m_local_name;
    std::vector<RobotDescription> m_robot_devices;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();

    virtual bool detachAll();
    virtual bool attachAll(const yarp::dev::PolyDriverList &l);
    virtual bool read(yarp::os::ConnectionReader& connection);

private:
    bool add_device(RobotDescription dev);
    bool remove_device(RobotDescription dev);
};

#endif // YARP_DEV_ROBOTDESCRIPTIONSERVER_H
