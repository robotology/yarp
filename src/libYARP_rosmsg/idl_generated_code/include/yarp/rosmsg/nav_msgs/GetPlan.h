/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "nav_msgs/GetPlan" msg definition:
//   # Get a plan from the current position to the goal Pose 
//   
//   # The start pose for the plan
//   geometry_msgs/PoseStamped start
//   
//   # The final pose of the goal position
//   geometry_msgs/PoseStamped goal
//   
//   # If the goal is obstructed, how many meters the planner can 
//   # relax the constraint in x and y before failing. 
//   float32 tolerance
//   ---
//   nav_msgs/Path plan
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_nav_msgs_GetPlan_h
#define YARP_ROSMSG_nav_msgs_GetPlan_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/PoseStamped.h>

namespace yarp {
namespace rosmsg {
namespace nav_msgs {

class GetPlan : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::geometry_msgs::PoseStamped start;
    yarp::rosmsg::geometry_msgs::PoseStamped goal;
    yarp::os::NetFloat32 tolerance;

    GetPlan() :
            start(),
            goal(),
            tolerance(0.0)
    {
    }

    void clear()
    {
        // *** start ***
        start.clear();

        // *** goal ***
        goal.clear();

        // *** tolerance ***
        tolerance = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** start ***
        if (!start.read(connection)) {
            return false;
        }

        // *** goal ***
        if (!goal.read(connection)) {
            return false;
        }

        // *** tolerance ***
        if (!connection.expectBlock((char*)&tolerance, 4)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** start ***
        if (!start.read(connection)) {
            return false;
        }

        // *** goal ***
        if (!goal.read(connection)) {
            return false;
        }

        // *** tolerance ***
        tolerance = reader.expectDouble();

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
        // *** start ***
        if (!start.write(connection)) {
            return false;
        }

        // *** goal ***
        if (!goal.write(connection)) {
            return false;
        }

        // *** tolerance ***
        connection.appendBlock((char*)&tolerance, 4);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** start ***
        if (!start.write(connection)) {
            return false;
        }

        // *** goal ***
        if (!goal.write(connection)) {
            return false;
        }

        // *** tolerance ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)tolerance);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::GetPlan> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::GetPlan> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# Get a plan from the current position to the goal Pose \n\
\n\
# The start pose for the plan\n\
geometry_msgs/PoseStamped start\n\
\n\
# The final pose of the goal position\n\
geometry_msgs/PoseStamped goal\n\
\n\
# If the goal is obstructed, how many meters the planner can \n\
# relax the constraint in x and y before failing. \n\
float32 tolerance\n\
---\n\
nav_msgs/Path plan\n\
") + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/PoseStamped\n\
") + yarp::rosmsg::geometry_msgs::PoseStamped::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::nav_msgs::GetPlan::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/GetPlan", "nav_msgs/GetPlan");
        typ.addProperty("md5sum", yarp::os::Value("e25a43e0752bcca599a8c2eef8282df8"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_GetPlan_h
