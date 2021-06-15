/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "frameTransformSet_nws_yarp.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORMSETNWSYARP, "yarp.device.frameTransformSet_nws_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

FrameTransformSet_nws_yarp::FrameTransformSet_nws_yarp() :
m_thriftPortName("/frameTransformSet/rpc"),
m_pDriver(nullptr)
{
}

bool FrameTransformSet_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMSETNWSYARP,"Error! YARP Network is not initialized");
        return false;
    }

    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("rpc_port"))       {m_thriftPortName = general_config.find("rpc_port").asString();}
    }
    if(!m_thriftPort.open(m_thriftPortName))
    {
        yCError(FRAMETRANSFORMSETNWSYARP,"Could not open \"%s\" port",m_thriftPortName.c_str());
        return false;
    }
    if(!this->yarp().attachAsServer(m_thriftPort))
    {
        yCError(FRAMETRANSFORMSETNWSYARP,"Error! Cannot attach the port as a server");
        return false;
    }

    return true;
}

bool FrameTransformSet_nws_yarp::close()
{
    return true;
}

bool FrameTransformSet_nws_yarp::detach()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    m_iSetIf = nullptr;
    return true;
}

bool FrameTransformSet_nws_yarp::setTransform(const yarp::math::FrameTransform& transform)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);

    if(!m_iSetIf->setTransform(transform))
    {
        yCError(FRAMETRANSFORMSETNWSYARP, "Unable to set transform");
        return false;
    }

    return true;
}

bool FrameTransformSet_nws_yarp::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if(!m_iSetIf->setTransforms(transforms))
    {
        yCError(FRAMETRANSFORMSETNWSYARP, "Unable to set transformations");
        return false;
    }

    return true;
}

bool FrameTransformSet_nws_yarp::attach(yarp::dev::PolyDriver* device2attach)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    m_pDriver = device2attach;

    if(m_pDriver->isValid())
    {
        if (!m_pDriver->view(m_iSetIf) || m_iSetIf==nullptr)
        {
            yCError(FRAMETRANSFORMSETNWSYARP, "Attach failed");
            return false;
        }
    }
    return true;
}
