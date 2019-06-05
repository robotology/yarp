/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_JOYPADCONTROLSERVER_FRAMERECEIVER_H
#define YARP_DEV_JOYPADCONTROLSERVER_FRAMERECEIVER_H
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameSource.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Node.h>
#include <map>
#include <mutex>
#include <memory>
namespace yarp
{
    namespace dev
    {
        class FrameReceiver;
        class PortCallback;
    }
}

class yarp::dev::PortCallback : public yarp::os::TypedReaderCallback<yarp::rosmsg::tf2_msgs::TFMessage>
{
public:
    FrameReceiver* owner{nullptr};
    std::map<std::string, yarp::math::FrameTransform> fvec;
    std::mutex fvecguard;
    void onRead(yarp::rosmsg::tf2_msgs::TFMessage& msg) override;
};

class yarp::dev::FrameReceiver : public yarp::dev::DeviceDriver,
                                 public yarp::dev::ImplementIFrameSource
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    friend class PortCallback; //<-- ugly.. the reason is that we miss a way to give to a buffered port a lambda as a callback.. sorry
    std::string  m_name;
    std::string  m_remote;
    PortCallback portCallback;
    yarp::os::Subscriber<yarp::rosmsg::tf2_msgs::TFMessage> port;
    //yarp::os::BufferedPort<yarp::rosmsg::tf2_msgs::TFMessage> port;

    std::unique_ptr<yarp::os::Node> n;

#endif //DOXYGEN_SHOULD_SKIP_THIS
protected:
    virtual void updateFrameContainer(FrameEditor& fs) override;
    virtual bool callbackPrepare() override;
    virtual bool callbackDismiss() override;
public:
    bool open(yarp::os::Searchable& params) override;
    bool close() override;
};

#endif //YARP_DEV_JOYPADCONTROLSERVER_FRAMERECEIVER_H


