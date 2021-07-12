/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "shape_msgs/Plane" msg definition:
//   # Representation of a plane, using the plane equation ax + by + cz + d = 0
//   
//   # a := coef[0]
//   # b := coef[1]
//   # c := coef[2]
//   # d := coef[3]
//   
//   float64[4] coef
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_shape_msgs_Plane_h
#define YARP_ROSMSG_shape_msgs_Plane_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace shape_msgs {

class Plane : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::conf::float64_t> coef;

    Plane() :
            coef()
    {
        coef.resize(4, 0.0);
    }

    void clear()
    {
        // *** coef ***
        coef.clear();
        coef.resize(4, 0.0);
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** coef ***
        int len = 4;
        coef.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&coef[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(1)) {
            return false;
        }

        // *** coef ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        int len = connection.expectInt32();
        coef.resize(len);
        for (int i=0; i<len; i++) {
            coef[i] = (yarp::conf::float64_t)connection.expectFloat64();
        }

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
        // *** coef ***
        if (coef.size()>0) {
            connection.appendExternalBlock((char*)&coef[0], sizeof(yarp::conf::float64_t)*coef.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(1);

        // *** coef ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(coef.size());
        for (size_t i=0; i<coef.size(); i++) {
            connection.appendFloat64(coef[i]);
        }

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::shape_msgs::Plane> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::shape_msgs::Plane> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "shape_msgs/Plane";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "2c1b92ed8f31492f8e73f6a4a44ca796";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Representation of a plane, using the plane equation ax + by + cz + d = 0\n\
\n\
# a := coef[0]\n\
# b := coef[1]\n\
# c := coef[2]\n\
# d := coef[3]\n\
\n\
float64[4] coef\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace shape_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_shape_msgs_Plane_h
