/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "INavigation2DServerImpl.h"

/*! \file INavigation2DServerImpl.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace std;

namespace {
YARP_LOG_COMPONENT(NAVIGATION2DSERVER, "yarp.device.localization2DServer")
}

#define CHECK_POINTER(xxx) {if (xxx==nullptr) {yCError(NAVIGATION2DSERVER, "Invalid interface"); return false;}}

void INavigation2DRPCd::setInterfaces(yarp::dev::Nav2D::INavigation2DTargetActions* iNav_target, yarp::dev::Nav2D::INavigation2DControlActions* iNav_ctrl, yarp::dev::Nav2D::INavigation2DVelocityActions* iNav_vel)
{
    m_iNav_target = iNav_target;
    m_iNav_ctrl = iNav_ctrl;
    m_iNav_vel = iNav_vel;
}

bool INavigation2DRPCd::stop_navigation_RPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    {if (m_iNav_ctrl == nullptr) { yCError(NAVIGATION2DSERVER, "Invalid interface"); return false; }}

    if (!m_iNav_ctrl->stopNavigation())
    {
        yCError(NAVIGATION2DSERVER, "Unable to startLocalizationService");
        return false;
    }
    return true;
}
