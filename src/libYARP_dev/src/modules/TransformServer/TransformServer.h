/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef TRANSFORM_SERVER_H_
#define TRANSFORM_SERVER_H_

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
#include <yarp/dev/ITransform.h>
#include <string>

#include "include/geometry_msgs_TransformStamped.h"
#include "include/tf_tfMessage.h"
#include <yarp/math/Transform.h>

/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp
{
    namespace dev
    {
        class TransformServer;
    }
}

#define ROSNODENAME "/tfNode"
#define ROSTOPICNAME "/tf"
#define DEFAULT_THREAD_PERIOD 20 //ms

class Transforms_server_storage
{
private:
    std::vector <yarp::math::Transform_t> m_transforms;

public:
     Transforms_server_storage()      {}
     ~Transforms_server_storage()     {}
     bool     set_transform           (yarp::math::Transform_t t);
     bool     delete_transform        (std::string t1, std::string t2);
     inline size_t   size()                                             { return m_transforms.size(); }
     inline yarp::math::Transform_t& operator[]   (std::size_t idx)     { return m_transforms[idx]; }
     inline void clear()             {m_transforms.clear();}
};

class yarp::dev::TransformServer : public yarp::os::RateThread,
                                   public yarp::dev::DeviceDriver,
                                   public yarp::os::PortReader
{
public:
    TransformServer();
    ~TransformServer();

    bool open(yarp::os::Searchable &params);
    bool close();
    yarp::os::Bottle getOptions();

    bool threadInit();
    void threadRelease();
    void run();

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::ConstString        m_streamingPortName;
    yarp::os::ConstString        m_rpcPortName;
    yarp::os::Stamp              m_lastStateStamp;
    int                          m_period;
    yarp::os::Node*              m_rosNode;
    bool                         m_enable_publish_ros_tf;
    bool                         m_enable_subscribe_ros_tf;
    Transforms_server_storage*   m_ros_transform_storage;
    Transforms_server_storage*   m_yarp_transform_storage;

    yarp::os::RpcServer                      m_rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> m_streamingPort;
    yarp::os::Publisher<tf_tfMessage>        m_rosPublisherPort_tf;
    yarp::os::Subscriber<tf_tfMessage>       m_rosSubscriberPort_tf;

    virtual bool read(yarp::os::ConnectionReader& connection);

#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif
