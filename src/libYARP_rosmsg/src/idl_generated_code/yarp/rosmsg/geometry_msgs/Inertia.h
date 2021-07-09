/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "geometry_msgs/Inertia" msg definition:
//   # Mass [kg]
//   float64 m
//   
//   # Center of mass [m]
//   geometry_msgs/Vector3 com
//   
//   # Inertia Tensor [kg-m^2]
//   #     | ixx ixy ixz |
//   # I = | ixy iyy iyz |
//   #     | ixz iyz izz |
//   float64 ixx
//   float64 ixy
//   float64 ixz
//   float64 iyy
//   float64 iyz
//   float64 izz
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Inertia_h
#define YARP_ROSMSG_geometry_msgs_Inertia_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Vector3.h>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Inertia : public yarp::os::idl::WirePortable
{
public:
    yarp::conf::float64_t m;
    yarp::rosmsg::geometry_msgs::Vector3 com;
    yarp::conf::float64_t ixx;
    yarp::conf::float64_t ixy;
    yarp::conf::float64_t ixz;
    yarp::conf::float64_t iyy;
    yarp::conf::float64_t iyz;
    yarp::conf::float64_t izz;

    Inertia() :
            m(0.0),
            com(),
            ixx(0.0),
            ixy(0.0),
            ixz(0.0),
            iyy(0.0),
            iyz(0.0),
            izz(0.0)
    {
    }

    void clear()
    {
        // *** m ***
        m = 0.0;

        // *** com ***
        com.clear();

        // *** ixx ***
        ixx = 0.0;

        // *** ixy ***
        ixy = 0.0;

        // *** ixz ***
        ixz = 0.0;

        // *** iyy ***
        iyy = 0.0;

        // *** iyz ***
        iyz = 0.0;

        // *** izz ***
        izz = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** m ***
        m = connection.expectFloat64();

        // *** com ***
        if (!com.read(connection)) {
            return false;
        }

        // *** ixx ***
        ixx = connection.expectFloat64();

        // *** ixy ***
        ixy = connection.expectFloat64();

        // *** ixz ***
        ixz = connection.expectFloat64();

        // *** iyy ***
        iyy = connection.expectFloat64();

        // *** iyz ***
        iyz = connection.expectFloat64();

        // *** izz ***
        izz = connection.expectFloat64();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(8)) {
            return false;
        }

        // *** m ***
        m = reader.expectFloat64();

        // *** com ***
        if (!com.read(connection)) {
            return false;
        }

        // *** ixx ***
        ixx = reader.expectFloat64();

        // *** ixy ***
        ixy = reader.expectFloat64();

        // *** ixz ***
        ixz = reader.expectFloat64();

        // *** iyy ***
        iyy = reader.expectFloat64();

        // *** iyz ***
        iyz = reader.expectFloat64();

        // *** izz ***
        izz = reader.expectFloat64();

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
        // *** m ***
        connection.appendFloat64(m);

        // *** com ***
        if (!com.write(connection)) {
            return false;
        }

        // *** ixx ***
        connection.appendFloat64(ixx);

        // *** ixy ***
        connection.appendFloat64(ixy);

        // *** ixz ***
        connection.appendFloat64(ixz);

        // *** iyy ***
        connection.appendFloat64(iyy);

        // *** iyz ***
        connection.appendFloat64(iyz);

        // *** izz ***
        connection.appendFloat64(izz);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(8);

        // *** m ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(m);

        // *** com ***
        if (!com.write(connection)) {
            return false;
        }

        // *** ixx ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(ixx);

        // *** ixy ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(ixy);

        // *** ixz ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(ixz);

        // *** iyy ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(iyy);

        // *** iyz ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(iyz);

        // *** izz ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(izz);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Inertia> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Inertia> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "geometry_msgs/Inertia";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "1d26e4bb6c83ff141c5cf0d883c2b0fe";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Mass [kg]\n\
float64 m\n\
\n\
# Center of mass [m]\n\
geometry_msgs/Vector3 com\n\
\n\
# Inertia Tensor [kg-m^2]\n\
#     | ixx ixy ixz |\n\
# I = | ixy iyy iyz |\n\
#     | ixz iyz izz |\n\
float64 ixx\n\
float64 ixy\n\
float64 ixz\n\
float64 iyy\n\
float64 iyz\n\
float64 izz\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
# This represents a vector in free space. \n\
# It is only meant to represent a direction. Therefore, it does not\n\
# make sense to apply a translation to it (e.g., when applying a \n\
# generic rigid transformation to a Vector3, tf2 will only apply the\n\
# rotation). If you want your data to be translatable too, use the\n\
# geometry_msgs/Point message instead.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
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

#endif // YARP_ROSMSG_geometry_msgs_Inertia_h
