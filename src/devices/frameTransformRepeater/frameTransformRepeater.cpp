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

#include "frameTransformRepeater.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORREPEATER, "yarp.device.frameTransformRepeater")
}

//------------------------------------------------------------------------------------------------------------------------------

bool FrameTransformRepeater::open(yarp::os::Searchable& config)
{
    return true;
}

bool FrameTransformRepeater::close()
{
    return true;
}

bool FrameTransformRepeater::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    transforms = m_transforms;
    return true;
}

#ifndef SIMPLE_PASS
bool FrameTransformRepeater::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    for (auto& it : transforms)
    {
        setTransform(it);
    }
    return true;
}

bool FrameTransformRepeater::setTransform(const yarp::math::FrameTransform& t)
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    for (auto& it : m_transforms)
    {
        //this linear search may require some optimization
        if (it.dst_frame_id == t.dst_frame_id && it.src_frame_id == t.src_frame_id)
        {
            //transform already exists, update it
            it = t;
            return true;
        }
    }

    //add a new transform
    m_transforms.push_back(t);
    return true;
}

#else
bool FrameTransformRepeater::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    m_transforms.clear();
    for(auto& t : transforms)
    {
        m_transforms.push_back(t);
    }
    return true;
}

bool FrameTransformRepeater::setTransform(const yarp::math::FrameTransform& t)
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    m_transforms.clear();
    m_transforms.push_back(t);
    return true;
}
#endif


void FrameTransformRepeater::run()
{
    return;
}
