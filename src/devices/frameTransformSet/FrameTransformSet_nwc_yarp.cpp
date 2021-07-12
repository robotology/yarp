/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformSet_nwc_yarp.h"

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
    // client port configuration
    if (config.check("rpc_port_client")){
        m_thriftPortName = config.find("rpc_port_client").asString();
    } else {
        yCWarning(FRAMETRANSFORMSETNWCYARP) << "no rpc_port_client param found, using default one: " << m_thriftPortName;
    }

    //server port configuration
    if (config.check("rpc_port_server")){
        m_thrift_server_rpcPort_Name = config.find("rpc_port_server").asString();
    } else {
        yCWarning(FRAMETRANSFORMSETNWCYARP) << "no rpc_port_server param found, using default one " << m_thrift_server_rpcPort_Name;
    }
    // rpc inizialisation
    if(!m_thriftPort.open(m_thriftPortName))
    {
        yCError(FRAMETRANSFORMSETNWCYARP,"Could not open \"%s\" port",m_thriftPortName.c_str());
        return false;
    }
    // connect to server
    if (!yarp::os::NetworkBase::connect(m_thriftPortName,m_thrift_server_rpcPort_Name))
    {
        yCError(FRAMETRANSFORMSETNWCYARP,"Could not connect \"%s\" to \"%s\" port",m_thriftPortName.c_str(), m_thrift_server_rpcPort_Name.c_str());
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
    if(m_thriftPort.isOpen())
    {
        m_thriftPort.close();
    }
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
