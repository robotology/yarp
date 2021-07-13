/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformGet_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>


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

    // configuration
    if (config.check("rpc_port_server")){
        m_thrift_rpcPort_Name = config.find("rpc_port_server").asString();
    }
    if (config.check("streaming_port_server")) {
        m_streaming_port_name = config.find("streaming_port_server").asString();
    }
    if (config.check("streaming_enabled")) {
        m_streaming_port_enabled = config.find("streaming_enabled").asBool();
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
        yCInfo(FRAMETRANSFORMGETNWSYARP) << "Streaming transforms on Yarp port enabled";
        this->start();
    }
    else
    {
        yCInfo(FRAMETRANSFORMGETNWSYARP) << "Streaming transforms on Yarp port NOT enabled";
    }

    return true;
}


bool FrameTransformGet_nws_yarp::close()
{
    if (m_streaming_port_enabled)
    {
        this->stop();
    }

    yCTrace(FRAMETRANSFORMGETNWSYARP, "Close");
    detach();
    // Closing port
    m_thrift_rpcPort.interrupt();
    m_thrift_rpcPort.close();
    return true;
}


bool FrameTransformGet_nws_yarp::detach()
{
    m_iFrameTransformStorageGet = nullptr;
    return true;
}


bool FrameTransformGet_nws_yarp::attach( yarp::dev::PolyDriver* deviceToAttach)
{
    deviceToAttach->view(m_iFrameTransformStorageGet);

    if ( m_iFrameTransformStorageGet==nullptr){
        yCError(FRAMETRANSFORMGETNWSYARP) << "could not attach to the device";
        return false;
    }
    return true;
}


return_getAllTransforms FrameTransformGet_nws_yarp::getTransformsRPC()
{
    if (m_iFrameTransformStorageGet != nullptr) {
        std::vector<yarp::math::FrameTransform> localTransform;
        if (m_iFrameTransformStorageGet->getTransforms(localTransform)) {
            return return_getAllTransforms(true, localTransform);
        }
    }
    yCError(FRAMETRANSFORMGETNWSYARP) << "error getting transform from interface";
    return return_getAllTransforms(false, std::vector<yarp::math::FrameTransform>());;

}

bool FrameTransformGet_nws_yarp::threadInit()
{
    if (!m_streaming_port.open(m_streaming_port_name))
    {
        yCError(FRAMETRANSFORMGETNWSYARP, "Could not open \"%s\" port", m_streaming_port_name.c_str());
        return false;
    }
    return true;
}

void FrameTransformGet_nws_yarp::threadRelease()
{
    m_streaming_port.interrupt();
    m_streaming_port.close();
}

void FrameTransformGet_nws_yarp::run()
{
    if (m_iFrameTransformStorageGet != nullptr)
    {
        std::vector<yarp::math::FrameTransform> localTransform;
        if (m_iFrameTransformStorageGet->getTransforms(localTransform))
        {
            return_getAllTransforms rgt(true, localTransform);
            m_streaming_port.write(rgt);
        }
    }
}
