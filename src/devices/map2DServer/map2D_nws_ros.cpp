/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#include <sstream>
#include <limits>
#include "map2D_nws_ros.h"
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/GenericVocabs.h>
#include <yarp/math/Math.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cstdlib>
#include <fstream>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <yarp/rosmsg/TickDuration.h>
#include <yarp/rosmsg/TickTime.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace std;

namespace {
YARP_LOG_COMPONENT(MAP2D_NWS_ROS, "yarp.device.map2D_nws_ros")
}

/**
  * Map2D_nws_ros
  */

Map2D_nws_ros::Map2D_nws_ros()
{
    m_enable_publish_ros_map = false;
    m_enable_subscribe_ros_map = false;
    m_rosNode = nullptr;
}

Map2D_nws_ros::~Map2D_nws_ros() = default;

bool Map2D_nws_ros::read(yarp::os::ConnectionReader& connection)
{
    yCWarning(MAP2D_NWS_ROS) << "not yet implemented";

    std::lock_guard<std::mutex> lock(m_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    //parse string command
    if(in.get(0).isString())
    {
      //  parse_string_command(in, out);
    }
    // parse vocab command
    else if(in.get(0).isVocab())
    {
   //     parse_vocab_command(in, out);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        out.write(*returnToSender);
    }
    else
    {
        yCError(MAP2D_NWS_ROS) << "Invalid return to sender";
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
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!m_drv.open(p) || !m_drv.isValid())
        {
            yCError(MAP2D_NWS_ROS) << "Failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&m_drv, "1");
        if (!attachAll(driverlist))
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
        if (ROS_config.check("enable_ros_publisher") == false)
        {
            yCError(MAP2D_NWS_ROS) << "Missing 'enable_ros_publisher' in ROS group";
            return false;
        }
        if (ROS_config.find("enable_ros_publisher").asInt32() == 1 || ROS_config.find("enable_ros_publisher").asString() == "true")
        {
            m_enable_publish_ros_map = true;
            yCInfo(MAP2D_NWS_ROS) << "Enabled ROS publisher";
        }
        if (ROS_config.check("enable_ros_subscriber") == false)
        {
            yCError(MAP2D_NWS_ROS) << "Missing 'enable_ros_subscriber' in ROS group";
            return false;
        }
        if (ROS_config.find("enable_ros_subscriber").asInt32() == 1 || ROS_config.find("enable_ros_subscriber").asString() == "true")
        {
            m_enable_subscribe_ros_map = true;
            yCInfo(MAP2D_NWS_ROS) << "Enabled ROS subscriber";
        }

        if (m_enable_subscribe_ros_map || m_enable_publish_ros_map)
        {
            if (m_rosNode == nullptr)
            {
                m_rosNode = new yarp::os::Node(ROSNODENAME);
            }
            if (m_enable_publish_ros_map && !m_rosPublisherPort_map.topic(ROSTOPICNAME_MAP))
            {
                yCError(MAP2D_NWS_ROS) << "Unable to publish to" << ROSTOPICNAME_MAP << "topic, check your YARP-ROS network configuration";
                return false;
            }
            if (m_enable_publish_ros_map && !m_rosPublisherPort_metamap.topic(ROSTOPICNAME_MAPMETADATA))
            {
                yCError(MAP2D_NWS_ROS) << "Unable to publish to " << ROSTOPICNAME_MAPMETADATA << " topic, check your YARP-ROS network configuration";
                return false;
            }

            if (m_enable_subscribe_ros_map && !m_rosSubscriberPort_map.topic(ROSTOPICNAME_MAP))
            {
                yCError(MAP2D_NWS_ROS) << "Unable to subscribe to " << ROSTOPICNAME_MAP << " topic, check your YARP-ROS network configuration";
                return false;
            }
            if (m_enable_subscribe_ros_map && !m_rosSubscriberPort_metamap.topic(ROSTOPICNAME_MAPMETADATA))
            {
                yCError(MAP2D_NWS_ROS) << "Unable to subscribe to " << ROSTOPICNAME_MAPMETADATA << " topic, check your YARP-ROS network configuration";
                return false;
            }
            m_rosSubscriberPort_map.setStrict();
            m_rosSubscriberPort_metamap.setStrict();

        }
    }
    else
    {
        //no ROS options
        yCWarning(MAP2D_NWS_ROS) << "ROS Group not configured";
    }

    //In this block receives data from a ROS topic and stores data on attached device
    //yarp::os::Time::delay(5);
    yarp::rosmsg::nav_msgs::OccupancyGrid*   map_ros = nullptr;
    yarp::rosmsg::nav_msgs::MapMetaData*     metamap_ros = nullptr;

    map_ros = m_rosSubscriberPort_map.read(true);
    metamap_ros = m_rosSubscriberPort_metamap.read(true);
    if (map_ros!=nullptr && metamap_ros!=nullptr)
    {
        yCInfo(MAP2D_NWS_ROS) << "Received map for ROS";
        string map_name = "ros_map";
        MapGrid2D map;
        map.setSize_in_cells(map_ros->info.width,map_ros->info.height);
        map.setResolution( map_ros->info.resolution);
        map.setMapName(map_name);
        yarp::math::Quaternion quat(map_ros->info.origin.orientation.x,
                                    map_ros->info.origin.orientation.y,
                                    map_ros->info.origin.orientation.z,
                                    map_ros->info.origin.orientation.w);
        yarp::sig::Matrix mat=quat.toRotationMatrix4x4();
        yarp::sig::Vector vec=yarp::math::dcm2rpy(mat);
        double orig_angle = vec[2];
        map.setOrigin(map_ros->info.origin.position.x,map_ros->info.origin.position.y,orig_angle);
        for (size_t y=0; y< map_ros->info.height; y++)
        {
            for (size_t x=0; x< map_ros->info.width; x++)
            {
                XYCell cell(x,map_ros->info.height-1-y);
                double occ = map_ros->data[x+y*map_ros->info.width];
                map.setOccupancyData(cell,occ);

                if      (occ >= 0   && occ <= 70)  map.setMapFlag(cell, MapGrid2D::MAP_CELL_FREE);
                else if (occ >= 71 && occ <= 100)  map.setMapFlag(cell, MapGrid2D::MAP_CELL_WALL);
                else                               map.setMapFlag(cell, MapGrid2D::MAP_CELL_UNKNOWN);
            }
        }
        if(m_iMap2D->store_map(map))
        {
            yCInfo(MAP2D_NWS_ROS) << "Added map " << map.getMapName() << " to storage";
        }
        else
        {
            yCInfo(MAP2D_NWS_ROS) << "Unable to add map " << map.getMapName() << " to storage";
        }
    }

    return true;
}

bool Map2D_nws_ros::close()
{
    yCTrace(MAP2D_NWS_ROS, "Close");
    if (m_enable_publish_ros_map)
    {
        m_rosPublisherPort_map.interrupt();
        m_rosPublisherPort_metamap.interrupt();
        m_rosPublisherPort_map.close();
        m_rosPublisherPort_metamap.close();
    }
    if (m_enable_subscribe_ros_map)
    {
        m_rosSubscriberPort_map.interrupt();
        m_rosSubscriberPort_metamap.interrupt();
        m_rosSubscriberPort_map.close();
        m_rosSubscriberPort_metamap.close();
    }
    return true;
}

bool Map2D_nws_ros::updateVizMarkers()
{
    if (m_rosPublisherPort_markers.asPort().isOpen()==false)
    {
        m_rosPublisherPort_markers.topic("/locationServerMarkers");
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

    yarp::rosmsg::visualization_msgs::MarkerArray& markers = m_rosPublisherPort_markers.prepare();
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

    m_rosPublisherPort_markers.write();
    return true;
}


bool Map2D_nws_ros::detachAll()
{
    m_iMap2D = nullptr;
    return true;
}

bool Map2D_nws_ros::attachAll(const PolyDriverList& device2attach)
{
    if (device2attach.size() != 1)
    {
        yCError(MAP2D_NWS_ROS, "Cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver* Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(m_iMap2D);
    }

    if (nullptr == m_iMap2D)
    {
        yCError(MAP2D_NWS_ROS, "Subdevice passed to attach method is invalid");
        return false;
    }

    return true;
}
