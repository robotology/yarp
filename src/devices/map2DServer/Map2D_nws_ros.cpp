/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Map2D_nws_ros.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>

#include <yarp/dev/GenericVocabs.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>

#include <yarp/math/Math.h>

#include <yarp/rosmsg/TickDuration.h>
#include <yarp/rosmsg/TickTime.h>

#include <cstdlib>
#include <fstream>
#include <limits>
#include <mutex>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace std;

namespace {
YARP_LOG_COMPONENT(MAP2D_NWS_ROS, "yarp.device.map2D_nws_ros")
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG 180/M_PI
#define DEG2RAD M_PI/180

/**
  * Map2D_nws_ros
  */

Map2D_nws_ros::Map2D_nws_ros()
{
    m_enable_publish_map = false;
    m_enable_subscribe_map = false;
    m_node = nullptr;
}

Map2D_nws_ros::~Map2D_nws_ros() = default;

bool Map2D_nws_ros::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }

    reply.clear();

    if (command.get(0).isString() && command.get(0).asString() == "help")
    {
        reply.addVocab32("many");
        reply.addString("No commands currently available:");
    }
    else
    {
        yCError(MAP2D_NWS_ROS) << "Invalid command. Try `help`";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter* returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
}

bool Map2D_nws_ros::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (!config.check("name"))
    {
        m_rpcPortName = "/map2D_nws_ros/rpc";
    }
    else
    {
        m_rpcPortName = config.find("name").asString();
    }

    //subdevice handling
    if (config.check("subdevice"))
    {
        Property       p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!m_drv.open(p) || !m_drv.isValid())
        {
            yCError(MAP2D_NWS_ROS) << "Failed to open subdevice.. check params";
            return false;
        }

        if (!attach(&m_drv))
        {
            yCError(MAP2D_NWS_ROS) << "Failed to open subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(MAP2D_NWS_ROS) << "Waiting for device to attach";
    }

    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yCError(MAP2D_NWS_ROS, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    //ROS configuration
    if (config.check("ROS"))
    {
        yCInfo(MAP2D_NWS_ROS, "Configuring ROS params");
        Bottle ROS_config = config.findGroup("ROS");

        if (ROS_config.find("enable_publisher").asInt32() == 1 || ROS_config.find("enable_publisher").asString() == "true")
        {
            m_enable_publish_map = true;
            yCInfo(MAP2D_NWS_ROS) << "Enabled ROS publisher";
        }

        if (ROS_config.find("enable_subscriber").asInt32() == 1 || ROS_config.find("enable_subscriber").asString() == "true")
        {
            m_enable_subscribe_map = true;
            yCInfo(MAP2D_NWS_ROS) << "Enabled ROS subscriber";
        }

        if (m_enable_publish_map)
        {
            if (m_node == nullptr)
            {
                m_node = new yarp::os::Node(ROSNODENAME);
            }
            if (!m_publisherPort_map.topic(ROSTOPICNAME_MAP))
            {
                yCError(MAP2D_NWS_ROS) << "Unable to publish to" << ROSTOPICNAME_MAP << "topic, check your YARP-ROS network configuration";
                return false;
            }
            if (!m_publisherPort_metamap.topic(ROSTOPICNAME_MAPMETADATA))
            {
                yCError(MAP2D_NWS_ROS) << "Unable to publish to " << ROSTOPICNAME_MAPMETADATA << " topic, check your YARP-ROS network configuration";
                return false;
            }
            //should I publish the map now? with which name ?
            //publishMapToRos();
        }

        if (m_enable_subscribe_map)
        {
            if (!m_subscriberPort_map.topic(ROSTOPICNAME_MAP))
            {
                yCError(MAP2D_NWS_ROS) << "Unable to subscribe to " << ROSTOPICNAME_MAP << " topic, check your YARP-ROS network configuration";
                return false;
            }
            if (!m_subscriberPort_metamap.topic(ROSTOPICNAME_MAPMETADATA))
            {
                yCError(MAP2D_NWS_ROS) << "Unable to subscribe to " << ROSTOPICNAME_MAPMETADATA << " topic, check your YARP-ROS network configuration";
                return false;
            }
            m_subscriberPort_map.setStrict();
            m_subscriberPort_metamap.setStrict();

            //should I subscribe the map now ? with which name ?
            //subscribeMapFromRos();
        }
    }
    else
    {
        //no ROS options
        yCWarning(MAP2D_NWS_ROS) << "ROS Group not configured";
    }

    return true;
}

