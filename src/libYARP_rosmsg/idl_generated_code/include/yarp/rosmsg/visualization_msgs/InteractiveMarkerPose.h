/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "visualization_msgs/InteractiveMarkerPose" msg definition:
//   # Time/frame info.
//   Header header
//   
//   # Initial pose. Also, defines the pivot point for rotations.
//   geometry_msgs/Pose pose
//   
//   # Identifying string. Must be globally unique in
//   # the topic that this message is sent through.
//   string name
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_InteractiveMarkerPose_h
#define YARP_ROSMSG_visualization_msgs_InteractiveMarkerPose_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class InteractiveMarkerPose : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::geometry_msgs::Pose pose;
    std::string name;

    InteractiveMarkerPose() :
            header(),
            pose(),
            name("")
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** pose ***
        pose.clear();

        // *** name ***
        name = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** name ***
        int len = connection.expectInt();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
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

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** name ***
        if (!reader.readString(name)) {
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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** name ***
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** name ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerPose> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerPose> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# Time/frame info.\n\
Header header\n\
\n\
# Initial pose. Also, defines the pivot point for rotations.\n\
geometry_msgs/Pose pose\n\
\n\
# Identifying string. Must be globally unique in\n\
# the topic that this message is sent through.\n\
string name\n\
") + std::string("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/Pose\n\
") + yarp::rosmsg::geometry_msgs::Pose::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::visualization_msgs::InteractiveMarkerPose::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/InteractiveMarkerPose", "visualization_msgs/InteractiveMarkerPose");
        typ.addProperty("md5sum", yarp::os::Value("a6e6833209a196a38d798dadb02c81f8"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_InteractiveMarkerPose_h
