/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "MobileBaseVelocityControlServerImpl.h"

/*! \file MobileBaseVelocityControlServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;


namespace {
YARP_LOG_COMPONENT(MOBVEL_NWS_YARP, "yarp.device.MobileBaseVelocityControl_nws_yarp.IMobileBaseVelocityControlRPCd")
}

ReturnValue IMobileBaseVelocityControlRPCd::applyVelocityCommandRPC(const double x_vel, const double y_vel, const double theta_vel, const double timeout)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret =m_iNavVel->applyVelocityCommand(x_vel, y_vel, theta_vel, timeout);

    if (!ret)
    {
        yCError(MOBVEL_NWS_YARP, "Unable to applyVelocityCommandRPC");
    }
    return ret;
}

return_getLastVelocityCommand IMobileBaseVelocityControlRPCd::getLastVelocityCommandRPC()
{
    return_getLastVelocityCommand retrievedFromRPC;
    std::lock_guard <std::mutex> lg(m_mutex);

    double x_vel = 0;
    double y_vel = 0;
    double t_vel = 0;
    retrievedFromRPC.ret = m_iNavVel->getLastVelocityCommand(x_vel, y_vel, t_vel);

    if (!retrievedFromRPC.ret)
    {
        yCError(MOBVEL_NWS_YARP, "Unable to getLastVelocityCommandRPC");
    }

    retrievedFromRPC.x_vel = x_vel;
    retrievedFromRPC.y_vel = y_vel;
    retrievedFromRPC.theta_vel = t_vel;
    return retrievedFromRPC;
}