bool Map2D_nws_ros::publishMapToRos(string map_name)
{
    MapGrid2D current_map;
    if (!m_iMap2D->get_map(map_name, current_map))
    {
        yCError(MAP2D_NWS_ROS) << "publishMapToRos() " << map_name << " does not exists";
        return false;
    }

    double tmp = 0;
    yarp::rosmsg::nav_msgs::OccupancyGrid& ogrid = m_publisherPort_map.prepare();
    ogrid.clear();
    ogrid.info.height = current_map.height();
    ogrid.info.width = current_map.width();
    current_map.getResolution(tmp);
    ogrid.info.resolution = tmp;
    ogrid.header.frame_id = "map";
    ogrid.info.map_load_time.sec = 0;
    ogrid.info.map_load_time.nsec = 0;
    double x, y, t;
    current_map.getOrigin(x, y, t);
    ogrid.info.origin.position.x = x;
    ogrid.info.origin.position.y = y;
    yarp::math::Quaternion q;
    yarp::sig::Vector v(4);
    v[0] = 0; v[1] = 0; v[2] = 1; v[3] = t * DEG2RAD;
    q.fromAxisAngle(v);
    ogrid.info.origin.orientation.x = q.x();
    ogrid.info.origin.orientation.y = q.y();
    ogrid.info.origin.orientation.z = q.z();
    ogrid.info.origin.orientation.w = q.w();
    ogrid.data.resize(current_map.width() * current_map.height());
    int index = 0;
    yarp::dev::Nav2D::XYCell cell;
    for (cell.y = current_map.height(); cell.y-- > 0;) {
        for (cell.x = 0; cell.x < current_map.width(); cell.x++)
        {
            current_map.getOccupancyData(cell, tmp);
            ogrid.data[index++] = (int)tmp;
        }
    }

    m_publisherPort_map.write();

    //what about the m_publisherPort_metamap ?
    //I don't know...

    return true;
}

bool Map2D_nws_ros::subscribeMapFromRos(string map_name)
{
    //In this block receives data from a ROS topic and stores data on attached device
    //yarp::os::Time::delay(5);
    yarp::rosmsg::nav_msgs::OccupancyGrid* map_ros = nullptr;
    yarp::rosmsg::nav_msgs::MapMetaData* metamap_ros = nullptr;

    map_ros = m_subscriberPort_map.read(true);
    metamap_ros = m_subscriberPort_metamap.read(true);
    if (map_ros != nullptr && metamap_ros != nullptr)
    {
        yCInfo(MAP2D_NWS_ROS) << "Received map for ROS";
        string map_name = "ros_map";
        MapGrid2D map;
        map.setSize_in_cells(map_ros->info.width,map_ros->info.height);
        map.setResolution(map_ros->info.resolution);
        map.setMapName(map_name);
        yarp::math::Quaternion quat(map_ros->info.origin.orientation.x,
                                    map_ros->info.origin.orientation.y,
                                    map_ros->info.origin.orientation.z,
                                    map_ros->info.origin.orientation.w);
        yarp::sig::Matrix mat = quat.toRotationMatrix4x4();
        yarp::sig::Vector vec = yarp::math::dcm2rpy(mat);
        double orig_angle = vec[2];
        map.setOrigin(map_ros->info.origin.position.x,map_ros->info.origin.position.y,orig_angle);
        for (size_t y = 0; y < map_ros->info.height; y++)
        {
            for (size_t x = 0; x < map_ros->info.width; x++)
            {
                XYCell cell(x,map_ros->info.height - 1 - y);
                double occ = map_ros->data[x + y * map_ros->info.width];
                map.setOccupancyData(cell,occ);

                if (occ >= 0 && occ <= 70) {
                    map.setMapFlag(cell, MapGrid2D::MAP_CELL_FREE);
                } else if (occ >= 71 && occ <= 100) {
                    map.setMapFlag(cell, MapGrid2D::MAP_CELL_WALL);
                } else {
                    map.setMapFlag(cell, MapGrid2D::MAP_CELL_UNKNOWN);
                }
            }
        }
        if (m_iMap2D->store_map(map))
        {
            yCInfo(MAP2D_NWS_ROS) << "Added map " << map.getMapName() << " to storage";
            return true;
        }

        yCInfo(MAP2D_NWS_ROS) << "Unable to add map " << map.getMapName() << " to storage";
        return false;
    }

    return false;
}

