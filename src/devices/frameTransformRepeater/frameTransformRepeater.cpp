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
FrameTransformRepeater::FrameTransformRepeater(double tperiod) :
PeriodicThread (tperiod),
m_period(tperiod)
{}

bool FrameTransformRepeater::open(yarp::os::Searchable& config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORREPEATER,"Error! YARP Network is not initialized");
        return false;
    }

    bool okGeneral = config.check("GENERAL");
    if(okGeneral)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        if (general_config.check("period"))
        {
            m_period = general_config.find("period").asFloat64();
            setPeriod(m_period);
        }
    }

    start();
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

void FrameTransformRepeater::run()
{
    return;
}
