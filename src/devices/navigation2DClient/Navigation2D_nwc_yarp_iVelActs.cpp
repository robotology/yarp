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
    YARP_LOG_COMPONENT(NAVIGATION2D_NWC, "yarp.device.navigation2D_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool  Navigation2D_nwc_yarp::getLastVelocityCommand(double& x_vel, double& y_vel, double& theta_vel)
{
    yCError(NAVIGATION2D_NWC) << "getLastVelocityCommand() not yet implemented";
    return false;
}

bool  Navigation2D_nwc_yarp::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_VELOCITY_CMD);
    b.addFloat64(x_vel);
    b.addFloat64(y_vel);
    b.addFloat64(theta_vel);
    b.addFloat64(timeout);

    bool ret = m_rpc_port_to_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(NAVIGATION2D_NWC) << "applyVelocityCommand() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(NAVIGATION2D_NWC) << "applyVelocityCommand() error on writing on rpc port";
        return false;
    }

    return true;
}
