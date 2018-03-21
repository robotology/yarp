/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
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
#include <yarp/rosmsg/visualization_msgs/MarkerArray.h>
#include <yarp/rosmsg/nav_msgs/MapMetaData.h>
#include <yarp/rosmsg/nav_msgs/OccupancyGrid.h>

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
 * | name           |      -         | string  | -              | /mapServer/rpc   | No           | Full name of the rpc port opened by the Map2DServer device.       |       |
 * | mapCollection  |      -         | string  | -              |   -              | No           | The name of .ini file containing a map collection.                |       |

 * \section Notes:
 * Integration with ROS map server is currently under development.
 */

class yarp::dev::Map2DServer : public yarp::dev::DeviceDriver, public yarp::os::PortReader
{
private:
    std::map<std::string, yarp::dev::MapGrid2D>     m_maps_storage;
    std::map<std::string, yarp::dev::Map2DLocation> m_locations_storage;

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
    bool                         m_enable_publish_ros_map;
    bool                         m_enable_subscribe_ros_map;

    #define ROSNODENAME "/map2DServerNode"
    #define ROSTOPICNAME_MAP "/map"
    #define ROSTOPICNAME_MAPMETADATA "/map_metadata"

    yarp::os::RpcServer                                     m_rpcPort;
    yarp::os::Publisher<yarp::rosmsg::nav_msgs::OccupancyGrid>             m_rosPublisherPort_map;
    yarp::os::Publisher<yarp::rosmsg::nav_msgs::MapMetaData>               m_rosPublisherPort_metamap;
    yarp::os::Subscriber<yarp::rosmsg::nav_msgs::OccupancyGrid>            m_rosSubscriberPort_map;
    yarp::os::Subscriber<yarp::rosmsg::nav_msgs::MapMetaData>              m_rosSubscriberPort_metamap;
    yarp::os::Publisher<yarp::rosmsg::visualization_msgs::MarkerArray>     m_rosPublisherPort_markers;

    virtual bool read(yarp::os::ConnectionReader& connection) override;
    inline  void list_response(yarp::os::Bottle& out);

    void parse_string_command(yarp::os::Bottle& in, yarp::os::Bottle& out);
    void parse_vocab_command(yarp::os::Bottle& in, yarp::os::Bottle& out);
    bool updateVizMarkers();

#endif //DOXYGEN_SHOULD_SKIP_THIS
};

#endif // YARP_DEV_MAP2DSERVER_H
