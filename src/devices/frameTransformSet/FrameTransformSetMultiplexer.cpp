/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformSetMultiplexer.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(FRAMETRANSFORMSETMULTIPLEXER, "yarp.devices.FrameTransformSetMultiplexer")


/** Device driver interface */
bool FrameTransformSetMultiplexer::open(yarp::os::Searchable &config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMSETMULTIPLEXER,"Error! YARP Network is not initialized");
        return false;
    }
    return true;
}



bool FrameTransformSetMultiplexer::detachAll()
{
    m_iFrameTransformStorageSetList.clear();
    return true;
}


bool FrameTransformSetMultiplexer::close()
{
    yCTrace(FRAMETRANSFORMSETMULTIPLEXER, "Close");
    detachAll();
    return true;
}


bool FrameTransformSetMultiplexer::attachAll(const yarp::dev::PolyDriverList& devices2attach)
{
    for (int i = 0; i < devices2attach.size(); i++)
    {
        yarp::dev::PolyDriver* polyDriverLocal = devices2attach[i]->poly;
        if (polyDriverLocal->isValid())
        {
            yarp::dev::IFrameTransformStorageSet* iFrameTransformStorageSet=nullptr;
            if (polyDriverLocal->view(iFrameTransformStorageSet) && iFrameTransformStorageSet!=nullptr)
            {
                m_iFrameTransformStorageSetList.push_back(iFrameTransformStorageSet);
            }
            else
            {
                yCError(FRAMETRANSFORMSETMULTIPLEXER) << "failed to attach all the devices";
                return false;
            }
        }
        else
        {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "polydriver not valid";
            return false;
        }
    }
    return true;
}


bool FrameTransformSetMultiplexer::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageSetList[i] != nullptr) {
            m_iFrameTransformStorageSetList[i]->setTransforms(transforms);
        }
        else {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "pointer to interface IFrameTransformStorageSet not valid";
            return false;
        }
    }
    return true;
}


bool FrameTransformSetMultiplexer::setTransform(const yarp::math::FrameTransform& transform)
{
    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageSetList[i] != nullptr) {
            m_iFrameTransformStorageSetList[i]->setTransform(transform);
        }
        else {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "pointer to interface IFrameTransformStorageSet not valid";
            return false;
        }
    }
    return true;
}

bool FrameTransformSetMultiplexer::deleteTransform(std::string t1, std::string t2)
{
    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageSetList[i] != nullptr) {
            m_iFrameTransformStorageSetList[i]->deleteTransform(t1,t2);
        }
        else {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "pointer to interface IFrameTransformStorageSet not valid";
            return false;
        }
    }
    return true;
}

bool FrameTransformSetMultiplexer::clearAll()
{
    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageSetList[i] != nullptr) {
            m_iFrameTransformStorageSetList[i]->clearAll();
        }
        else {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "pointer to interface IFrameTransformStorageSet not valid";
            return false;
        }
    }
    return true;
}
