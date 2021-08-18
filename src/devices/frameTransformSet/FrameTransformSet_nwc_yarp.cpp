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
    std::string prefix;
    //checking default config params
    bool default_config = true;
    if(config.check("default-client")) {
        default_config = config.find("default-client").asString() == "true";
    }
    bool default_server = true;
    if(config.check("default-server")) {
        default_server = config.find("default-server").asString() == "true";
    }
    // client port configuration
    if (config.check("nwc_thrift_port_prefix")){
        prefix = config.find("nwc_thrift_port_prefix").asString() + (default_config ? m_defaultConfigPrefix : "");
        if(prefix[0] != '/') {prefix = "/"+prefix;}
        m_thriftPortName = prefix + "/" + m_deviceName + "/thrift";
    }
    else {
        prefix =  default_config ? m_defaultConfigPrefix : "";
        m_thriftPortName = prefix + "/" + m_deviceName + "/thrift";
        yCWarning(FRAMETRANSFORMSETNWCYARP) << "no nwc_thrift_port_prefix param found. The resulting port name will be: " << m_thriftPortName;
    }

    //server port configuration
    if (config.check("nws_thrift_port_prefix")){
        prefix = config.find("nws_thrift_port_prefix").asString() + (default_server ? m_defaultServerPrefix : "");
        if(prefix[0] != '/') {prefix = "/"+prefix;}
        m_thrift_server_rpcPort_Name = prefix + "/thrift";
    }
    else {
        prefix =  default_server ? m_defaultServerPrefix : "";
        m_thrift_server_rpcPort_Name = prefix + "/thrift";
        yCWarning(FRAMETRANSFORMSETNWCYARP) << "no nws_thrift_port_prefix param found. The resulting port name will be: " << m_thrift_server_rpcPort_Name;
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
    if(!m_setRPC.setTransformRPC(transform))
    {
        yCError(FRAMETRANSFORMSETNWCYARP, "Unable to set transformation");
        return false;
    }
    return true;
}

bool FrameTransformSet_nwc_yarp::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if(!m_setRPC.setTransformsRPC(transforms))
    {
        yCError(FRAMETRANSFORMSETNWCYARP, "Unable to set transformations");
        return false;
    }
    return true;
}

bool FrameTransformSet_nwc_yarp::deleteTransform(std::string t1, std::string t2)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if (!m_setRPC.deleteTransformRPC(t1,t2))
    {
        yCError(FRAMETRANSFORMSETNWCYARP, "Unable to delete transformation");
        return false;
    }
    return true;
}

bool FrameTransformSet_nwc_yarp::clearAll()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if (!m_setRPC.clearAllRPC())
    {
        yCError(FRAMETRANSFORMSETNWCYARP, "Unable to clear all transformations");
        return false;
    }
    return true;
}
