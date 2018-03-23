/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "nav_msgs/GetPlanReply" msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_nav_msgs_GetPlanReply_h
#define YARP_ROSMSG_nav_msgs_GetPlanReply_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/nav_msgs/Path.h>

namespace yarp {
namespace rosmsg {
namespace nav_msgs {

class GetPlanReply : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::nav_msgs::Path plan;

    GetPlanReply() :
            plan()
    {
    }

    void clear()
    {
        // *** plan ***
        plan.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** plan ***
        if (!plan.read(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(1)) {
            return false;
        }

        // *** plan ***
        if (!plan.read(connection)) {
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
        // *** plan ***
        if (!plan.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(1);

        // *** plan ***
        if (!plan.write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::GetPlanReply> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::GetPlanReply> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: nav_msgs/Path\n\
") + yarp::rosmsg::nav_msgs::Path::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::nav_msgs::GetPlanReply::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/GetPlanReply", "nav_msgs/GetPlanReply");
        typ.addProperty("md5sum", yarp::os::Value("6dfb16dea9d3887d02805fba77da7d13"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_GetPlanReply_h
