/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "FrameBroadcaster.h"
#include <map>
#include <vector>
#include <yarp/os/LogStream.h>
#include <yarp/rosmsg/geometry_msgs/TransformStamped.h>

#define DEFAULT_THREAD_PERIOD   0.010 //s

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::rosmsg::tf2_msgs;

inline void BottleaddFrame(TFMessage& b, const yarp::math::FrameTransform& frame)
{

    auto& submsg = yarp::rosmsg::  geometry_msgs::TransformStamped();
    submsg.child_frame_id          = frame.frameId;
    submsg.header.frame_id         = frame.parentFrame;
    submsg.transform.rotation.w    = frame.rotation.w();
    submsg.transform.rotation.x    = frame.rotation.x();
    submsg.transform.rotation.y    = frame.rotation.y();
    submsg.transform.rotation.z    = frame.rotation.z();
    submsg.transform.translation.x = frame.translation.tX;
    submsg.transform.translation.y = frame.translation.tY;
    submsg.transform.translation.z = frame.translation.tZ;
    submsg.header.stamp            = frame.timestamp;
    b.transforms.push_back(submsg);
}

inline void sendFrameContainer(yarp::os::Publisher<TFMessage>& port, const std::vector<yarp::math::FrameTransform>& frames)
//inline void sendFrameContainer(yarp::os::BufferedPort<TFMessage>& port, const std::vector<yarp::math::FrameTransform>& frames)
{
    auto& msg = port.prepare();
    msg.clear();
    for (auto& frame : frames)
    {
        BottleaddFrame(msg, frame);
    }
    port.write();
}

FrameBroadcaster::FrameBroadcaster() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}

bool FrameBroadcaster::open(yarp::os::Searchable& params)
{
    sendFrames = [this](IFrameSource * fs)
    {
        if (!fs)
            return false;

        if (!port.getOutputCount())
            return false;
        portMutex.lock();
        if (shouldClose)
        {
            port.close();
            fs->unsetCallback(callbackId);
            portMutex.unlock();
            return false;
        }

        sendFrameContainer(port, fs->getAllFrames());
        portMutex.unlock();
        return true;
    };

    static int id = 0;
    n = std::make_unique<yarp::os::Node>("/frameBroadcaster" + std::to_string(id));
    id++;

    if(params.check("help"))
    {
        yInfo() << "parameters:\n\n" <<
                   "period             - refresh period of the broadcasted values in ms.. default" << DEFAULT_THREAD_PERIOD * 1000<< "\n"
                   "topic              - topic to publish data to, e.g. /oculus\n" <<
                   "eventBased         - don't start a thread and sends frame from the caller thread" <<
                   "subdevice          - name of the subdevice to open\n";
        return false;
    }
    std::string rootName;
    if (!params.check("period", "refresh period of the broadcasted values in ms"))
    {
        yInfo() << "FrameReceiver: using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "s";
    }
    else
    {
        m_period = params.find("period").asInt32() / 1000.0;
    }

    rootName = params.check("topic",Value("/"), "starting '/' if needed.").asString();

    if (!params.check("topic", "Prefix name of the ports opened by the FrameBroadcaster."))
    {
        yError() << "FrameBroadcaster: missing 'name' parameter. Check you configuration file; it must be like:";
        yError() << "   topic:         topic to publish data to, e.g. /oculus";
        return false;
    }

    eventBased = params.check("eventBased");

    rootName = params.find("topic").asString();

    if (!port.topic(rootName))
    //if(!port.open(rootName))
    {
        yError("FrameBroadcaster: error while opening topic\n");
        return false;
    }
    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(params.check("subdevice"))
    {
        m_isSubdeviceOwned=true;
        if(!openAndAttachSubDevice(params))
        {
            yError("FrameBroadcaster: error while opening subdevice\n");
            return false;
        }
    }
    else
    {
        m_isSubdeviceOwned = false;
    }
    
    if (!m_device)
        return true;

    auto ret = m_device->setCallback(sendFrames);
    if (!ret.valid)
    {
        PeriodicThread::setPeriod(m_period);
        PeriodicThread::start();
        eventBased = false;
    }
    else
    {
        callbackId = ret.value;
    }
    
    return true;
}

