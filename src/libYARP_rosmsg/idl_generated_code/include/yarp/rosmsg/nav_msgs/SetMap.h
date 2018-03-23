/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "nav_msgs/SetMap" msg definition:
//   # Set a new map together with an initial pose
//   nav_msgs/OccupancyGrid map
//   geometry_msgs/PoseWithCovarianceStamped initial_pose
//   ---
//   bool success
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_nav_msgs_SetMap_h
#define YARP_ROSMSG_nav_msgs_SetMap_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/nav_msgs/OccupancyGrid.h>
#include <yarp/rosmsg/geometry_msgs/PoseWithCovarianceStamped.h>

namespace yarp {
namespace rosmsg {
namespace nav_msgs {

class SetMap : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::nav_msgs::OccupancyGrid map;
    yarp::rosmsg::geometry_msgs::PoseWithCovarianceStamped initial_pose;

    SetMap() :
            map(),
            initial_pose()
    {
    }

    void clear()
    {
        // *** map ***
        map.clear();

        // *** initial_pose ***
        initial_pose.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** map ***
        if (!map.read(connection)) {
            return false;
        }

        // *** initial_pose ***
        if (!initial_pose.read(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(2)) {
            return false;
        }

        // *** map ***
        if (!map.read(connection)) {
            return false;
        }

        // *** initial_pose ***
        if (!initial_pose.read(connection)) {
            return false;
        }

        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::read;
    bool read(yarp::os::ConnectionReader& connection) override
    {
        return (connection.isBareMode() ? readBare(connection)
                                        : readBottle(connection));
    }

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** map ***
        if (!map.write(connection)) {
            return false;
        }

        // *** initial_pose ***
        if (!initial_pose.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** map ***
        if (!map.write(connection)) {
            return false;
        }

        // *** initial_pose ***
        if (!initial_pose.write(connection)) {
            return false;
        }

        connection.convertTextMode();
        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::write;
    bool write(yarp::os::ConnectionWriter& connection) override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::SetMap> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::SetMap> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
# Set a new map together with an initial pose\n\
nav_msgs/OccupancyGrid map\n\
geometry_msgs/PoseWithCovarianceStamped initial_pose\n\
---\n\
bool success\n\
\n\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: nav_msgs/OccupancyGrid\n\
") + yarp::rosmsg::nav_msgs::OccupancyGrid::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: geometry_msgs/PoseWithCovarianceStamped\n\
") + yarp::rosmsg::geometry_msgs::PoseWithCovarianceStamped::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::nav_msgs::SetMap::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/SetMap", "nav_msgs/SetMap");
        typ.addProperty("md5sum", yarp::os::Value("85980e5164caf4e7f9849c41c8c25a05"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_SetMap_h
