/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_ROBOTDESCRIPTION_CLIENT_H
#define YARP_DEV_ROBOTDESCRIPTION_CLIENT_H

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
#include <yarp/dev/IRobotDescription.h>

namespace yarp {
    namespace dev {
        class RobotDescriptionClient;
    }
}

/**
* @ingroup dev_impl_wrapper
* This client device is used to connect to a robotDescriptionServer and ask info about the currently opened devices.
*
* Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                                 | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:---------------------------------------------------------------------------:|:-----:|
* | local          |      -         | string  | -              |   -           | Yes                         | prefix of the port opened by the device, like /my_module/descriptionClient  | MUST start with a '/' character. /rpc is automatically appended. |
* | remote         |      -         | string  | -              |   -           | Yes                         | prefix of the port opened by the robotDescriptionServer, like /descriptionServer.  | MUST start with a '/' character. /rpc is automatically appended. |
*/

class yarp::dev::RobotDescriptionClient : public DeviceDriver,
                                       public IRobotDescription
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    yarp::os::Mutex               m_mutex;
    yarp::os::Port                m_rpc_port;
    yarp::os::ConstString         m_local_name;
    yarp::os::ConstString         m_remote_name;

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config);
    bool close();

    /**
    * Ask the complete list of all yarp device drivers registered by a robot description server.
    */
    bool getAllDevices(std::vector<RobotDescription>& dev_list);

    /**
    * Ask a list of all registered yarp device drivers whose type corresponds to the given param.
    */
    bool getAllDevicesByType(const std::string &type, std::vector<RobotDescription>& dev_list);

    /**
    * Register a new running yarp device into a robot description server.
    */
    bool registerDevice(const RobotDescription& dev);
};

#endif // YARP_DEV_ROBOTDESCRIPTION_CLIENT_H