bool FrameBroadcaster::openAndAttachSubDevice(Searchable& prop)
{
    Property p;

    m_subDeviceOwned = std::make_unique<PolyDriver>();

    p.fromString(prop.toString());
    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if errors occurred during open, quit here.
    m_subDeviceOwned->open(p);

    if (!m_subDeviceOwned->isValid())
    {
        yError("FrameBroadcaster: opening subdevice... FAILED\n");
        return false;
    }
    m_isSubdeviceOwned = true;
    if(!attach(m_subDeviceOwned.get()))
        return false;

    return true;
}

bool FrameBroadcaster::attach(PolyDriver* poly)
{
    if(poly)
        poly->view(m_device);

    if(m_device == nullptr)
    {
        yError() << "JoypadControlServer: attached device has no valid IJoypadController interface.";
        return false;
    }
    return true;
}

bool FrameBroadcaster::attach(yarp::dev::IFrameSource* s)
{
    if(s == nullptr)
    {
        yError() << "FrameBroadcaster: attached device has no valid IFrameSource interface.";
        return false;
    }
    m_device = s;
    return true;
}

bool FrameBroadcaster::detach()
{
    m_device = nullptr;
    return true;
}

bool FrameBroadcaster::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void FrameBroadcaster::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}

void FrameBroadcaster::run()
{
    if(!callbackId)
        sendFrames(m_device);
    portMutex.lock();
    sendFrameContainer(port, internalFrames);
    portMutex.unlock();
}

bool FrameBroadcaster::attachAll(const PolyDriverList& p)
{
    if (p.size() != 1)
    {
        yError("FrameBroadcaster: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver* Idevice2attach = p[0]->poly;
    if(p[0]->key == "IFrameSource")
    {
        yInfo() << "FrameBroadcaster: Good name!";
    }
    else
    {
        yInfo() << "FrameBroadcaster: Bad name!";
    }

    if (!Idevice2attach->isValid())
    {
        yError() << "FrameBroadcaster: Device " << p[0]->key << " to attach to is not valid ... cannot proceed";
        return false;
    }

    Idevice2attach->view(m_device);
    if(!attach(m_device))
        return false;

    return true;
}

bool FrameBroadcaster::detachAll()
{
    if (yarp::os::PeriodicThread::isRunning())
        yarp::os::PeriodicThread::stop();

    //check if we already instantiated a subdevice previously
    if (m_isSubdeviceOwned)
        return false;

    m_device = nullptr;
    return true;
}

bool FrameBroadcaster::close()
{
    detachAll();
    shouldClose = true;
    while (port.isWriting())
    {
        yarp::os::Time::delay(0.05);
    }
    // close subdevice if it was created inside the open (--subdevice option)
    if(m_isSubdeviceOwned)
    {
        if(m_subDeviceOwned)m_subDeviceOwned->close();

        m_device           = nullptr;
        m_isSubdeviceOwned = false;
    }
    return true;
}

bool FrameBroadcaster::clear()
{
    auto ret = internalFrames.size();
    internalFrames.clear();
    return ret;
}

bool FrameBroadcaster::setTransform (const yarp::math::FrameTransform& frame)
{
    if (eventBased && !this->isRunning())
    {
        start();
    }

    for (auto& f : internalFrames)
    {
        if (f.frameId == frame.frameId)
        {
            if (f.parentFrame == frame.parentFrame)
            {
                portMutex.lock();
                f = frame;

                if (!PeriodicThread::isRunning())
                    sendFrameContainer(port, { frame });

                portMutex.unlock();
                return true;
            }
            else
                return false;
        }
    }

    portMutex.lock();
    internalFrames.emplace_back(frame);
    if (!PeriodicThread::isRunning())
        sendFrameContainer(port, {frame});
    portMutex.unlock();
    return true;
}

void FrameBroadcaster::setTransforms(const std::vector<yarp::math::FrameTransform>& frames, bool append)
{
    if (eventBased && !this->isRunning())
    {
        start();
    }

    portMutex.lock();
    if (append)
        internalFrames.insert(std::end(internalFrames), std::begin(frames), std::end(frames));
    else
        internalFrames = frames;
    if (!PeriodicThread::isRunning())
        sendFrameContainer(port, frames);
    portMutex.unlock();
}

bool FrameBroadcaster::deleteTransform (const std::string &frame_id)
{
    portMutex.lock();
    for (auto i = internalFrames.begin(); i < internalFrames.end(); i++)
        if (i->frameId == frame_id)
        {
            internalFrames.erase(i);
            portMutex.unlock();
            return true;
        }

    portMutex.unlock();
    return false;
}
