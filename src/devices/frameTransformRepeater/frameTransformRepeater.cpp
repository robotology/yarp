/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
FrameTransformRepeater::FrameTransformRepeater()
{}

bool FrameTransformRepeater::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORREPEATER,"Error! YARP Network is not initialized");
        return false;
    }

    return true;
}

bool FrameTransformRepeater::close()
{
    return true;
}

bool FrameTransformRepeater::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    if(!m_ftContainer.getTransforms(transforms))
    {
        yCError(FRAMETRANSFORREPEATER,"Unable to retrieve transforms");
        return false;
    }
    return true;
}

bool FrameTransformRepeater::setTransforms(const std::vector<yarp::math::FrameTransform>& transforms)
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    if(!m_ftContainer.setTransforms(transforms))
    {
        yCError(FRAMETRANSFORREPEATER,"Unable to set transforms");
        return false;
    }
    return true;
}

bool FrameTransformRepeater::setTransform(const yarp::math::FrameTransform& t)
{
    std::lock_guard<std::mutex> lock(m_trf_mutex);
    if(!m_ftContainer.setTransform(t))
    {
        yCError(FRAMETRANSFORREPEATER,"Unable to set transform");
        return false;
    }
    return true;
}
