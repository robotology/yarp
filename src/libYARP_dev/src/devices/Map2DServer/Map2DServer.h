/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_MAP2DSERVER_H
#define YARP_DEV_MAP2DSERVER_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/RpcServer.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/MapGrid2D.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <string>

#include "include/geometry_msgs_TransformStamped.h"
#include "include/tf_tfMessage.h"
#include <yarp/math/FrameTransform.h>

/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp
{
    namespace dev
    {
        class Map2DServer;
    }
}

#define ROSNODENAME "/tfNode"
#define ROSTOPICNAME_TF "/tf"
#define ROSTOPICNAME_TF_STATIC "/tf_static"
#define DEFAULT_THREAD_PERIOD 20 //ms

class yarp::dev::Map2DServer : public yarp::dev::DeviceDriver, public yarp::os::PortReader
{
private:
    std::map<std::string, yarp::sig::MapGrid2D> m_maps_storage;

public:
    Map2DServer();
    ~Map2DServer();
    
    bool saveMaps(std::string filename);
    bool loadMaps(std::string filename);
    bool open(yarp::os::Searchable &params);
    bool close();
    yarp::os::Bottle getOptions();

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::Mutex              m_mutex;
    yarp::os::ConstString        m_rpcPortName;
    yarp::os::Node*              m_rosNode;
    bool                         m_enable_publish_ros_tf;
    bool                         m_enable_subscribe_ros_tf;

    yarp::os::RpcServer                      m_rpcPort;
    yarp::os::Publisher<tf_tfMessage>        m_rosPublisherPort_tf_timed;
    yarp::os::Subscriber<tf_tfMessage>       m_rosSubscriberPort_tf_timed;

    virtual bool read(yarp::os::ConnectionReader& connection);
    inline  void list_response(yarp::os::Bottle& out);

#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif // YARP_DEV_MAP2DSERVER_H
