/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FrameTransformContainer.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORSTORAGE, "yarp.device.frameTransformUtils")
}

//------------------------------------------------------------------------------------------------------------------------------

bool FrameTransformContainer::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    transforms = m_transforms;
    return true;
}

bool FrameTransformContainer::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    for (auto& it : transforms)
    {
        setTransform(it);
    }
    return true;
}

bool FrameTransformContainer::setTransform(const yarp::math::FrameTransform& t)
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
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

bool FrameTransformContainer::deleteTransform(string t1, string t2)
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    if (t1 == "*" && t2 == "*")
    {
        m_transforms.clear();
        return true;
    }
    else
    {
        if (t1 == "*")
        {
            for (size_t i = 0; i < m_transforms.size(); )
            {
                //source frame is jolly, thus delete all frames with destination == t2
                if (m_transforms[i].dst_frame_id == t2)
                {
                    m_transforms.erase(m_transforms.begin() + i);
                    i = 0; //the erase operation invalidates the iteration, loop restart is required
                }
                else
                {
                    i++;
                }
            }
            return true;
        }
        else
        {
            if (t2 == "*")
            {
                for (size_t i = 0; i < m_transforms.size(); )
                {
                    //destination frame is jolly, thus delete all frames with source == t1
                    if (m_transforms[i].src_frame_id == t1)
                    {
                        m_transforms.erase(m_transforms.begin() + i);
                        i = 0; //the erase operation invalidates the iteration, loop restart is required
                    }
                    else
                    {
                        i++;
                    }
                }
                return true;
            }
            else
            {
                for (size_t i = 0; i < m_transforms.size(); i++)
                {
                    if ((m_transforms[i].dst_frame_id == t1 && m_transforms[i].src_frame_id == t2) ||
                        (m_transforms[i].dst_frame_id == t2 && m_transforms[i].src_frame_id == t1))
                    {
                        m_transforms.erase(m_transforms.begin() + i);
                        return true;
                    }
                }
            }
        }
    }

    yCError(FRAMETRANSFORSTORAGE) << "Transformation deletion not successful";
    return false;
}

bool FrameTransformContainer::clear()
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    m_transforms.clear();
    return true;
}

bool FrameTransformContainer::checkAndRemoveExpired()
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    double curr_t = yarp::os::Time::now();
    for (auto it= m_transforms.begin(); it!= m_transforms.end(); it++)
    {
        if (it->timestamp- curr_t >m_timeout &&
            it->isStatic == false)
        {
            m_transforms.erase(it);
            it = m_transforms.begin();
        }
    }
    return true;
}

bool FrameTransformContainer::checkAndRemoveExpired() const
{
    return const_cast<FrameTransformContainer*>(this)->checkAndRemoveExpired();
}

bool FrameTransformContainer::size(size_t& size) const
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    size = m_transforms.size();
    return true;
}

/*
bool FrameTransformStorage::delete_transform(int id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (id >= 0 && (size_t)id < m_transforms.size())
    {
        m_transforms.erase(m_transforms.begin() + id);
        return true;
    }
    return false;
}


yarp::math::FrameTransform& operator[]   (std::size_t idx)
{
    return m_transforms[idx];
}
*/
