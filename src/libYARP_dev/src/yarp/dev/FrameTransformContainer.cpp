/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameTransformContainer.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <algorithm>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORMCONTAINER, "yarp.device.frameTransformContainer")

}

//------------------------------------------------------------------------------------------------------------------------------

void FrameTransformContainer::invalidateTransform(yarp::math::FrameTransform& trf)
{
    trf.timestamp = yarp::os::Time::now();
    trf.isStatic = false;
    trf.translation = { 0,0,0 };
    trf.rotation = { 0,0,0,0 };
    if (m_verbose_debug)
    {
        yCIDebug(FRAMETRANSFORMCONTAINER, m_name) << "At time" << std::to_string(trf.timestamp)
            << "deleted transform (marked invalid):" << trf.src_frame_id << "->" << trf.dst_frame_id << ", assigned timestamp" << std::to_string(trf.timestamp);
    }
}

bool FrameTransformContainer::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    std::copy_if (m_transforms.begin(), m_transforms.end(), std::back_inserter(transforms), [](const yarp::math::FrameTransform& trf){return trf.isValid();});
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

bool FrameTransformContainer::setTransform(const yarp::math::FrameTransform& new_tr)
{
    if (new_tr.isValid()==false) return true;

    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    for (auto& it : m_transforms)
    {
        //this linear search may require some optimization
        if (it.dst_frame_id == new_tr.dst_frame_id && it.src_frame_id == new_tr.src_frame_id)
        {
            //if transform already exists and
            //its timestamp is more recent than the currently stored transform
            //than update it
            if (it.isStatic == false)
            {
                if (new_tr.timestamp > it.timestamp)
                {
                    it = new_tr;
                    return true;
                }
                else
                {
                    //yCDebug(FRAMETRANSFORMCONTAINER) << "Received old transform" << it.dst_frame_id << it.src_frame_id << std::to_string(it.timestamp) << it.isStatic;
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }

    //add a new transform
    m_transforms.push_back(new_tr);
    return true;
}

bool FrameTransformContainer::deleteTransform(std::string t1, std::string t2)
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    if (t1 == "*" && t2 == "*")
    {
        for (size_t i = 0; i < m_transforms.size(); i++)
        {
            invalidateTransform(m_transforms[i]);
        }
        return true;
    }
    else
    {
        if (t1 == "*")
        {
            for (size_t i = 0; i < m_transforms.size(); i++)
            {
                //source frame is jolly, thus delete all frames with destination == t2
                if (m_transforms[i].dst_frame_id == t2)
                {
                    invalidateTransform(m_transforms[i]);
                }
            }
            return true;
        }
        else
        {
            if (t2 == "*")
            {
                for (size_t i = 0; i < m_transforms.size(); i++)
                {
                    //destination frame is jolly, thus delete all frames with source == t1
                    if (m_transforms[i].src_frame_id == t1)
                    {
                        invalidateTransform(m_transforms[i]);
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
                        invalidateTransform(m_transforms[i]);
                        return true;
                    }
                }
            }
        }
    }

    yCError(FRAMETRANSFORMCONTAINER) << "Transformation deletion not successful";
    return false;
}

bool FrameTransformContainer::clearAll()
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    for (size_t i = 0; i < m_transforms.size(); i++)
    {
        invalidateTransform(m_transforms[i]);
    }
    return true;
}

bool FrameTransformContainer::checkAndRemoveExpired()
{
    std::lock_guard<std::recursive_mutex> lock(m_trf_mutex);
    double curr_t = yarp::os::Time::now();
    check_vector:
    for (auto it= m_transforms.begin(); it!= m_transforms.end(); it++)
    {
        if (curr_t - it->timestamp > m_timeout &&
            it->isStatic == false)
        {
            if (m_verbose_debug)
            {
                if (it->isValid())
                {yCIDebug(FRAMETRANSFORMCONTAINER, m_name) << "At time" << std::to_string(curr_t)
                 <<"Transform expired:" << it->src_frame_id << "->" << it->dst_frame_id << "with timestamp" << std::to_string(it->timestamp);}
                else
                {yCIDebug(FRAMETRANSFORMCONTAINER, m_name) << "At time" << std::to_string(curr_t)
                 << "Invalid transform expired:" << it->src_frame_id << "->"<< it->dst_frame_id << "with timestamp" << std::to_string(it->timestamp);}
            }
            m_transforms.erase(it);
            goto check_vector;
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
