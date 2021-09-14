/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWC_YARP
#define YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWC_YARP

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>
#include "MobileBaseVelocityControlRPC.h"

#include <mutex>
#include <string>

/**
 *  @ingroup dev_impl_network_clients dev_impl_navigation
 *
 * \section MobileBaseVelocityControl_nwc_yarp
 *
 * \brief `MobileBaseVelocityControl_nwc_yarp`: A device which connects to MobileBaseVelocityControl_nws_yarp to control the velocity of a mobile base.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the device.                             |       |
 * | server         |     -          | string  | -              |   -           | Yes          | Full port name of the port remotely opened by the server, to which this client connects to.           |  |
 * | carrier        |     -          | string  | -   | tcp           | No           | The carier used for the connection with the server.          |  |
 */

class MobileBaseVelocityControl_nwc_yarp:
        public yarp::dev::DeviceDriver,
        public yarp::dev::Nav2D::INavigation2DVelocityActions
{
protected:
    std::mutex                    m_mutex;
    yarp::os::Port                m_rpc_port;
    std::string                   m_local_name;
    std::string                   m_server_name;
    std::string                   m_carrier;
    MobileBaseVelocityControlRPC  m_RPC;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* The following methods belong to INavigation2D interface */
    bool   applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout = 0.1) override;
    bool   getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel) override;
};

#endif // YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWC_YARP
