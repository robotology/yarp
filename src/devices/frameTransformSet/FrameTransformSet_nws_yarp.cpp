/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformSet_nws_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORMSETNWSYARP, "yarp.device.frameTransformSet_nws_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

bool FrameTransformSet_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMSETNWSYARP,"Error! YARP Network is not initialized");
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
        m_thriftPortName = prefix + "/" + m_deviceName + "/thrift";
    }
    else {
        prefix =  default_config ? m_defaultConfigPrefix : "";
        m_thriftPortName = prefix + "/" + m_deviceName + "/thrift";
        yCWarning(FRAMETRANSFORMSETNWSYARP) << "no nws_thrift_port_prefix param found. The resulting port name will be: " << m_thriftPortName;
    }

    if(!m_thriftPort.open(m_thriftPortName))
    {
        yCError(FRAMETRANSFORMSETNWSYARP,"Could not open \"%s\" port",m_thriftPortName.c_str());
        return false;
    }
    if(!this->yarp().attachAsServer(m_thriftPort))
    {
        yCError(FRAMETRANSFORMSETNWSYARP,"Error! Cannot attach the port as a server");
        return false;
    }

    return true;
}

bool FrameTransformSet_nws_yarp::close()
{
    if(m_thriftPort.isOpen())
    {
        m_thriftPort.close();
    }
    return true;
}

bool FrameTransformSet_nws_yarp::detach()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    m_iSetIf = nullptr;
    return true;
}

bool FrameTransformSet_nws_yarp::setTransformRPC(const yarp::math::FrameTransform& transform)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);

    if(!m_iSetIf->setTransform(transform))
    {
        yCError(FRAMETRANSFORMSETNWSYARP, "Unable to set transform");
        return false;
    }

    return true;
}

bool FrameTransformSet_nws_yarp::setTransformsRPC(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if(!m_iSetIf->setTransforms(transforms))
    {
        yCError(FRAMETRANSFORMSETNWSYARP, "Unable to set transformations");
        return false;
    }

    return true;
}

bool FrameTransformSet_nws_yarp::deleteTransformRPC(const std::string& src, const std::string& dst)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if (!m_iSetIf->deleteTransform(src,dst))
    {
        yCError(FRAMETRANSFORMSETNWSYARP, "Unable to delete transforms");
        return false;
    }

    return true;
}

bool FrameTransformSet_nws_yarp::clearAllRPC()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if (!m_iSetIf->clearAll())
    {
        yCError(FRAMETRANSFORMSETNWSYARP, "Unable to clear all transforms");
        return false;
    }

    return true;
}


bool FrameTransformSet_nws_yarp::attach(yarp::dev::PolyDriver* device2attach)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    m_pDriver = device2attach;

    if(!m_pDriver->isValid() ||
      (!m_pDriver->view(m_iSetIf)   || m_iSetIf == nullptr) ||
      (!m_pDriver->view(m_iUtilsIf) || m_iUtilsIf == nullptr))
    {
        yCError(FRAMETRANSFORMSETNWSYARP, "Attach failed");
        return false;
    }
    return true;
}
