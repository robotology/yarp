/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
YARP_LOG_COMPONENT(FRAMETRANSFORSETNWSYARP, "yarp.device.frameTransformSet_nws_yarp")
}

//------------------------------------------------------------------------------------------------------------------------------

FrameTransformSet_nws_yarp::FrameTransformSet_nws_yarp(double tperiod) :
PeriodicThread(tperiod),
m_thriftPortName("/frameTransformSet/rpc")
{
}

bool FrameTransformSet_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORSETNWSYARP,"Error! YARP Network is not initialized");
        return false;
    }

    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("period"))         {m_period = general_config.find("period").asDouble();}
        if (general_config.check("rpc_port"))       {m_thriftPortName = general_config.find("thrift_port").asString();}
    }
    if(!m_thriftPort.open(m_thriftPortName))
    {
        yCError(FRAMETRANSFORSETNWSYARP,"Could not open \"%s\" port",m_thriftPortName.c_str());
        return false;
    }
    if(!this->yarp().attachAsServer(m_thriftPort))
    {
        yCError(FRAMETRANSFORSETNWSYARP,"Error! Cannot attach the port as a server");
        return false;
    }

    return true;
}

bool FrameTransformSet_nws_yarp::close()
{
    return true;
}

void FrameTransformSet_nws_yarp::run()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    return;
}

bool FrameTransformSet_nws_yarp::detachAll()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
#ifndef SINGLE_SET
    m_iSetIfs.clear();
#else
    m_iSetIf = nullptr;
#endif
    return true;
}

bool FrameTransformSet_nws_yarp::setTransform(const yarp::math::FrameTransform& transform)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
#ifndef SINGLE_SET
    for (int i=0; i<m_iSetIfs.size(); i++)
    {
        if (!m_iSetIfs[i]->setTransform(transform))
        {
            yCError(FRAMETRANSFORSETNWSYARP, "Unable to set transform");
            return false;
        }
    }
#else
    if(!m_iSetIf->setTransform(transform))
    {
        yCError(FRAMETRANSFORSETNWSYARP, "Unable to set transform");
        return false;
    }
#endif
    return true;
}

bool FrameTransformSet_nws_yarp::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
#ifndef SINGLE_SET
    for (int i=0; i<m_iSetIfs.size(); i++)
    {
        if (!m_iSetIfs[i]->setTransforms(transforms))
        {
            yCError(FRAMETRANSFORSETNWSYARP, "Unable to set transformations");
            return false;
        }
    }
#else
    if(!m_iSetIf->setTransforms(transforms))
    {
        yCError(FRAMETRANSFORSETNWSYARP, "Unable to set transformations");
        return false;
    }
#endif
    return true;
}

bool FrameTransformSet_nws_yarp::attachAll(const yarp::dev::PolyDriverList& device2attach)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    m_pDriverList = device2attach;

#ifndef SINGLE_SET
    for (size_t i = 0; i < m_pDriverList.size(); i++)
    {
        yarp::dev::PolyDriver* pd = m_pDriverList[i]->poly;
        if (pd->isValid())
        {
            IFrameTransformStorageSet* pp=nullptr;
            if (pd->view(pp) && pp!=nullptr)
            {
                m_iSetIfs.push_back(pp);
            }
        }
    }
#else
    if (m_pDriverList.size() > 1)
    {
        yCError(FRAMETRANSFORSETNWSYARP,"Only one device can be attached");
        return false;
    }
    yarp::dev::PolyDriver* pd = m_pDriverList[0]->poly;
    if(pd->isValid())
    {
        IFrameTransformStorageSet* pp=nullptr;
        if (pd->view(pp) && pp!=nullptr)
        {
            m_iSetIf = pp;
        }
    }
#endif
    return true;
}
