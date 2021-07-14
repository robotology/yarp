/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H
#define YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/RpcServer.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/api.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <yarp/dev/IFrameTransform.h>

#include <yarp/math/FrameTransform.h>

#include <yarp/rosmsg/geometry_msgs/TransformStamped.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>


#define ROSNODENAME "/tfNode"
#define ROSTOPICNAME_TF "/tf"
#define ROSTOPICNAME_TF_STATIC "/tf_static"
#define DEFAULT_THREAD_PERIOD 0.02 //s

class Transforms_server_storage
{
private:
    std::vector <yarp::math::FrameTransform> m_transforms;
    std::mutex  m_mutex;

public:
     Transforms_server_storage()      {}
     ~Transforms_server_storage()     {}
     bool     set_transform           (const yarp::math::FrameTransform& t);
     bool     delete_transform        (int id);
     bool     delete_transform        (std::string t1, std::string t2);
     inline size_t   size()                                             { return m_transforms.size(); }
     inline yarp::math::FrameTransform& operator[]   (std::size_t idx)  { return m_transforms[idx]; }
     void clear                       ();
};

/**
* @ingroup dev_impl_network_wrapper dev_impl_deprecated
 *
 * \brief `transformServer` *deprecated*: Documentation to be added
 */
class TransformServer :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader
{
public:
    TransformServer();
    ~TransformServer();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;
    yarp::os::Bottle getOptions();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    std::mutex              m_mutex;
    std::string        m_streamingPortName;
    std::string        m_rpcPortName;
    yarp::os::Stamp              m_lastStateStamp;
    double                       m_period;
    yarp::os::Node*              m_rosNode;
    bool                         m_enable_publish_ros_tf;
    bool                         m_enable_subscribe_ros_tf;
    Transforms_server_storage*   m_ros_timed_transform_storage;
    Transforms_server_storage*   m_ros_static_transform_storage;
    Transforms_server_storage*   m_yarp_timed_transform_storage;
    Transforms_server_storage*   m_yarp_static_transform_storage;
    double                       m_FrameTransformTimeout;

    enum class show_transforms_in_diagram_t
    {
        do_not_show=0,
        show_quaternion=1,
        show_matrix=2,
        show_rpy=3
    };
    show_transforms_in_diagram_t  m_show_transforms_in_diagram= show_transforms_in_diagram_t::do_not_show;

    yarp::os::RpcServer                      m_rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> m_streamingPort;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage> m_rosPublisherPort_tf_timed;
    yarp::os::Publisher<yarp::rosmsg::tf2_msgs::TFMessage> m_rosPublisherPort_tf_static;
    yarp::os::Subscriber<yarp::rosmsg::tf2_msgs::TFMessage> m_rosSubscriberPort_tf_timed;
    yarp::os::Subscriber<yarp::rosmsg::tf2_msgs::TFMessage> m_rosSubscriberPort_tf_static;

    bool read(yarp::os::ConnectionReader& connection) override;
    inline  void list_response(yarp::os::Bottle& out);
    bool         generate_view();
    std::string  get_matrix_as_text(Transforms_server_storage* storage, int i);
    bool         parseStartingTf(yarp::os::Searchable &config);
};

#endif // YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H
