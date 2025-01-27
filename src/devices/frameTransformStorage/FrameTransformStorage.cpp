/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FrameTransformStorage.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORSTORAGE, "yarp.device.frameTransformStorage")
}

//------------------------------------------------------------------------------------------------------------------------------
yarp::dev::ReturnValue FrameTransformStorage::getInternalContainer(FrameTransformContainer*& container)
{
    container = &m_tf_container;
    return ReturnValue_ok;
}

bool FrameTransformStorage::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) { return false; }

    std::string sstr = config.toString();

    if (m_FrameTransform_verbose_debug)
    {
        m_tf_container.m_verbose_debug=true;
        m_tf_container.m_name = this->id() + ".container";
    }

    //if (m_FrameTransform_container_timeout>0) //should I check?
    {
        m_tf_container.m_timeout = m_FrameTransform_container_timeout;
    }

    yCTrace(FRAMETRANSFORSTORAGE);
    bool b = this->start();
    return b;
}

bool FrameTransformStorage::close()
{
    detach();
    return true;
}

yarp::dev::ReturnValue FrameTransformStorage::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    return m_tf_container.getTransforms(transforms);
}

yarp::dev::ReturnValue FrameTransformStorage::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    return m_tf_container.setTransforms(transforms);
}

yarp::dev::ReturnValue FrameTransformStorage::setTransform(const yarp::math::FrameTransform& t)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    return m_tf_container.setTransform (t);
}

yarp::dev::ReturnValue FrameTransformStorage::deleteTransform(std::string t1, std::string t2)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    return m_tf_container.deleteTransform(t1,t2);
}

void FrameTransformStorage::run()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);

    // remove expired non static transforms
    m_tf_container.checkAndRemoveExpired();

    // get new transforms
    if (iGetIf)
    {
        std::vector<yarp::math::FrameTransform> tfs;
        bool b=iGetIf->getTransforms(tfs);
        if (b)
        {
            m_tf_container.setTransforms(tfs);
        }
    }
}

bool FrameTransformStorage::detach()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    iGetIf = nullptr;
    pDriver = nullptr;
    if(isRunning())
    {
        stop();
    }
    return true;
}

bool FrameTransformStorage::attach(yarp::dev::PolyDriver* driver)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if (driver->isValid())
    {
        pDriver = driver;
        pDriver->view(iGetIf);
        if(iGetIf)
        {
            return true;
        }
    }

    return false;
}

yarp::dev::ReturnValue FrameTransformStorage::clearAll()
{
    return m_tf_container.clearAll();
}

yarp::dev::ReturnValue FrameTransformStorage::size(size_t& size) const
{
    if (m_tf_container.size(size))
    {
        return ReturnValue_ok;
    }
    return ReturnValue::return_code::return_value_error_method_failed;
}

yarp::dev::ReturnValue FrameTransformStorage::startStorageThread()
{
    if (this->start())
    {
        return ReturnValue_ok;
    }
    return ReturnValue::return_code::return_value_error_method_failed;
}

yarp::dev::ReturnValue FrameTransformStorage::stopStorageThread()
{
    this->askToStop();
    do
    {
        yarp::os::Time::delay(0.001);
    }
    while (this->isRunning());
    return ReturnValue_ok;
}
