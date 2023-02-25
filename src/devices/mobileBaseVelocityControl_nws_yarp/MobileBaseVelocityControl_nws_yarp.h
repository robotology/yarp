/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWS_YARP
#define YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWS_YARP

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/WrapperSingle.h>
#include "MobileBaseVelocityControlRPC.h"
#include <yarp/dev/MobileBaseVelocity.h>

#include <mutex>
#include <string>

/**
 *  @ingroup dev_impl_network_clients dev_impl_navigation
 *
 * \section MobileBaseVelocityControl_nws_yarp
 *
 * \brief `MobileBaseVelocityControl_nws_yarp`: A device which allows a client to control the velocity of a mobile base from YARP.
 * The device opens two ports: a streaming port `/exampleName/data:i` for receiving streaming commands, and a rpc port `/exampleName/rpc:i` for rpc connection with
 * a `MobileBaseVelocityControl_nwc_yarp` client device.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units | Default Value | Required | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:-----:|:-------------:|:-------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -     |   -           | Yes      | Full name of the port opened by the device. For both ports (i.e. /rpc:i, /data:i) the corresponding suffix is automatically added |       |
 * | subdevice      |      -         | string  | -     |   -           | No       | name of the subdevice to instantiate                              | when used, parameters for the subdevice must be provided as well |
 *
 * Example usage:
 * yarpdev --device mobileBaseVelocityControl_nws_yarp --subdevice velocityInputHandler --local /input1
*/

class VelocityInputPortProcessor : public yarp::os::BufferedPort<yarp::dev::MobileBaseVelocity>
{
public:
    double m_timeout = 0.1;
    yarp::dev::Nav2D::INavigation2DVelocityActions* m_iVel = nullptr;

public:
    using yarp::os::BufferedPort<yarp::dev::MobileBaseVelocity>::onRead;
    void onRead(yarp::dev::MobileBaseVelocity& v) override;
};

class MobileBaseVelocityControl_nws_yarp:
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public MobileBaseVelocityControlRPC
{
protected:
    std::mutex                    m_mutex;
    yarp::os::Port                m_rpc_port_navigation_server;
    VelocityInputPortProcessor    m_StreamingInput;
    std::string                   m_local_name;

    yarp::dev::PolyDriver                           m_subdev;
    yarp::dev::Nav2D::INavigation2DVelocityActions* m_iNavVel = nullptr;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool detach() override;
    bool attach(yarp::dev::PolyDriver* driver) override;

public:
    //* MobileBaseVelocityControlRPC methods*/
    bool applyVelocityCommandRPC(const double x_vel, const double y_vel, const double theta_vel, const double timeout) override;
    return_getLastVelocityCommand getLastVelocityCommandRPC() override;
};

#endif // YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWS_YARP
