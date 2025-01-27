/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformGet_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/ReturnValue.h>

using namespace yarp::dev;

YARP_LOG_COMPONENT(FRAMETRANSFORMGETNWSYARP, "yarp.devices.FrameTransformGet_nws_yarp")

FrameTransformGet_nws_yarp::FrameTransformGet_nws_yarp() :
PeriodicThread (0.010),
m_thrift_rpcPort_Name("/frameTransformGet/rpc")
{
}


/** Device driver interface */
bool FrameTransformGet_nws_yarp::open(yarp::os::Searchable &config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMGETNWSYARP,"Error! YARP Network is not initialized");
        return false;
    }
    std::string prefix;
    //checking default config param
    bool default_config = true;
    if(config.check("default-config")) {
        default_config = config.find("default-config").asString() == "true";
    }
    // configuration
    if (config.check("nws_thrift_port_prefix")){
        prefix = config.find("nws_thrift_port_prefix").asString() + (default_config ? m_defaultConfigPrefix : "");
        if(prefix[0] != '/') {prefix = "/"+prefix;}
        m_thrift_rpcPort_Name = prefix + "/" + m_deviceName + "/thrift";
    }
    else {
        prefix =  default_config ? m_defaultConfigPrefix : "";
        m_thrift_rpcPort_Name = prefix + "/" + m_deviceName + "/thrift";
        yCWarning(FRAMETRANSFORMGETNWSYARP) << "no nws_thrift_port_prefix param found. The resulting port name will be: " << m_thrift_rpcPort_Name;
    }
    if(config.check("streaming_enabled")) {
        m_streaming_port_enabled = config.find("streaming_enabled").asString() == "true";
    }
    if (config.check("period")) {
        double period = config.find("period").asFloat64();
        this->setPeriod(period);
    }

    // rpc initialization
    if(!m_thrift_rpcPort.open(m_thrift_rpcPort_Name))
    {
        yCError(FRAMETRANSFORMGETNWSYARP,"Could not open \"%s\" port",m_thrift_rpcPort_Name.c_str());
        return false;
    }
    if(!this->yarp().attachAsServer(m_thrift_rpcPort))
    {
        yCError(FRAMETRANSFORMGETNWSYARP,"Error! Cannot attach the port as a server");
        return false;
    }
    if(m_verbose <= 2) {
        yCTrace(FRAMETRANSFORMGETNWSYARP) << "\nParameters are: \n" << config.toString();
    }

    if (m_streaming_port_enabled)
    {
        if (config.check("output_streaming_port_prefix")){
            prefix = config.find("output_streaming_port_prefix").asString() + (default_config ? m_defaultConfigPrefix : "");
            if(prefix[0] != '/') {prefix = "/"+prefix;}
            m_streaming_port_name = prefix + "/" + m_deviceName + "/tf:o";
        }
        else {
            prefix =  default_config ? m_defaultConfigPrefix : "";
            m_streaming_port_name = prefix + "/" + m_deviceName + "/tf:o";
            yCWarning(FRAMETRANSFORMGETNWSYARP) << "no output_streaming_port_prefix param found. The resulting port name will be: " << m_streaming_port_name;
        }
        yCInfo(FRAMETRANSFORMGETNWSYARP) << "Streaming transforms on Yarp port enabled";
        if (!m_streaming_port.open(m_streaming_port_name))
        {
            yCError(FRAMETRANSFORMGETNWSYARP, "Could not open \"%s\" port", m_streaming_port_name.c_str());
            return false;
        }

        this->start();
    }
    else
    {
        yCInfo(FRAMETRANSFORMGETNWSYARP) << "Streaming transforms on Yarp port NOT enabled";
    }

    return true;
}

FrameTransformGet_nws_yarp::~FrameTransformGet_nws_yarp()
{
    close();
}

bool FrameTransformGet_nws_yarp::close()
{
    // Stop the thread
    if (this->isRunning())
    {
        this->stop();
    }

    // Detaching
    detach();

    // Closing port
    m_thrift_rpcPort.interrupt();
    m_thrift_rpcPort.close();
    m_streaming_port.interrupt();
    m_streaming_port.close();
    return true;
}


bool FrameTransformGet_nws_yarp::detach()
{
    std::lock_guard<std::mutex> m_lock(m_mutex);
    m_iFrameTransformStorageGet = nullptr;
    return true;
}


bool FrameTransformGet_nws_yarp::attach( yarp::dev::PolyDriver* deviceToAttach)
{
    std::lock_guard<std::mutex> m_lock(m_mutex);
    deviceToAttach->view(m_iFrameTransformStorageGet);

    if ( m_iFrameTransformStorageGet==nullptr){
        yCError(FRAMETRANSFORMGETNWSYARP) << "could not attach to the device";
        return false;
    }
    return true;
}


return_getAllTransforms FrameTransformGet_nws_yarp::getTransformsRPC()
{
    std::lock_guard<std::mutex> m_lock(m_mutex);

    return_getAllTransforms ret;
    if (m_iFrameTransformStorageGet != nullptr)
    {
        std::vector<yarp::math::FrameTransform> localTransform;
        if (m_iFrameTransformStorageGet->getTransforms(localTransform))
        {
            ret.transforms_list = localTransform;
            ret.retvalue = ReturnValue_ok;
            return ret;
        }
    }
    yCError(FRAMETRANSFORMGETNWSYARP) << "error getting transform from interface";
    ret.transforms_list = std::vector<yarp::math::FrameTransform>();
    ret.retvalue = ReturnValue::return_code::return_value_error_method_failed;
    return ret;

}

void FrameTransformGet_nws_yarp::run()
{
    std::lock_guard<std::mutex> m_lock(m_mutex);

    if (m_iFrameTransformStorageGet != nullptr)
    {
        std::vector<yarp::math::FrameTransform> localTransform;
        if (m_iFrameTransformStorageGet->getTransforms(localTransform))
        {
            return_getAllTransforms rgt;
            rgt.retvalue = ReturnValue_ok;
            rgt.transforms_list = localTransform;
            m_streaming_port.write(rgt);
        }
    }
}
