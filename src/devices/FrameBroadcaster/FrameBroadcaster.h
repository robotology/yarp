/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_JOYPADCONTROLSERVER_FRAMEBROADCASTER_H
#define YARP_DEV_JOYPADCONTROLSERVER_FRAMEBROADCASTER_H
#define DEFAULT_THREAD_PERIOD   0.010 //s
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameSource.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <yarp/dev/IFrameSet.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Node.h>
#include <mutex>
#include <map>

namespace yarp
{
    namespace dev
    {
        class FrameBroadcaster;
    }
}

class yarp::dev::FrameBroadcaster : public yarp::dev::IFrameSet,
                                    public yarp::dev::DeviceDriver,
                                    public yarp::dev::IWrapper,
                                    public yarp::dev::IMultipleWrapper,
                                    public yarp::os::PeriodicThread
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS

    double                                  m_period{ DEFAULT_THREAD_PERIOD };
    yarp::dev::IFrameSource*                m_device{nullptr};
    std::unique_ptr<yarp::dev::PolyDriver>  m_subDeviceOwned{nullptr};
    bool                                    m_isSubdeviceOwned{false};
    std::string                             m_name;
    bool                                    shouldClose{false};
    std::mutex                              portMutex;
    std::vector<yarp::math::FrameTransform> internalFrames;
    bool                                    eventBased{false};
    std::unique_ptr<yarp::os::Node>         n;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage> port;
    //yarp::os::BufferedPort<yarp::rosmsg::tf2_msgs::TFMessage> port;
    std::function<bool(yarp::dev::IFrameSource*)> sendFrames;
    int callbackId{0};

    bool openAndAttachSubDevice(yarp::os::Searchable& prop);

#endif //DOXYGEN_SHOULD_SKIP_THIS

public:
    FrameBroadcaster();

    bool open(yarp::os::Searchable& params) override;
    bool close() override;
    bool attachAll(const yarp::dev::PolyDriverList& p) override;
    bool detachAll() override;
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool attach(yarp::dev::IFrameSource* s);
    bool detach() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

    virtual bool clear () override;
    virtual bool setTransform (const yarp::math::FrameTransform& frame) override;
    virtual void setTransforms (const std::vector<yarp::math::FrameTransform>& frames, bool append = false) override;
    virtual bool deleteTransform (const std::string &frame_id) override;


};

#endif


