/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_DEV_MAP2D_NWS_ROS_H
#define YARP_DEV_MAP2D_NWS_ROS_H

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
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/dev/Map2DPath.h>
#include <yarp/dev/WrapperSingle.h>
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

/**
 *  @ingroup dev_impl_wrapper dev_impl_navigation
 *
 * \section Map2D_nws_ros
 *
 * \brief `Map2D_nws_ros`: A device capable of read/save collections of maps from disk, and make them accessible to any Map2DClient device.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter           | Type    | Units          | Default Value    | Required     | Description                                                       | Notes |
 * |:--------------:|:----------------------:|:-------:|:--------------:|:----------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -                 | string  | -              | /map2D_nws_ros/rpc   | No       | Full name of the rpc port opened by the Map2DServer device.       |       |
 * | ROS            | enable_publisher       | bool    | -              | false            | No           | Publishes maps stored in a map2DStorage on a ROS topic            |       |
 * | ROS            | enable_subscriber      | bool    | -              | false            | No           | Receives maps from a ROS topic and stores them in a map2DStorage  |       |

 * \section Notes:
 * Integration with ROS map server is currently under development.
 */

class Map2D_nws_ros :
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader,
        public yarp::dev::WrapperSingle
{
public:
    Map2D_nws_ros();
    ~Map2D_nws_ros();
    bool open(yarp::os::Searchable &params) override;
    bool close() override;
    bool detach() override;
    bool attach(yarp::dev::PolyDriver* driver) override;

private:
    //drivers and interfaces
    yarp::dev::Nav2D::IMap2D*    m_iMap2D = nullptr;
    yarp::dev::PolyDriver        m_drv;

    std::mutex                   m_mutex;
    std::string                  m_rpcPortName;
    yarp::os::Node*              m_node = nullptr;
    bool                         m_enable_publish_map;
    bool                         m_enable_subscribe_map;

    #define ROSNODENAME "/map2DServerNode"
    #define ROSTOPICNAME_MAP "/map"
    #define ROSTOPICNAME_MAPMETADATA "/map_metadata"

    yarp::os::RpcServer                                                    m_rpcPort;
    yarp::os::Publisher<yarp::rosmsg::nav_msgs::OccupancyGrid>             m_publisherPort_map;
    yarp::os::Publisher<yarp::rosmsg::nav_msgs::MapMetaData>               m_publisherPort_metamap;
    yarp::os::Subscriber<yarp::rosmsg::nav_msgs::OccupancyGrid>            m_subscriberPort_map;
    yarp::os::Subscriber<yarp::rosmsg::nav_msgs::MapMetaData>              m_subscriberPort_metamap;
    yarp::os::Publisher<yarp::rosmsg::visualization_msgs::MarkerArray>     m_publisherPort_markers;

    bool read(yarp::os::ConnectionReader& connection) override;

    bool updateVizMarkers(std::string map_name = "ros_map");
    bool subscribeMapFromRos(std::string map_name = "ros_map");
    bool publishMapToRos(std::string map_name = "ros_map");
};

#endif // YARP_DEV_MAP2D_NWS_ROS_H
