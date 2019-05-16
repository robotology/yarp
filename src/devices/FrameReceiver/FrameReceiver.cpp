/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "FrameReceiver.h"
#include <map>
#include <vector>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::rosmsg::tf2_msgs;
void PortCallback::onRead(TFMessage& msg)
{
    int i = 0;
    Bottle* submsg{ nullptr };

    FrameTransform ft;
    owner->cacheValid = false;
    fvecguard.lock();
    for(const auto& submsg : msg.transforms)
    {
        ft.frameId        = submsg.child_frame_id;
        ft.parentFrame    = submsg.header.frame_id;
        ft.translation.tX = submsg.transform.translation.x;
        ft.translation.tY = submsg.transform.translation.y;
        ft.translation.tZ = submsg.transform.translation.z;
        ft.rotation.w()   = submsg.transform.rotation.w;
        ft.rotation.x()   = submsg.transform.rotation.x;
        ft.rotation.y()   = submsg.transform.rotation.y;
        ft.rotation.z()   = submsg.transform.rotation.z;
        ft.timestamp      = submsg.header.stamp.sec + submsg.header.stamp.nsec/1'000'000'000;
        fvec[ft.frameId] = ft;
        i++;
    }
    fvecguard.unlock();

    owner->callAllCallbacks();
}

bool FrameReceiver::callbackPrepare()
{
    return true;
}

bool FrameReceiver::callbackDismiss()
{
    return true;
}

void FrameReceiver::updateFrameContainer(FrameEditor& fs)
{
    portCallback.fvecguard.lock();
    for (auto& f : portCallback.fvec)
    {
        fs.insertUpdate(f.second);
    }
    cacheValid = true;
    portCallback.fvecguard.unlock();
}

bool FrameReceiver::open(yarp::os::Searchable& params)
{
    static int id = 0;
    n = std::make_unique<yarp::os::Node>("/frameReceiver" + std::to_string(id));
    id++;
    if(params.check("help"))
    {
        yInfo() << "parameters:\n\n" <<
            "topic               - Topic name\n";
        return false;
    }

    m_name = params.check("topic",Value("/"), "starting '/' if needed.").asString();

    if (!params.check("topic", "name of the topic to subscribe to."))
    {
        yError() << "FrameReceiver: missing 'topic' parameter. Check you configuration file; it must be like:";
        yError() << "   topic:         name of the topic to subscribe to, e.g. /tfOVR";
        return false;
    }

    m_name = params.find("topic").asString();

    if (!port.topic(m_name))
    //if (!port.open(m_name + "receiver"))
    {
        yError("FrameReceiver: error while subscribing\n");
        return false;
    }

    //auto b = yarp::os::Network::connect("/testframetransform", m_name + "receiver");
    portCallback.owner = this;
    port.useCallback(portCallback);
    cacheValid = true;
    return true;
}

bool FrameReceiver::close()
{
    port.close();
    return true;
}
