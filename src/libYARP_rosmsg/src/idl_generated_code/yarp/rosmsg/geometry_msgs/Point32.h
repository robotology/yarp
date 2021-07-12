/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "geometry_msgs/Point32" msg definition:
//   # This contains the position of a point in free space(with 32 bits of precision).
//   # It is recommeded to use Point wherever possible instead of Point32.  
//   # 
//   # This recommendation is to promote interoperability.  
//   #
//   # This message is designed to take up less space when sending
//   # lots of points at once, as in the case of a PointCloud.  
//   
//   float32 x
//   float32 y
//   float32 z// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Point32_h
#define YARP_ROSMSG_geometry_msgs_Point32_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Point32 : public yarp::os::idl::WirePortable
{
public:
    yarp::conf::float32_t x;
    yarp::conf::float32_t y;
    yarp::conf::float32_t z;

    Point32() :
            x(0.0f),
            y(0.0f),
            z(0.0f)
    {
    }

    void clear()
    {
        // *** x ***
        x = 0.0f;

        // *** y ***
        y = 0.0f;

        // *** z ***
        z = 0.0f;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** x ***
        x = connection.expectFloat32();

        // *** y ***
        y = connection.expectFloat32();

        // *** z ***
        z = connection.expectFloat32();

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
        x = reader.expectFloat32();

        // *** y ***
        y = reader.expectFloat32();

        // *** z ***
        z = reader.expectFloat32();

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
        connection.appendFloat32(x);

        // *** y ***
        connection.appendFloat32(y);

        // *** z ***
        connection.appendFloat32(z);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** x ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(x);

        // *** y ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(y);

        // *** z ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(z);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Point32> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Point32> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "geometry_msgs/Point32";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "cc153912f1453b708d221682bc23d9ac";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This contains the position of a point in free space(with 32 bits of precision).\n\
# It is recommeded to use Point wherever possible instead of Point32.  \n\
# \n\
# This recommendation is to promote interoperability.  \n\
#\n\
# This message is designed to take up less space when sending\n\
# lots of points at once, as in the case of a PointCloud.  \n\
\n\
float32 x\n\
float32 y\n\
float32 z\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_Point32_h
