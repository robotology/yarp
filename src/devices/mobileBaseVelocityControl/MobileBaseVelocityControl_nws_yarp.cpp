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

    m_rpc_port_navigation_server.setReader(*this);
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

    if (nullptr == MOBVEL_NWS_YARP)
    {
        yCError(MOBVEL_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    return true;
}

bool MobileBaseVelocityControl_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }

    if (command.get(0).isVocab32() == false)
    {
        yCError(MOBVEL_NWS_YARP) << "General error";
        return false;
    }

    if (command.get(0).asVocab32() != VOCAB_INAVIGATION ||
        command.get(1).isVocab32() == false)
    {
        yCError(MOBVEL_NWS_YARP) << "Invalid vocab received";
        reply.addVocab32(VOCAB_ERR);
        return true;
    }

    int request = command.get(1).asVocab32();
    if (request == VOCAB_NAV_VELOCITY_CMD)
    {
        double x_vel = command.get(2).asFloat64();
        double y_vel = command.get(3).asFloat64();
        double t_vel = command.get(4).asFloat64();
        double timeout = command.get(5).asFloat64();
        bool ret = m_iNavVel->applyVelocityCommand(x_vel, y_vel, t_vel, timeout);
        if (ret)
        {
            //clear_current_goal_name();
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            yCError(MOBVEL_NWS_YARP) << "applyVelocityCommand() failed";
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else
    {
        yCError(MOBVEL_NWS_YARP) << "Invalid vocab received:" << yarp::os::Vocab32::decode(request);
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter* returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }
    else
    {
        yCError(MOBVEL_NWS_YARP) << "Invalid return to sender";
    }
    return true;
}
