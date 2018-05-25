/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "geometry_msgs/Accel" msg definition:
//   # This expresses acceleration in free space broken into its linear and angular parts.
//   Vector3  linear
//   Vector3  angular
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Accel_h
#define YARP_ROSMSG_geometry_msgs_Accel_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Vector3.h>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Accel : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::geometry_msgs::Vector3 linear;
    yarp::rosmsg::geometry_msgs::Vector3 angular;

    Accel() :
            linear(),
            angular()
    {
    }

    void clear()
    {
        // *** linear ***
        linear.clear();

        // *** angular ***
        angular.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** linear ***
        if (!linear.read(connection)) {
            return false;
        }

        // *** angular ***
        if (!angular.read(connection)) {
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

        // *** linear ***
        if (!linear.read(connection)) {
            return false;
        }

        // *** angular ***
        if (!angular.read(connection)) {
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
        // *** linear ***
        if (!linear.write(connection)) {
            return false;
        }

        // *** angular ***
        if (!angular.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** linear ***
        if (!linear.write(connection)) {
            return false;
        }

        // *** angular ***
        if (!angular.write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Accel> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Accel> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# This expresses acceleration in free space broken into its linear and angular parts.\n\
Vector3  linear\n\
Vector3  angular\n\
") + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
") + yarp::rosmsg::geometry_msgs::Vector3::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::geometry_msgs::Accel::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Accel", "geometry_msgs/Accel");
        typ.addProperty("md5sum", yarp::os::Value("9f195f881246fdfa2798d1d3eebca84a"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_Accel_h
