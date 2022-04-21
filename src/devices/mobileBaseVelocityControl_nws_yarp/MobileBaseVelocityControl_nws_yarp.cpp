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

void VelocityInputPortProcessor::onRead(yarp::dev::MobileBaseVelocity& v)
{
    if (m_iVel)
    {
        m_iVel->applyVelocityCommand(v.vel_x, v.vel_y, v.vel_theta, m_timeout);
    }
    else
    {
        yCError(MOBVEL_NWS_YARP, "received data on streaming port, but no device attached yet");
    }
}


//------------------------------------------------------------------------------------------------------------------------------

bool MobileBaseVelocityControl_nws_yarp::open(yarp::os::Searchable &config)
{
    //param configuration
    m_local_name = config.find("local").asString();

    if (m_local_name.empty())
    {
        yCError(MOBVEL_NWS_YARP, "open() error you have to provide a valid 'local' param");
        return false;
    }

    //rpc block
    {
        std::string local_rpc_1;
        local_rpc_1           = m_local_name + ":rpc";

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
    }

    //streaming input block
    {
        std::string local_stream_1;
        local_stream_1 = m_local_name + ":i";

        if (!m_StreamingInput.open(local_stream_1))
        {
            yCError(MOBVEL_NWS_YARP, "open() error could not open port %s, check network", local_stream_1.c_str());
            return false;
        }
        m_StreamingInput.setStrict();
        m_StreamingInput.useCallback();
    }

    //attach subdevice if required
    if (config.check("subdevice"))
    {
        Property       p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!m_subdev.open(p) || !m_subdev.isValid())
        {
            yCError(MOBVEL_NWS_YARP) << "Failed to open subdevice.. check params";
            return false;
        }

        if (!attach(&m_subdev))
        {
            yCError(MOBVEL_NWS_YARP) << "Failed to attach subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(MOBVEL_NWS_YARP) << "Waiting for device to attach";
    }

    return true;
}

bool MobileBaseVelocityControl_nws_yarp::close()
{
    m_rpc_port_navigation_server.close();
    m_StreamingInput.close();
    if (m_subdev.isValid()) { m_subdev.close(); }
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
    m_StreamingInput.m_iVel = m_iNavVel;

    return true;
}

bool MobileBaseVelocityControl_nws_yarp::applyVelocityCommandRPC(const double x_vel, const double y_vel, const double theta_vel, const double timeout)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    if (nullptr == m_iNavVel)
    {
        yCError(MOBVEL_NWS_YARP, "Unable to applyVelocityCommandRPC");
        return false;
    }

    bool b =m_iNavVel->applyVelocityCommand(x_vel, y_vel, theta_vel, timeout);

    if (!b)
    {
        yCError(MOBVEL_NWS_YARP, "Unable to applyVelocityCommandRPC");
        return false;
    }
    return true;
}

return_getLastVelocityCommand MobileBaseVelocityControl_nws_yarp::getLastVelocityCommandRPC()
{
    return_getLastVelocityCommand retrievedFromRPC;
    std::lock_guard <std::mutex> lg(m_mutex);
    if (nullptr == m_iNavVel)
    {
        yCError(MOBVEL_NWS_YARP, "Unable to getLastVelocityCommand");
        retrievedFromRPC.retvalue = false;
        return retrievedFromRPC;
    }

    double x_vel = 0;
    double y_vel = 0;
    double t_vel = 0;
    bool b = m_iNavVel->getLastVelocityCommand(x_vel, y_vel, t_vel);

    retrievedFromRPC.retvalue = b;
    retrievedFromRPC.x_vel = x_vel;
    retrievedFromRPC.y_vel = x_vel;
    retrievedFromRPC.theta_vel = t_vel;
    return retrievedFromRPC;
}
