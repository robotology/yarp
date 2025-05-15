/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ROBOTDESCRIPTION_NWC_YARP_H
#define YARP_DEV_ROBOTDESCRIPTION_NWC_YARP_H

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


#include "RobotDescription_nwc_yarp_ParamsParser.h"

/**
* @ingroup dev_impl_nwc_yarp
*
* \brief `robotDescription_nwc_yarp`: This client device is used to connect to a robotDescription_nws_yarp and ask info about the currently opened devices.
*
* Parameters required by this device are shown in class: RobotDescription_nwc_yarp_ParamsParser
*/

class RobotDescription_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRobotDescription,
        public RobotDescription_nwc_yarp_ParamsParser
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

#endif // YARP_DEV_ROBOTDESCRIPTION_NWC_YARP_H
