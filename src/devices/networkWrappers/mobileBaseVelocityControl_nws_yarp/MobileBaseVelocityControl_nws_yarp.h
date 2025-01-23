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
#include "MobileBaseVelocityControlServerImpl.h"
#include <yarp/dev/MobileBaseVelocity.h>

#include <mutex>
#include <string>

#include "MobileBaseVelocityControl_nws_yarp_ParamsParser.h"

class VelocityInputPortProcessor : public yarp::os::BufferedPort<yarp::dev::MobileBaseVelocity>
{
public:
    double m_timeout = 0.1;
    yarp::dev::Nav2D::INavigation2DVelocityActions* m_iVel = nullptr;

public:
    using yarp::os::BufferedPort<yarp::dev::MobileBaseVelocity>::onRead;
    void onRead(yarp::dev::MobileBaseVelocity& v) override;
};

/**
 *  @ingroup dev_impl_network_clients dev_impl_navigation
 *
 * \section MobileBaseVelocityControl_nws_yarp
 *
 * \brief `MobileBaseVelocityControl_nws_yarp`: A device which allows a client to control the velocity of a mobile base from YARP.
 * The device opens two ports: a streaming port `/exampleName/data:i` for receiving streaming commands, and a rpc port `/exampleName/rpc:i` for rpc connection with
 * a `MobileBaseVelocityControl_nwc_yarp` client device.
 *
 * Check velocityInputHandler or fakeNavigation as an example of device which can be attached to MobileBaseVelocityControl_nws_yarp
 *
 * Parameters required by this device are shown in class: MobileBaseVelocityControl_nws_yarp_ParamsParser
 *
*/
class MobileBaseVelocityControl_nws_yarp:
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public MobileBaseVelocityControl_nws_yarp_ParamsParser
{
protected:
    std::mutex                    m_mutex;
    yarp::os::Port                m_rpc_port_navigation_server;
    VelocityInputPortProcessor    m_StreamingInput;

    yarp::dev::Nav2D::INavigation2DVelocityActions* m_iNavVel = nullptr;

    //thrift
    IMobileBaseVelocityControlRPCd*                 m_RPC = nullptr;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool detach() override;
    bool attach(yarp::dev::PolyDriver* driver) override;

public:
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_MOBILEBASEVELOCITYCONTROL_NWS_YARP
