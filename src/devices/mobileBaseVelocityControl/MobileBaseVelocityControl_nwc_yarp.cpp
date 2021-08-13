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

    ok = Network::connect(local_rpc_1, remote_rpc_1);
    if (!ok)
    {
        yCError(MOBVEL_NWC_YARP, "open() error could not connect to %s", remote_rpc_1.c_str());
        return false;
    }

    return true;
}

bool MobileBaseVelocityControl_nwc_yarp::close()
{
    m_rpc_port.close();
    return true;
}

bool  MobileBaseVelocityControl_nwc_yarp::applyVelocityCommand(double x_vel, double y_vel, double theta_vel, double timeout)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_INAVIGATION);
    b.addVocab32(VOCAB_NAV_VELOCITY_CMD);
    b.addFloat64(x_vel);
    b.addFloat64(y_vel);
    b.addFloat64(theta_vel);
    b.addFloat64(timeout);

    bool ret = m_rpc_port.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(MOBVEL_NWC_YARP) << "applyVelocityCommand() received error from navigation server";
            return false;
        }
    }
    else
    {
        yCError(MOBVEL_NWC_YARP) << "applyVelocityCommand() error on writing on rpc port";
        return false;
    }

    return true;
}
