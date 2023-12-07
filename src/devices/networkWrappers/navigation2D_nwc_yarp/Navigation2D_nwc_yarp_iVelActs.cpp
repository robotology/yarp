/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Navigation2D_nwc_yarp.h"
#include <yarp/dev/INavigation2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cmath>

 /*! \file Navigation2D_nwc_yarp_iVelActs.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
    YARP_LOG_COMPONENT(NAVIGATION2D_NWC_YARP, "yarp.device.navigation2D_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool  Navigation2D_nwc_yarp::getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_nav_RPC.get_last_velocity_command_RPC();
    if (!ret.ret)
    {
        yCError(NAVIGATION2D_NWC_YARP, "Unable to get_last_velocity_command_RPC");
        return false;
    }
    x_vel = ret.x_vel;
    y_vel = ret.y_vel;
    theta_vel = ret.theta_vel;
    return true;
}

bool  Navigation2D_nwc_yarp::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return m_nav_RPC.apply_velocity_command_RPC(x_vel,y_vel,theta_vel, timeout);
}
