/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "geometry_msgs/Point" msg definition:
//   # This contains the position of a point in free space
//   float64 x
//   float64 y
//   float64 z
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Point_h
#define YARP_ROSMSG_geometry_msgs_Point_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Point : public yarp::os::idl::WirePortable
{
public:
    yarp::os::NetFloat64 x;
    yarp::os::NetFloat64 y;
    yarp::os::NetFloat64 z;

    Point() :
            x(0.0),
            y(0.0),
            z(0.0)
    {
    }

    void clear()
    {
        // *** x ***
        x = 0.0;

        // *** y ***
        y = 0.0;

        // *** z ***
        z = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** x ***
        x = connection.expectDouble();

        // *** y ***
        y = connection.expectDouble();

        // *** z ***
        z = connection.expectDouble();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** x ***
        x = reader.expectDouble();

        // *** y ***
        y = reader.expectDouble();

        // *** z ***
        z = reader.expectDouble();

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
        // *** x ***
        connection.appendDouble(x);

        // *** y ***
        connection.appendDouble(y);

        // *** z ***
        connection.appendDouble(z);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** x ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)x);

        // *** y ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)y);

        // *** z ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)z);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Point> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Point> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
");
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::geometry_msgs::Point::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Point", "geometry_msgs/Point");
        typ.addProperty("md5sum", yarp::os::Value("4a842b65f413084dc2b10fb484ea7f17"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_Point_h
