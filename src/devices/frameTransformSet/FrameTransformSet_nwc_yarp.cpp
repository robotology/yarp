/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

FrameTransformSet_nwc_yarp::FrameTransformSet_nwc_yarp() :
m_thriftPortName("/frameTransformSet/rpc")
{
}

bool FrameTransformSet_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMSETNWCYARP,"Error! YARP Network is not initialized");
        return false;
    }

    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("rpc_port"))       {m_thriftPortName = general_config.find("thrift_port").asString();}
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
