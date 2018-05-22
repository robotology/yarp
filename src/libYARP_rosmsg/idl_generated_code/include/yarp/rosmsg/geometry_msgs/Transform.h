/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "geometry_msgs/Transform" msg definition:
//   # This represents the transform between two coordinate frames in free space.
//   
//   Vector3 translation
//   Quaternion rotation
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Transform_h
#define YARP_ROSMSG_geometry_msgs_Transform_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Vector3.h>
#include <yarp/rosmsg/geometry_msgs/Quaternion.h>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Transform : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::geometry_msgs::Vector3 translation;
    yarp::rosmsg::geometry_msgs::Quaternion rotation;

    Transform() :
            translation(),
            rotation()
    {
    }

    void clear()
    {
        // *** translation ***
        translation.clear();

        // *** rotation ***
        rotation.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** translation ***
        if (!translation.read(connection)) {
            return false;
        }

        // *** rotation ***
        if (!rotation.read(connection)) {
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

        // *** translation ***
        if (!translation.read(connection)) {
            return false;
        }

        // *** rotation ***
        if (!rotation.read(connection)) {
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
        // *** translation ***
        if (!translation.write(connection)) {
            return false;
        }

        // *** rotation ***
        if (!rotation.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** translation ***
        if (!translation.write(connection)) {
            return false;
        }

        // *** rotation ***
        if (!rotation.write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Transform> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Transform> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# This represents the transform between two coordinate frames in free space.\n\
\n\
Vector3 translation\n\
Quaternion rotation\n\
") + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
") + yarp::rosmsg::geometry_msgs::Vector3::typeText() + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
") + yarp::rosmsg::geometry_msgs::Quaternion::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::geometry_msgs::Transform::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Transform", "geometry_msgs/Transform");
        typ.addProperty("md5sum", yarp::os::Value("ac9eff44abf714214112b05d54a3cf9b"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_Transform_h
