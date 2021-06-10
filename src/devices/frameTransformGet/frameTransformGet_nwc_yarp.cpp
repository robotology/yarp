/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "frameTransformGet_nwc_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

YARP_LOG_COMPONENT(FRAMETRANSFORMGETNWCYARP, "yarp.devices.FrameTransformGet_nwc_yarp")


bool FrameTransformGet_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMGETNWCYARP,"Error! YARP Network is not initialized");
        return false;
    }

    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("rpc_port"))       {m_thrift_rpcPort_Name = general_config.find("thrift_port").asString();}
    }
    if(!m_thrift_rpcPort.open(m_thrift_rpcPort_Name))
    {
        yCError(FRAMETRANSFORMGETNWCYARP,"Could not open \"%s\" port",m_thrift_rpcPort_Name.c_str());
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
