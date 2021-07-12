/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Time.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/math/math.h>
#include <yarp/dev/MapGrid2D.h>

#include <yarp/rosmsg/nav_msgs/OccupancyGrid.h>

using yarp::os::Network;
using yarp::os::Node;
using yarp::os::Publisher;

namespace {
YARP_LOG_COMPONENT(MAP_PUBLISHER, "yarp.example.rosmap_publisher")
constexpr double loop_delay = 0.1;
}

std::string extractPathFromFile(std::string full_filename)
{
    size_t found;
    found = full_filename.find_last_of('/');
    if (found != std::string::npos) return full_filename.substr(0, found) + "/";
    found = full_filename.find_last_of('\\');
    if (found != std::string::npos) return full_filename.substr(0, found) + "\\";
    return full_filename;
}

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;

    /* creates a node called /map_publisher */
    Node node("/map_publisher");

    /* creates a publisher */
    yarp::os::Publisher<yarp::rosmsg::nav_msgs::OccupancyGrid> publisher;
    if (!publisher.topic("/map"))
    {
        yCError(MAP_PUBLISHER) << "Failed to create publisher to /map";
        return -1;
    }
    yarp::os::Time::delay(0.1);

    /* read a map from disk */
    yarp::dev::Nav2D::MapGrid2D yarpmap;
    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("mapGrid2DTest");
    std::string si = rf.findFileByName("map_yarpAndRos.map");
    std::string so = extractPathFromFile(si);
    bool b = yarpmap.loadFromFile(si);
    if (!b)
    {
        yCError(MAP_PUBLISHER) << "Failed to load the map file";
        return -1;
    }

    /* publish the occupancy grid over the network */
    do
    {
        double tmp = 0;
        yarp::rosmsg::nav_msgs::OccupancyGrid& ogrid = publisher.prepare();
        ogrid.clear();
        ogrid.info.height = yarpmap.height();
        ogrid.info.width = yarpmap.width();
        yarpmap.getResolution(tmp);
        ogrid.info.resolution = tmp;
        ogrid.header.frame_id = "map";
        ogrid.info.map_load_time.sec = 0;
        ogrid.info.map_load_time.nsec = 0;
        double x, y, t;
        yarpmap.getOrigin(x, y, t);
        ogrid.info.origin.position.x = x;
        ogrid.info.origin.position.y = y;
        yarp::math::Quaternion q;
        yarp::sig::Vector v(4);
        v[0] = 0; v[1] = 0; v[2] = 1; v[3] = t * 3.14159/180;
        q.fromAxisAngle(v);
        ogrid.info.origin.orientation.x = q.x();
        ogrid.info.origin.orientation.y = q.y();
        ogrid.info.origin.orientation.z = q.z();
        ogrid.info.origin.orientation.w = q.w();
        //the occupancy grid
        ogrid.data.resize(yarpmap.width() * yarpmap.height());
        int index = 0;
        yarp::dev::Nav2D::XYCell cell;
        for (cell.y = yarpmap.height(); cell.y-- > 0;)
        {
            for (cell.x = 0; cell.x < yarpmap.width(); cell.x++)
            {
                yarpmap.getOccupancyData(cell, tmp);
                ogrid.data[index++] = (int)tmp;
            }
        }
        publisher.write();
        yCInfo(MAP_PUBLISHER) << "sent map on /map topic";
        yarp::os::Time::delay(1);
    }
    while(1);

    return 0;
}
