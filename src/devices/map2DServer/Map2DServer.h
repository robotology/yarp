/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MAP2DSERVER_H
#define YARP_DEV_MAP2DSERVER_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/RpcServer.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/dev/Map2DPath.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/api.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <yarp/rosmsg/visualization_msgs/MarkerArray.h>
#include <yarp/rosmsg/nav_msgs/MapMetaData.h>
#include <yarp/rosmsg/nav_msgs/OccupancyGrid.h>


#define DEFAULT_THREAD_PERIOD 20 //ms

/**
 *  @ingroup dev_impl_wrapper dev_impl_navigation dev_impl_deprecated
 *
 * \section Map2DServer
 *
 * \brief `map2DServer` *deprecated*: A device capable of read/save collections of maps from disk, and make them accessible to any Map2DClient device.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value    | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:----------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              | /mapServer/rpc   | No           | Full name of the rpc port opened by the Map2DServer device.       |       |
 * | mapCollection  |      -         | string  | -              |   -              | No           | The name of .ini file containing a map collection.                |       |

 * \section Notes:
 * Integration with ROS map server is currently under development.
 */

class Map2DServer :
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader
{
private:
    std::map<std::string, yarp::dev::Nav2D::MapGrid2D>     m_maps_storage;
    std::map<std::string, yarp::dev::Nav2D::Map2DLocation> m_locations_storage;
    std::map<std::string, yarp::dev::Nav2D::Map2DPath>     m_paths_storage;
    std::map<std::string, yarp::dev::Nav2D::Map2DArea>     m_areas_storage;

public:
    Map2DServer();
    ~Map2DServer();

    bool saveMaps(std::string filename);
    bool loadMaps(std::string filename);
    bool load_locations_and_areas(std::string locations_file);
    bool save_locations_and_areas(std::string locations_file);
    bool open(yarp::os::Searchable &params) override;
    bool close() override;
    yarp::os::Bottle getOptions();

private:
    bool priv_load_locations_and_areas_v1(std::ifstream& file);
    bool priv_load_locations_and_areas_v2(std::ifstream& file);

private:
    yarp::os::ResourceFinder     m_rf_mapCollection;
    std::mutex              m_mutex;
    std::string                  m_rpcPortName;
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

    bool read(yarp::os::ConnectionReader& connection) override;
    inline  void list_response(yarp::os::Bottle& out);

    void parse_string_command(yarp::os::Bottle& in, yarp::os::Bottle& out);
    void parse_vocab_command(yarp::os::Bottle& in, yarp::os::Bottle& out);
    bool updateVizMarkers();
};

#endif // YARP_DEV_MAP2DSERVER_H
