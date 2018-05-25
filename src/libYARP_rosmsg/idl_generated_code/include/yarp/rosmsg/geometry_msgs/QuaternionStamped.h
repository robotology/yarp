/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "geometry_msgs/QuaternionStamped" msg definition:
//   # This represents an orientation with reference coordinate frame and timestamp.
//   
//   Header header
//   Quaternion quaternion
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_QuaternionStamped_h
#define YARP_ROSMSG_geometry_msgs_QuaternionStamped_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Quaternion.h>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class QuaternionStamped : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::geometry_msgs::Quaternion quaternion;

    QuaternionStamped() :
            header(),
            quaternion()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** quaternion ***
        quaternion.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** quaternion ***
        if (!quaternion.read(connection)) {
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

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** quaternion ***
        if (!quaternion.read(connection)) {
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

        // *** quaternion ***
        if (!quaternion.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** quaternion ***
        if (!quaternion.write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::QuaternionStamped> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::QuaternionStamped> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# This represents an orientation with reference coordinate frame and timestamp.\n\
\n\
Header header\n\
Quaternion quaternion\n\
") + std::string("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
") + yarp::rosmsg::geometry_msgs::Quaternion::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::geometry_msgs::QuaternionStamped::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/QuaternionStamped", "geometry_msgs/QuaternionStamped");
        typ.addProperty("md5sum", yarp::os::Value("e57f1e547e0e1fd13504588ffc8334e2"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_QuaternionStamped_h
