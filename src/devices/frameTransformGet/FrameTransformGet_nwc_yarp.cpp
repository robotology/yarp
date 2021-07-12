/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformGet_nwc_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

YARP_LOG_COMPONENT(FRAMETRANSFORMGETNWCYARP, "yarp.devices.FrameTransformGet_nwc_yarp")


bool FrameTransformGet_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMGETNWCYARP,"Error! YARP Network is not initialized");
        return false;
    }
    // client port configuration
    if (config.check("rpc_port_client")){
        m_thrift_rpcPort_Name = config.find("rpc_port_client").asString();
    } else {
        yCWarning(FRAMETRANSFORMGETNWCYARP) << "no rpc_port_client param found, using default one: " << m_thrift_rpcPort_Name;
    }

    //server port configuration
    if (config.check("rpc_port_server")){
        m_thrift_server_rpcPort_Name = config.find("rpc_port_server").asString();
    } else {
        yCWarning(FRAMETRANSFORMGETNWCYARP) << "no rpc_port_server param found, using default one " << m_thrift_server_rpcPort_Name;
    }
    // rpc inizialisation
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

    return true;
}


bool FrameTransformGet_nwc_yarp::close()
{
    m_thrift_rpcPort.close();
    return true;
}


bool FrameTransformGet_nwc_yarp::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    return_getAllTransforms retrievedFromRPC = m_frameTransformStorageGetRPC.getTransforms();
    if(!retrievedFromRPC.retvalue)
    {
        yCError(FRAMETRANSFORMGETNWCYARP, "Unable to get transformations");
        return false;
    }
    transforms = retrievedFromRPC.transforms_list;
    return true;
}
