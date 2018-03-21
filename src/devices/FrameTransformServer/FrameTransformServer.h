/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#ifndef YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H
#define YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/RpcServer.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <yarp/dev/IFrameTransform.h>
#include <string>

#include <yarp/math/FrameTransform.h>

#include <yarp/rosmsg/geometry_msgs/TransformStamped.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>



/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp
{
    namespace dev
    {
        class FrameTransformServer;
    }
}

#define ROSNODENAME "/tfNode"
#define ROSTOPICNAME_TF "/tf"
#define ROSTOPICNAME_TF_STATIC "/tf_static"
#define DEFAULT_THREAD_PERIOD 20 //ms

class Transforms_server_storage
{
private:
    std::vector <yarp::math::FrameTransform> m_transforms;
    yarp::os::Mutex  m_mutex;

public:
     Transforms_server_storage()      {}
     ~Transforms_server_storage()     {}
     bool     set_transform           (yarp::math::FrameTransform t);
     bool     delete_transform        (int id);
     bool     delete_transform        (std::string t1, std::string t2);
     inline size_t   size()                                             { return m_transforms.size(); }
     inline yarp::math::FrameTransform& operator[]   (std::size_t idx)  { return m_transforms[idx]; }
     void clear                       ();
};

class yarp::dev::FrameTransformServer : public yarp::os::RateThread,
                                   public yarp::dev::DeviceDriver,
                                   public yarp::os::PortReader
{
public:
    FrameTransformServer();
    ~FrameTransformServer();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;
    yarp::os::Bottle getOptions();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::Mutex              m_mutex;
    yarp::os::ConstString        m_streamingPortName;
    yarp::os::ConstString        m_rpcPortName;
    yarp::os::Stamp              m_lastStateStamp;
    int                          m_period;
    yarp::os::Node*              m_rosNode;
    bool                         m_enable_publish_ros_tf;
    bool                         m_enable_subscribe_ros_tf;
    Transforms_server_storage*   m_ros_timed_transform_storage;
    Transforms_server_storage*   m_ros_static_transform_storage;
    Transforms_server_storage*   m_yarp_timed_transform_storage;
    Transforms_server_storage*   m_yarp_static_transform_storage;
    double                       m_FrameTransformTimeout;

    yarp::os::RpcServer                      m_rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> m_streamingPort;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage> m_rosPublisherPort_tf_timed;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage> m_rosPublisherPort_tf_static;
    yarp::os::Subscriber<yarp::rosmsg::tf2_msgs::TFMessage> m_rosSubscriberPort_tf_timed;
    yarp::os::Subscriber<yarp::rosmsg::tf2_msgs::TFMessage> m_rosSubscriberPort_tf_static;

    virtual bool read(yarp::os::ConnectionReader& connection) override;
    inline  void list_response(yarp::os::Bottle& out);
    bool         parseStartingTf(yarp::os::Searchable &config);

#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif // YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H
