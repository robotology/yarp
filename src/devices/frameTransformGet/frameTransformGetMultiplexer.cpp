/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "frameTransformGetMultiplexer.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(FRAMETRANSFORMGETMULTIPLEXER, "yarp.devices.FrameTransformGetMultiplexer")


/** Device driver interface */
bool FrameTransformGetMultiplexer::open(yarp::os::Searchable &config)
{
    if (!yarp::os::NetworkBase::checkNetwork()) {
        yCError(FRAMETRANSFORMGETMULTIPLEXER,"Error! YARP Network is not initialized");
        return false;
    }
    return true;
}



bool FrameTransformGetMultiplexer::detachAll()
{
    m_iFrameTransformStorageGetList.clear();
    return true;
}


bool FrameTransformGetMultiplexer::close()
{
    yCTrace(FRAMETRANSFORMGETMULTIPLEXER, "Close");
    detachAll();
    // Closing port
    return true;
}


bool FrameTransformGetMultiplexer::attachAll(const yarp::dev::PolyDriverList& devices2attach)
{
    switch (devices2attach.size())
    {
        case 2:
            // yarp tf repeater and nwc ros or ros2
        case 3:
            // yarp tf repeater, nwc ros and nwc ros2
            for (int i = 0; i < devices2attach.size(); i++)
            {
                yarp::dev::PolyDriver* polyDriverLocal = devices2attach[i]->poly;
                if (polyDriverLocal->isValid())
                {
                    yarp::dev::IFrameTransformStorageGet* iFrameTransformStorageGet=nullptr;
                    if (polyDriverLocal->view(iFrameTransformStorageGet) && iFrameTransformStorageGet!=nullptr)
                    {
                        m_iFrameTransformStorageGetList.push_back(iFrameTransformStorageGet);
                    }
                }
            }
            return true;
        default:
            yCError(FRAMETRANSFORMGETMULTIPLEXER) << "need to attach 2 or 3 devices, " << devices2attach.size() << "found";
            break;
    }
    return false;
}


bool FrameTransformGetMultiplexer::getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const
{
    for (size_t i = 0; i < m_iFrameTransformStorageGetList.size(); i++)
    {
        if (m_iFrameTransformStorageGetList[i] != nullptr) {
            std::vector<yarp::math::FrameTransform> localTransform;
            m_iFrameTransformStorageGetList[i]->getTransforms(localTransform);
            transforms.insert(transforms.end(),
                              localTransform.begin(),
                              localTransform.end());
        }
        else {
            yCError(FRAMETRANSFORMGETMULTIPLEXER) << "pointer to interface IFrameTransformStorageGet not valid";
            return false;
        }
    }
    return true;
}
