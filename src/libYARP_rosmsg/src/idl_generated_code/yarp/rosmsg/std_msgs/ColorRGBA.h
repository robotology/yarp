/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class ColorRGBA : public yarp::os::idl::WirePortable
{
public:
    yarp::conf::float32_t r;
    yarp::conf::float32_t g;
    yarp::conf::float32_t b;
    yarp::conf::float32_t a;

    ColorRGBA() :
            r(0.0f),
            g(0.0f),
            b(0.0f),
            a(0.0f)
    {
    }

    void clear()
    {
        // *** r ***
        r = 0.0f;

        // *** g ***
        g = 0.0f;

        // *** b ***
        b = 0.0f;

        // *** a ***
        a = 0.0f;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** r ***
        r = connection.expectFloat32();

        // *** g ***
        g = connection.expectFloat32();

        // *** b ***
        b = connection.expectFloat32();

        // *** a ***
        a = connection.expectFloat32();

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
        r = reader.expectFloat32();

        // *** g ***
        g = reader.expectFloat32();

        // *** b ***
        b = reader.expectFloat32();

        // *** a ***
        a = reader.expectFloat32();

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
        // *** r ***
        connection.appendFloat32(r);

        // *** g ***
        connection.appendFloat32(g);

        // *** b ***
        connection.appendFloat32(b);

        // *** a ***
        connection.appendFloat32(a);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(4);

        // *** r ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(r);

        // *** g ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(g);

        // *** b ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(b);

        // *** a ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(a);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::ColorRGBA> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::ColorRGBA> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "std_msgs/ColorRGBA";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "a29a96539573343b1310c73607334b00";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
float32 r\n\
float32 g\n\
float32 b\n\
float32 a\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_ColorRGBA_h
