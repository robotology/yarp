/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "frameTransformSet_nwc_yarp.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORMSETNWCYARP, "yarp.device.frameTransformSet_nwc_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool FrameTransformSet_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMSETNWCYARP,"Error! YARP Network is not initialized");
        return false;
    }

    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        const yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("rpc_port"))       {m_thriftPortName = general_config.find("rpc_port").asString();}
    }
    if(!m_thriftPort.open(m_thriftPortName))
    {
        yCError(FRAMETRANSFORMSETNWCYARP,"Could not open \"%s\" port",m_thriftPortName.c_str());
        return false;
    }
    if (!m_setRPC.yarp().attachAsClient(m_thriftPort))
    {
        yCError(FRAMETRANSFORMSETNWCYARP, "Error! Cannot attach the port as a client");
        return false;
    }

    return true;
}

bool FrameTransformSet_nwc_yarp::close()
{
    return true;
}

bool FrameTransformSet_nwc_yarp::setTransform(const yarp::math::FrameTransform& transform)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if(!m_setRPC.setTransform(transform))
    {
        yCError(FRAMETRANSFORMSETNWCYARP, "Unable to set transformation");
        return false;
    }
    return true;
}

bool FrameTransformSet_nwc_yarp::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if(!m_setRPC.setTransforms(transforms))
    {
        yCError(FRAMETRANSFORMSETNWCYARP, "Unable to set transformations");
        return false;
    }
    return true;
}
