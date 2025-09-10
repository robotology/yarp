/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformGet_nwc_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;

YARP_LOG_COMPONENT(FRAMETRANSFORMGETNWCYARP, "yarp.devices.FrameTransformGet_nwc_yarp")


bool FrameTransformGet_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

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
        m_thrift_rpcPort_Name = prefix + "/" + m_deviceName + "/thrift";
    }
    else {
        prefix =  default_config ? m_defaultConfigPrefix : "";
        m_thrift_rpcPort_Name = prefix + "/" + m_deviceName + "/thrift";
        yCWarning(FRAMETRANSFORMGETNWCYARP) << "no nwc_thrift_port_prefix param found. The resulting port name will be: " << m_thrift_rpcPort_Name;
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
        yCWarning(FRAMETRANSFORMGETNWCYARP) << "no nws_thrift_port_prefix param found. The resulting port name will be: " << m_thrift_server_rpcPort_Name;
    }
    // rpc initialization
    if(!m_thrift_rpcPort.open(m_thrift_rpcPort_Name))
    {
        yCError(FRAMETRANSFORMGETNWCYARP,"Could not open \"%s\" port",m_thrift_rpcPort_Name.c_str());
        return false;
    }
    // connect to server
    if (!yarp::os::NetworkBase::connect(m_thrift_rpcPort_Name,m_thrift_server_rpcPort_Name))
    {
        yCError(FRAMETRANSFORMGETNWCYARP,"Could not connect \"%s\" to \"%s\" port",m_thrift_rpcPort_Name.c_str(), m_thrift_server_rpcPort_Name.c_str());
        return false;
    }
    if (!m_frameTransformStorageGetRPC.yarp().attachAsClient(m_thrift_rpcPort))
    {
        yCError(FRAMETRANSFORMGETNWCYARP, "Error! Cannot attach the port as a client");
        return false;
    }

    //checking streaming_enabled param
    if (m_streaming_enabled)
    {
        yCInfo(FRAMETRANSFORMGETNWCYARP) << "Receiving transforms from Yarp port enabled";
        if (config.check("input_streaming_port_prefix")){
            prefix = config.find("input_streaming_port_prefix").asString() + (default_config ? m_defaultConfigPrefix : "");
            if(prefix[0] != '/') {prefix = "/"+prefix;}
            m_streaming_input_port_name = prefix + "/" + m_deviceName + "/tf:i";
        }
        else {
            prefix =  default_config ? m_defaultConfigPrefix : "";
            m_streaming_input_port_name = prefix + "/" + m_deviceName + "/tf:i";
            yCWarning(FRAMETRANSFORMGETNWCYARP) << "no input_streaming_port_prefix param found. The resulting port name will be: " << m_streaming_input_port_name;
        }

        //server port configuration
        if (config.check("output_streaming_port_prefix")){
            prefix = config.find("output_streaming_port_prefix").asString() + (default_server ? m_defaultServerPrefix : "");
            if(prefix[0] != '/') {prefix = "/"+prefix;}
            m_streaming_output_port_name = prefix + "/tf:o";
        }
        else {
            prefix =  default_server ? m_defaultServerPrefix : "";
            m_streaming_output_port_name = prefix + "/tf:o";
            yCWarning(FRAMETRANSFORMGETNWCYARP) << "no output_streaming_port_prefix param found. The resulting port name will be: " << m_streaming_output_port_name;
        }

        // data receiver initialization
        m_dataReader = new FrameTransformGet_nwc_yarp::DataReader();
        if(!m_dataReader->open(m_streaming_input_port_name))
        {
            yCError(FRAMETRANSFORMGETNWCYARP,"Could not open \"%s\" port",m_streaming_input_port_name.c_str());
            return false;
        }
        // connect to server
        if (!yarp::os::NetworkBase::connect(m_streaming_output_port_name,m_streaming_input_port_name))
        {
            yCError(FRAMETRANSFORMGETNWCYARP,"Could not connect \"%s\" to \"%s\" port",m_streaming_output_port_name.c_str(), m_streaming_input_port_name.c_str());
            return false;
        }
        //useCallback
        m_dataReader->useCallback();
    }
    else
    {
        yCInfo(FRAMETRANSFORMGETNWCYARP) << "Receiving transforms from Yarp port NOT enabled";
    }

    //Check the protocol version
    if (!m_frameTransformStorageGetRPC.checkProtocolVersion()) { return false; }

    yCInfo(FRAMETRANSFORMGETNWCYARP) << "Opening of NWC successful";
    return true;
}


bool FrameTransformGet_nwc_yarp::close()
{
    if (m_streaming_enabled)
    {
        m_dataReader->close();
    }
    m_thrift_rpcPort.close();

    if (m_dataReader)
    {
        delete m_dataReader;
        m_dataReader = nullptr;
    }
    return true;
}


ReturnValue FrameTransformGet_nwc_yarp::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    if (!m_streaming_enabled)
    {
        return_getAllTransforms retrievedFromRPC = m_frameTransformStorageGetRPC.getTransformsRPC();
        if(!retrievedFromRPC.retvalue)
        {
            yCError(FRAMETRANSFORMGETNWCYARP, "Unable to get transformations");
            return retrievedFromRPC.retvalue;
        }
        transforms = retrievedFromRPC.transforms_list;
        return retrievedFromRPC.retvalue;
    }
    else
    {
        if (m_dataReader)
        {
            return_getAllTransforms retrievedFromSteaming;
            m_dataReader->getData(retrievedFromSteaming);
            transforms = retrievedFromSteaming.transforms_list;
            return retrievedFromSteaming.retvalue;
        }
        yCError(FRAMETRANSFORMGETNWCYARP, "Unable to get transformations");
        return yarp::dev::ReturnValue::return_code::return_value_error_generic;
    }
}

bool FrameTransformGet_nwc_yarp::DataReader::getData(return_getAllTransforms& data)
{
    m_mutex.lock();
    data = m_Transforms;
    m_mutex.unlock();
    return true;
}

void FrameTransformGet_nwc_yarp::DataReader::onRead(return_getAllTransforms& v)
{
    m_mutex.lock();
    m_Transforms = v;
    m_mutex.unlock();
}
