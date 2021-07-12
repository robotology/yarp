/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformSet_nws_yarp.h"

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

bool FrameTransformSet_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMSETNWSYARP,"Error! YARP Network is not initialized");
        return false;
    }
    if (config.check("rpc_port_server"))       {m_thriftPortName = config.find("rpc_port_server").asString();}
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
    if(m_thriftPort.isOpen())
    {
        m_thriftPort.close();
    }
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

    if(!m_pDriver->isValid()/* || (!m_pDriver->view(m_iSetIf) || m_iSetIf==nullptr)*/)
    {
        yCError(FRAMETRANSFORMSETNWSYARP, "Attach failed");
        return false;
    }
    return true;
}
