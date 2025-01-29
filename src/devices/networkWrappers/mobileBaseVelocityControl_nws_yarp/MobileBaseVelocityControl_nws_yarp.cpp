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
    if (!parseParams(config)) { return false; }

    //rpc block
    {
        std::string local_rpc_1;
        local_rpc_1           = m_local + "/rpc:i";

        if (!m_rpc_port_navigation_server.open(local_rpc_1))
        {
            yCError(MOBVEL_NWS_YARP, "open() error could not open rpc port %s, check network", local_rpc_1.c_str());
            return false;
        }

        m_rpc_port_navigation_server.setReader(*this);
    }

    //streaming input block
    {
        std::string local_stream_1;
        local_stream_1 = m_local + "/data:i";

        if (!m_StreamingInput.open(local_stream_1))
        {
            yCError(MOBVEL_NWS_YARP, "open() error could not open port %s, check network", local_stream_1.c_str());
            return false;
        }
        m_StreamingInput.setStrict();
        m_StreamingInput.useCallback();
    }

    yCInfo(MOBVEL_NWS_YARP) << "Waiting for device to attach";
    return true;
}

bool MobileBaseVelocityControl_nws_yarp::close()
{
    m_rpc_port_navigation_server.close();
    m_StreamingInput.close();

    detach();
    return true;
}

bool MobileBaseVelocityControl_nws_yarp::detach()
{
    std::lock_guard lock (m_mutex);

    m_iNavVel = nullptr;

    if (m_RPC)
    {
        delete m_RPC;
        m_RPC = nullptr;
    }

    return true;
}

bool MobileBaseVelocityControl_nws_yarp::attach(PolyDriver* driver)
{
    std::lock_guard lock (m_mutex);

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

    m_RPC = new IMobileBaseVelocityControlRPCd(m_iNavVel);

    yCInfo(MOBVEL_NWS_YARP) << "Attach complete";
    return true;
}

bool MobileBaseVelocityControl_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}
    if (!m_RPC) { return false;}

    std::lock_guard<std::mutex> lock(m_mutex);

    bool b = m_RPC->read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCDebug(MOBVEL_NWS_YARP) << "read() Command failed";
        return false;
    }
}
