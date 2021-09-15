/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "MobileBaseVelocityControl_nwc_yarp.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

/*! \file MobileBaseVelocityControl_nwc_yarp.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(MOBVEL_NWC_YARP, "yarp.device.MobileBaseVelocityControl_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool MobileBaseVelocityControl_nwc_yarp::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_server_name.clear();

    m_local_name = config.find("local").asString();
    m_server_name = config.find("server").asString();
    m_carrier = config.check("carrier", yarp::os::Value("tcp"), "the carrier used for the connection with the server").asString();

    if (m_local_name.empty())
    {
        yCError(MOBVEL_NWC_YARP, "open() error you have to provide a valid 'local' param");
        return false;
    }

    if (m_server_name.empty())
    {
        yCError(MOBVEL_NWC_YARP, "open() error you have to provide a valid 'navigation_server' param");
        return false;
    }

    std::string local_rpc_1;
    std::string remote_rpc_1;

    local_rpc_1           = m_local_name  + "/navigation/rpc";
    remote_rpc_1          = m_server_name + "/rpc";

    if (!m_rpc_port.open(local_rpc_1))
    {
        yCError(MOBVEL_NWC_YARP, "open() error could not open rpc port %s, check network", local_rpc_1.c_str());
        return false;
    }

    bool ok = true;

    ok = Network::connect(local_rpc_1, remote_rpc_1, m_carrier);
    if (!ok)
    {
        yCError(MOBVEL_NWC_YARP, "open() error could not connect to %s", remote_rpc_1.c_str());
        return false;
    }

    if (!m_RPC.yarp().attachAsClient(m_rpc_port))
    {
        yCError(MOBVEL_NWC_YARP, "Error! Cannot attach the port as a client");
        return false;
    }

    return true;
}

bool MobileBaseVelocityControl_nwc_yarp::close()
{
    m_rpc_port.close();
    return true;
}

bool MobileBaseVelocityControl_nwc_yarp::getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_RPC.getLastVelocityCommandRPC();
    if (!ret.retvalue)
    {
        yCError(MOBVEL_NWC_YARP, "Unable to set transformation");
        return false;
    }
    x_vel = ret.x_vel;
    y_vel = ret.y_vel;
    theta_vel = ret.theta_vel;
    return true;
}

bool MobileBaseVelocityControl_nwc_yarp::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_RPC.applyVelocityCommandRPC(x_vel,y_vel,theta_vel,timeout);
}
