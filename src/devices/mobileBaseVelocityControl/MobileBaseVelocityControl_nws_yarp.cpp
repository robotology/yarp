/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "MobileBaseVelocityControl_nws_yarp.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

/*! \file MobileBaseVelocityControl_nws_yarp.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(MOBVEL_NWS_YARP, "yarp.device.MobileBaseVelocityControl_nws_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool MobileBaseVelocityControl_nws_yarp::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_local_name = config.find("local").asString();

    if (m_local_name.empty())
    {
        yCError(MOBVEL_NWS_YARP, "open() error you have to provide a valid 'local' param");
        return false;
    }

    std::string local_rpc_1;

    local_rpc_1           = m_local_name + "/navigation/rpc";

    if (!m_rpc_port_navigation_server.open(local_rpc_1))
    {
        yCError(MOBVEL_NWS_YARP, "open() error could not open rpc port %s, check network", local_rpc_1.c_str());
        return false;
    }

    if (!this->yarp().attachAsServer(m_rpc_port_navigation_server))
    {
        yCError(MOBVEL_NWS_YARP, "Error! Cannot attach the port as a server");
        return false;
    }

    return true;
}

bool MobileBaseVelocityControl_nws_yarp::close()
{
    m_rpc_port_navigation_server.close();
    return true;
}

bool MobileBaseVelocityControl_nws_yarp::detach()
{
    m_iNavVel = nullptr;
    return true;
}

bool MobileBaseVelocityControl_nws_yarp::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(m_iNavVel);
    }

    if (nullptr == m_iNavVel)
    {
        yCError(MOBVEL_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    return true;
}

bool MobileBaseVelocityControl_nws_yarp::applyVelocityCommandRPC(const double x_vel, const double y_vel, const double theta_vel, const double timeout)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    if (!m_iNavVel->applyVelocityCommand(x_vel,y_vel,theta_vel,timeout))
    {
        yCError(MOBVEL_NWS_YARP, "Unable to applyVelocityCommandRPC");
        return false;
    }
    return true;
}

return_getLastVelocityCommand MobileBaseVelocityControl_nws_yarp::getLastVelocityCommandRPC()
{
    double x_vel = 0;
    double y_vel = 0;
    double t_vel = 0;
    bool b = m_iNavVel->getLastVelocityCommand(x_vel, y_vel, t_vel);

    return_getLastVelocityCommand retrievedFromRPC;
    retrievedFromRPC.retvalue = b;
    retrievedFromRPC.x_vel = x_vel;
    retrievedFromRPC.y_vel = x_vel;
    retrievedFromRPC.theta_vel = t_vel;
    return retrievedFromRPC;
}
