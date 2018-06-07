/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "geometry_msgs/Pose2D" msg definition:
//   # This expresses a position and orientation on a 2D manifold.
//   
//   float64 x
//   float64 y
//   float64 theta// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Pose2D_h
#define YARP_ROSMSG_geometry_msgs_Pose2D_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Pose2D : public yarp::os::idl::WirePortable
{
public:
    yarp::conf::float64_t x;
    yarp::conf::float64_t y;
    yarp::conf::float64_t theta;

    Pose2D() :
            x(0.0),
            y(0.0),
            theta(0.0)
    {
    }

    void clear()
    {
        // *** x ***
        x = 0.0;

        // *** y ***
        y = 0.0;

        // *** theta ***
        theta = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** x ***
        x = connection.expectFloat64();

        // *** y ***
        y = connection.expectFloat64();

        // *** theta ***
        theta = connection.expectFloat64();

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
        x = reader.expectFloat64();

        // *** y ***
        y = reader.expectFloat64();

        // *** theta ***
        theta = reader.expectFloat64();

        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::read;
    bool read(yarp::os::ConnectionReader& connection) override
    {
        return (connection.isBareMode() ? readBare(connection)
                                        : readBottle(connection));
    }

    bool writeBare(yarp::os::ConnectionWriter& connection) const override
    {
        // *** x ***
        connection.appendFloat64(x);

        // *** y ***
        connection.appendFloat64(y);

        // *** theta ***
        connection.appendFloat64(theta);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** x ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(x);

        // *** y ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(y);

        // *** theta ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(theta);

        connection.convertTextMode();
        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::write;
    bool write(yarp::os::ConnectionWriter& connection) const override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Pose2D> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Pose2D> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# This expresses a position and orientation on a 2D manifold.\n\
\n\
float64 x\n\
float64 y\n\
float64 theta");
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::geometry_msgs::Pose2D::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Pose2D", "geometry_msgs/Pose2D");
        typ.addProperty("md5sum", yarp::os::Value("938fa65709584ad8e77d238529be13b8"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_Pose2D_h
