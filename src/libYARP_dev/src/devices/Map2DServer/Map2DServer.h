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
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/api.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <string>
#include "include/visualization_msgs_MarkerArray.h"
#include "include/geometry_msgs_TransformStamped.h"
#include "include/tf_tfMessage.h"

namespace yarp
{
    namespace dev
    {
        class Map2DServer;
    }
}

#define DEFAULT_THREAD_PERIOD 20 //ms

/**
 *  @ingroup dev_impl_wrapper
 *
 * \section Map2DServer
 * A device capable of read/save collections of maps from disk, and make them accessible to any Map2DClient device.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value    | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:----------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              | /mapServer/rpc   | No           | Full name of the rpc port openend by the Map2DServer device .     |       |
 * | mapCollection  |      -         | string  | -              |   -              | No           | The name of .ini file containgin a map collection.                |       |

 * \section Notes:
 * Integration with ROS map server is currently under development.
 */

class yarp::dev::Map2DServer : public yarp::dev::DeviceDriver, public yarp::os::PortReader
{
private:
    std::map<std::string, yarp::dev::MapGrid2D>     m_maps_storage;
    std::map<std::string, yarp::dev::Map2DLocation> m_locations;

public:
    Map2DServer();
    ~Map2DServer();
    
    bool saveMaps(std::string filename);
    bool loadMaps(std::string filename);
    bool load_locations(std::string locations_file);
    bool save_locations(std::string locations_file);
    bool open(yarp::os::Searchable &params) override;
    bool close() override;
    yarp::os::Bottle getOptions();

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::ResourceFinder     m_rf_mapCollection;
    yarp::os::Mutex              m_mutex;
    yarp::os::ConstString        m_rpcPortName;
    yarp::os::Node*              m_rosNode;
    bool                         m_enable_publish_ros_tf;
    bool                         m_enable_subscribe_ros_tf;

    yarp::os::RpcServer                      m_rpcPort;
    yarp::os::Publisher<tf_tfMessage>        m_rosPublisherPort_tf_timed;
    yarp::os::Subscriber<tf_tfMessage>       m_rosSubscriberPort_tf_timed;

    yarp::os::Publisher<visualization_msgs_MarkerArray>        m_rosPublisherPort_markers;

    virtual bool read(yarp::os::ConnectionReader& connection) override;
    inline  void list_response(yarp::os::Bottle& out);

    void parse_string_command(yarp::os::Bottle& in, yarp::os::Bottle& out);
    void parse_vocab_command(yarp::os::Bottle& in, yarp::os::Bottle& out);
    bool updateVizMarkers();

#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif // YARP_DEV_MAP2DSERVER_H
