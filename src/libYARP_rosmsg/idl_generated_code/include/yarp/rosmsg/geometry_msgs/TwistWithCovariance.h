/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "geometry_msgs/TwistWithCovariance" msg definition:
//   # This expresses velocity in free space with uncertainty.
//   
//   Twist twist
//   
//   # Row-major representation of the 6x6 covariance matrix
//   # The orientation parameters use a fixed-axis representation.
//   # In order, the parameters are:
//   # (x, y, z, rotation about X axis, rotation about Y axis, rotation about Z axis)
//   float64[36] covariance
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_TwistWithCovariance_h
#define YARP_ROSMSG_geometry_msgs_TwistWithCovariance_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Twist.h>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class TwistWithCovariance : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::geometry_msgs::Twist twist;
    std::vector<yarp::conf::float64_t> covariance;

    TwistWithCovariance() :
            twist(),
            covariance()
    {
        covariance.resize(36, 0.0);
    }

    void clear()
    {
        // *** twist ***
        twist.clear();

        // *** covariance ***
        covariance.clear();
        covariance.resize(36, 0.0);
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** twist ***
        if (!twist.read(connection)) {
            return false;
        }

        // *** covariance ***
        int len = 36;
        covariance.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&covariance[0], sizeof(yarp::conf::float64_t)*len)) {
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

        // *** twist ***
        if (!twist.read(connection)) {
            return false;
        }

        // *** covariance ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        int len = connection.expectInt32();
        covariance.resize(len);
        for (int i=0; i<len; i++) {
            covariance[i] = (yarp::conf::float64_t)connection.expectFloat64();
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
        // *** twist ***
        if (!twist.write(connection)) {
            return false;
        }

        // *** covariance ***
        if (covariance.size()>0) {
            connection.appendExternalBlock((char*)&covariance[0], sizeof(yarp::conf::float64_t)*covariance.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** twist ***
        if (!twist.write(connection)) {
            return false;
        }

        // *** covariance ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(covariance.size());
        for (size_t i=0; i<covariance.size(); i++) {
            connection.appendFloat64(covariance[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::TwistWithCovariance> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::TwistWithCovariance> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# This expresses velocity in free space with uncertainty.\n\
\n\
Twist twist\n\
\n\
# Row-major representation of the 6x6 covariance matrix\n\
# The orientation parameters use a fixed-axis representation.\n\
# In order, the parameters are:\n\
# (x, y, z, rotation about X axis, rotation about Y axis, rotation about Z axis)\n\
float64[36] covariance\n\
") + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/Twist\n\
") + yarp::rosmsg::geometry_msgs::Twist::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::geometry_msgs::TwistWithCovariance::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/TwistWithCovariance", "geometry_msgs/TwistWithCovariance");
        typ.addProperty("md5sum", yarp::os::Value("1fe8a28e6890a4cc3ae4c3ca5c7d82e6"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_TwistWithCovariance_h
