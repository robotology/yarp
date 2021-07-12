/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformGet_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>


YARP_LOG_COMPONENT(FRAMETRANSFORMGETNWSYARP, "yarp.devices.FrameTransformGet_nws_yarp")

FrameTransformGet_nws_yarp::FrameTransformGet_nws_yarp() :
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
    else {
        yCError(FRAMETRANSFORMGETNWSYARP) << "error, no rpc_port_server param found";
        return false;
    }

    // rpc inizialisation
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
    return true;
}


bool FrameTransformGet_nws_yarp::close()
{
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


return_getAllTransforms FrameTransformGet_nws_yarp::getTransforms()
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
