/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ROBOTDESCRIPTION_CLIENT_H
#define YARP_DEV_ROBOTDESCRIPTION_CLIENT_H

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <mutex>
#include <string>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRobotDescription.h>

#include "IRobotDescriptionMsgs.h"


#include "RobotDescriptionClient_ParamsParser.h"

/**
* @ingroup dev_impl_network_clients
*
* \brief `robotDescriptionClient`: This client device is used to connect to a robotDescriptionServer and ask info about the currently opened devices.
*
* Parameters required by this device are shown in class: RobotDescriptionClient_ParamsParser
*/

class RobotDescriptionClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRobotDescription,
        public RobotDescriptionClient_ParamsParser
{
protected:
    std::mutex              m_mutex;
    yarp::os::Port          m_rpc_port;
    IRobotDescriptionMsgs   m_RPC;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    yarp::dev::ReturnValue getAllDevices(std::vector<yarp::dev::DeviceDescription>& dev_list) override;
    yarp::dev::ReturnValue getAllDevicesByType(const std::string &type, std::vector<yarp::dev::DeviceDescription>& dev_list) override;
    yarp::dev::ReturnValue registerDevice(const yarp::dev::DeviceDescription& dev) override;
    yarp::dev::ReturnValue unregisterDevice(const std::string& device_name) override;
    yarp::dev::ReturnValue unregisterAll() override;
};

#endif // YARP_DEV_ROBOTDESCRIPTION_CLIENT_H
