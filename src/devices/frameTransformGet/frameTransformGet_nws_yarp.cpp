/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "frameTransformGet_nws_yarp.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>


YARP_LOG_COMPONENT(FRAMETRANSFORMGETNWSYARP, "yarp.devices.FrameTransformGet_nws_yarp")

FrameTransformGet_nws_yarp::FrameTransformGet_nws_yarp() :
PeriodicThread(DEFAULT_THREAD_PERIOD),
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
    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("period"))         {m_period = general_config.find("period").asFloat64();}
        if (general_config.check("rpc_port"))       {m_thrift_rpcPort_Name = general_config.find("thrift_port").asString();}
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

    if(m_verbose <= 2)
        yCTrace(FRAMETRANSFORMGETNWSYARP) << "\nParameters are: \n" << config.toString();

    return true;
}


bool FrameTransformGet_nws_yarp::close()
{
    yCTrace(FRAMETRANSFORMGETNWSYARP, "Close");
    detachAll();
    // Closing port
    m_thrift_rpcPort.interrupt();
    m_thrift_rpcPort.close();
    return true;
}


bool FrameTransformGet_nws_yarp::detachAll()
{
    if (yarp::os::PeriodicThread::isRunning())
    {
        yarp::os::PeriodicThread::stop();
    }
    return true;
}


bool FrameTransformGet_nws_yarp::attachAll(const yarp::dev::PolyDriverList& devices2attach)
{
    switch (devices2attach.size())
    {
    case 2:
        // yarp tf repeater and nwc ros or ros2
    case 3:
        // yarp tf repeater, nwc ros and nwc ros2
        for (int i = 0; i < devices2attach.size(); i++)
        {
            yarp::dev::PolyDriver* polyDriverLocal = devices2attach[i]->poly;
            if (polyDriverLocal->isValid())
            {
                yarp::dev::IFrameTransformStorageGet* iFrameTransformStorageGet=nullptr;
                if (polyDriverLocal->view(iFrameTransformStorageGet) && iFrameTransformStorageGet!=nullptr)
                {
                    m_iFrameTransformStorageGetList.push_back(iFrameTransformStorageGet);
                }
            }
        }
        yarp::os::PeriodicThread::setPeriod(m_period);
        return yarp::os::PeriodicThread::start();

    default:
        yCError(FRAMETRANSFORMGETNWSYARP) << "need to attach 2 or 3 devices, " << devices2attach.size() << "found";
        break;
    }
    return false;
}


void FrameTransformGet_nws_yarp::run()
{
    return;
}


return_getAllTransforms FrameTransformGet_nws_yarp::getTransforms()
{
    std::vector<yarp::math::FrameTransform> allTransform;
    for (size_t i = 0; i < m_iFrameTransformStorageGetList.size(); i++)
    {
        if (m_iFrameTransformStorageGetList[i] != nullptr) {
            std::vector<yarp::math::FrameTransform> localTransform;
            m_iFrameTransformStorageGetList[i]->getTransforms(localTransform);
            allTransform.insert(allTransform.end(),
                                localTransform.begin(),
                                localTransform.end());
        }
        else {
            yCError(FRAMETRANSFORMGETNWSYARP) << "pointer to interface IFrameTransformStorageGet not valid";
            return return_getAllTransforms(false, std::vector<yarp::math::FrameTransform>());;
        }
    }
    return return_getAllTransforms(true, allTransform);;
}
