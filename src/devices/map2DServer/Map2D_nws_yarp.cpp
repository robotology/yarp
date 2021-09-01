/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Map2D_nws_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>

#include <yarp/dev/GenericVocabs.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>

#include <yarp/math/Math.h>

#include <yarp/rosmsg/TickDuration.h>
#include <yarp/rosmsg/TickTime.h>

#include <cstdlib>
#include <fstream>
#include <limits>
#include <mutex>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(MAP2D_NWS_YARP, "yarp.device.map2D_nws_yarp")
}

/**
  * Map2D_nws_yarp
  */

Map2D_nws_yarp::Map2D_nws_yarp()
{
}

Map2D_nws_yarp::~Map2D_nws_yarp() = default;

bool Map2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }
    yCDebug(MAP2D_NWS_YARP) << "read() Command failed";
    return false;
}

bool Map2D_nws_yarp::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (!config.check("name"))
    {
        m_rpcPortName = "/map2D_nws_yarp/rpc";
    }
    else
    {
        m_rpcPortName = config.find("name").asString();
    }

    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yCError(MAP2D_NWS_YARP, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    //subdevice handling
    if (config.check("subdevice"))
    {
        Property       p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!m_drv.open(p) || !m_drv.isValid())
        {
            yCError(MAP2D_NWS_YARP) << "Failed to open subdevice.. check params";
            return false;
        }

        if (!attach(&m_drv))
        {
            yCError(MAP2D_NWS_YARP) << "Failed to open subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(MAP2D_NWS_YARP) << "Waiting for device to attach";
    }

    return true;
}

bool Map2D_nws_yarp::close()
{
    yCTrace(MAP2D_NWS_YARP, "Close");
    return true;
}

bool Map2D_nws_yarp::detach()
{
    m_iMap2D = nullptr;
    return true;
}

bool Map2D_nws_yarp::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(m_iMap2D);
        m_RPC.setInterface(m_iMap2D);
    }

    if (nullptr == m_iMap2D)
    {
        yCError(MAP2D_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    return true;
}
