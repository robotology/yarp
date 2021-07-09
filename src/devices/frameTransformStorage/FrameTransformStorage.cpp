/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FrameTransformStorage.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORSTORAGE, "yarp.device.frameTransformStorage")
}

//------------------------------------------------------------------------------------------------------------------------------
bool FrameTransformStorage::getInternalContainer(FrameTransformContainer*& container)
{
    container = &m_tf_container;
    return true;
}

bool FrameTransformStorage::open(yarp::os::Searchable& config)
{
    yCTrace(FRAMETRANSFORSTORAGE);
    return true;
}

bool FrameTransformStorage::close()
{
    return true;
}

bool FrameTransformStorage::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    return m_tf_container.getTransforms(transforms);
}

bool FrameTransformStorage::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    return m_tf_container.setTransforms(transforms);
}

bool FrameTransformStorage::setTransform(const yarp::math::FrameTransform& t)
{
    return m_tf_container.setTransform (t);
}

bool FrameTransformStorage::deleteTransform(string t1, string t2)
{
    return m_tf_container.deleteTransform(t1,t2);
}

void FrameTransformStorage::run()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);

    // remove expired non static transforms
    m_tf_container.checkAndRemoveExpired();

    // get new transforms
    std::vector<yarp::math::FrameTransform> tfs;
    bool b=iGetIf->getTransforms(tfs);
    if (b) this->setTransforms(tfs);
}

bool FrameTransformStorage::detach()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    iGetIf = nullptr;
    pDriver = nullptr;
    return true;
}

bool FrameTransformStorage::attach(yarp::dev::PolyDriver* driver)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    if (driver->isValid())
    {
        pDriver = driver;
        if (pDriver->view(iGetIf) && iGetIf!=nullptr)
        {
            return true;
        }
    }

    return false;
}

bool FrameTransformStorage::clear()
{
    return m_tf_container.clear();
}

bool FrameTransformStorage::size(size_t& size) const
{
    return m_tf_container.size(size);
}