bool Map2D_nws_ros::close()
{
    yCTrace(MAP2D_NWS_ROS, "Close");
    if (m_enable_publish_map)
    {
        m_publisherPort_map.interrupt();
        m_publisherPort_metamap.interrupt();
        m_publisherPort_map.close();
        m_publisherPort_metamap.close();
    }
    if (m_enable_subscribe_map)
    {
        m_subscriberPort_map.interrupt();
        m_subscriberPort_metamap.interrupt();
        m_subscriberPort_map.close();
        m_subscriberPort_metamap.close();
    }
    return true;
}

bool Map2D_nws_ros::updateVizMarkers(std::string map_name)
{
    if (m_publisherPort_markers.asPort().isOpen()==false)
    {
        m_publisherPort_markers.topic("/locationServerMarkers");
    }
    yarp::rosmsg::TickDuration dur; dur.sec = 0xFFFFFFFF;
    double yarpTimeStamp = yarp::os::Time::now();
    uint64_t time;
    uint64_t nsec_part;
    uint64_t sec_part;
    yarp::rosmsg::TickTime ret;
    time = (uint64_t)(yarpTimeStamp * 1000000000UL);
    nsec_part = (time % 1000000000UL);
    sec_part = (time / 1000000000UL);
    if (sec_part > std::numeric_limits<unsigned int>::max())
    {
        yCWarning(MAP2D_NWS_ROS) << "Timestamp exceeded the 64 bit representation, resetting it to 0";
        sec_part = 0;
    }

    yarp::rosmsg::visualization_msgs::Marker marker;
    yarp::rosmsg::TickTime    tt;
    yarp::sig::Vector         rpy(3);
    yarp::math::Quaternion    q;

    yarp::rosmsg::visualization_msgs::MarkerArray& markers = m_publisherPort_markers.prepare();
    markers.markers.clear();

    std::vector<std::string> locations;
    int count = 1;
    m_iMap2D->getLocationsList(locations);
    for (auto it : locations)
    {
        yarp::dev::Nav2D::Map2DLocation loc;
        m_iMap2D->getLocation(it, loc);
        rpy[0] = 0; //x
        rpy[1] = 0; //y
        rpy[2] = loc.theta / 180 * 3.14159265359; //z
        yarp::sig::Matrix m = yarp::math::rpy2dcm(rpy);
        q.fromRotationMatrix(m);

        marker.header.frame_id    = "map";
        tt.sec                    = (yarp::os::NetUint32) sec_part;;
        tt.nsec                   = (yarp::os::NetUint32) nsec_part;;
        marker.header.stamp       = tt;
        marker.ns                 = "my_namespace";
        marker.id                 = count;
        marker.type               = yarp::rosmsg::visualization_msgs::Marker::ARROW;
        marker.action             = yarp::rosmsg::visualization_msgs::Marker::ADD;
        marker.pose.position.x    = loc.x;
        marker.pose.position.y    = loc.y;
        marker.pose.position.z    = 0;
        marker.pose.orientation.x = q.x();
        marker.pose.orientation.y = q.y();
        marker.pose.orientation.z = q.z();
        marker.pose.orientation.w = q.w();
        marker.scale.x            = 1;
        marker.scale.y            = 0.1;
        marker.scale.z            = 0.1;
        marker.color.a            = 1.0;
        marker.color.r            = 0.0;
        marker.color.g            = 1.0;
        marker.color.b            = 0.0;
        marker.lifetime           = dur;
        marker.text               = it;
        markers.markers.push_back(marker);
        count++;
    }

    m_publisherPort_markers.write();
    return true;
}


bool Map2D_nws_ros::detach()
{
    m_iMap2D = nullptr;
    return true;
}

bool Map2D_nws_ros::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(m_iMap2D);
    }

    if (nullptr == m_iMap2D)
    {
        yCError(MAP2D_NWS_ROS, "Subdevice passed to attach method is invalid");
        return false;
    }

    return true;
}
