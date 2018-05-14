/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "std_msgs/ColorRGBA" msg definition:
//   float32 r
//   float32 g
//   float32 b
//   float32 a
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_std_msgs_ColorRGBA_h
#define YARP_ROSMSG_std_msgs_ColorRGBA_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class ColorRGBA : public yarp::os::idl::WirePortable
{
public:
    yarp::os::NetFloat32 r;
    yarp::os::NetFloat32 g;
    yarp::os::NetFloat32 b;
    yarp::os::NetFloat32 a;

    ColorRGBA() :
            r(0.0),
            g(0.0),
            b(0.0),
            a(0.0)
    {
    }

    void clear()
    {
        // *** r ***
        r = 0.0;

        // *** g ***
        g = 0.0;

        // *** b ***
        b = 0.0;

        // *** a ***
        a = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** r ***
        if (!connection.expectBlock((char*)&r, 4)) {
            return false;
        }

        // *** g ***
        if (!connection.expectBlock((char*)&g, 4)) {
            return false;
        }

        // *** b ***
        if (!connection.expectBlock((char*)&b, 4)) {
            return false;
        }

        // *** a ***
        if (!connection.expectBlock((char*)&a, 4)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(4)) {
            return false;
        }

        // *** r ***
        r = reader.expectDouble();

        // *** g ***
        g = reader.expectDouble();

        // *** b ***
        b = reader.expectDouble();

        // *** a ***
        a = reader.expectDouble();

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
        // *** r ***
        connection.appendBlock((char*)&r, 4);

        // *** g ***
        connection.appendBlock((char*)&g, 4);

        // *** b ***
        connection.appendBlock((char*)&b, 4);

        // *** a ***
        connection.appendBlock((char*)&a, 4);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(4);

        // *** r ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)r);

        // *** g ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)g);

        // *** b ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)b);

        // *** a ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)a);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::ColorRGBA> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::ColorRGBA> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
float32 r\n\
float32 g\n\
float32 b\n\
float32 a\n\
");
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::std_msgs::ColorRGBA::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("std_msgs/ColorRGBA", "std_msgs/ColorRGBA");
        typ.addProperty("md5sum", yarp::os::Value("a29a96539573343b1310c73607334b00"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_ColorRGBA_h
