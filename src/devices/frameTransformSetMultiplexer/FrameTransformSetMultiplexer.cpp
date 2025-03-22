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
            yarp::dev::IFrameTransformStorageSet*   iFrameTransformStorageSet=nullptr;
            yarp::dev::IFrameTransformStorageUtils* iFrameTransformStorageUtils=nullptr;

            //all attached devices must have iFrameTransformStorageSet interface while
            //only FrameTransformStorage have iFrameTransformStorageUtils
            if (polyDriverLocal->view(iFrameTransformStorageSet) && iFrameTransformStorageSet!=nullptr)
            {
                m_iFrameTransformStorageSetList.push_back(iFrameTransformStorageSet);
            }
            else
            {
                yCError(FRAMETRANSFORMSETMULTIPLEXER) << "failed to attach all the devices";
                return false;
            }

            //attempt to iFrameTransformStorageUtils
            polyDriverLocal->view(iFrameTransformStorageUtils);
            m_iFrameTransformStorageUtilsList.push_back(iFrameTransformStorageUtils);
        }
        else
        {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "polydriver not valid";
            return false;
        }
    }
    return true;
}


yarp::dev::ReturnValue FrameTransformSetMultiplexer::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageSetList[i] != nullptr) {
            m_iFrameTransformStorageSetList[i]->setTransforms(transforms);
        }
        else
        {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "pointer to interface IFrameTransformStorageSet not valid";
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    return ReturnValue_ok;
}


yarp::dev::ReturnValue FrameTransformSetMultiplexer::setTransform(const yarp::math::FrameTransform& transform)
{
    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageSetList[i] != nullptr) {
            m_iFrameTransformStorageSetList[i]->setTransform(transform);
        }
        else
        {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "pointer to interface IFrameTransformStorageSet not valid";
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    return ReturnValue_ok;
}

yarp::dev::ReturnValue FrameTransformSetMultiplexer::deleteTransform(std::string t1, std::string t2)
{
    //stopThreads();

    bool frame_deleted = true;

    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageSetList[i] != nullptr)
        {
            frame_deleted &= m_iFrameTransformStorageSetList[i]->deleteTransform(t1,t2);
        }
        else
        {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "pointer to interface IFrameTransformStorageSet not valid";
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }

    //startThreads();

    if (frame_deleted) { return ReturnValue_ok; }

    return ReturnValue::return_code::return_value_error_method_failed;
}

yarp::dev::ReturnValue FrameTransformSetMultiplexer::clearAll()
{
    //stopThreads();

    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageSetList[i] != nullptr) {
            m_iFrameTransformStorageSetList[i]->clearAll();
        }
        else
        {
            yCError(FRAMETRANSFORMSETMULTIPLEXER) << "pointer to interface IFrameTransformStorageSet not valid";
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    }

    //startThreads();

    return ReturnValue_ok;
}

void FrameTransformSetMultiplexer::stopThreads()
{
    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageUtilsList[i] != nullptr) {
            m_iFrameTransformStorageUtilsList[i]->stopStorageThread();
        }
    }
}

void FrameTransformSetMultiplexer::startThreads()
{
    for (size_t i = 0; i < m_iFrameTransformStorageSetList.size(); i++)
    {
        if (m_iFrameTransformStorageUtilsList[i] != nullptr) {
            m_iFrameTransformStorageUtilsList[i]->startStorageThread();
        }
    }
}
