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
    for (size_t i = 0; i < iGetIf.size(); i++)
    {
        std::vector<yarp::math::FrameTransform> tfs;
        bool b=iGetIf[i]->getTransforms(tfs);
        if (b) this->setTransforms(tfs);
    }
}

bool FrameTransformStorage::detachAll()
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    iGetIf.clear();
    return true;
}

bool FrameTransformStorage::attachAll(const yarp::dev::PolyDriverList& device2attach)
{
    std::lock_guard <std::mutex> lg(m_pd_mutex);
    pDriverList = device2attach;

    for (size_t i = 0; i < pDriverList.size(); i++)
    {
        yarp::dev::PolyDriver* pd = pDriverList[i]->poly;
        if (pd->isValid())
        {
            IFrameTransformStorageGet* pp=nullptr;
            if (pd->view(pp) && pp!=nullptr)
            {
                iGetIf.push_back(pp);
            }
        }
    }
    return true;
}

bool FrameTransformStorage::clear()
{
    return m_tf_container.clear();
}

bool FrameTransformStorage::size(size_t& size) const
{
    return m_tf_container.size(size);
}
