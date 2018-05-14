/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/LaserEcho" msg definition:
//   # This message is a submessage of MultiEchoLaserScan and is not intended
//   # to be used separately.
//   
//   float32[] echoes  # Multiple values of ranges or intensities.
//                     # Each array represents data from the same angle increment.// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_LaserEcho_h
#define YARP_ROSMSG_sensor_msgs_LaserEcho_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class LaserEcho : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::os::NetFloat32> echoes;

    LaserEcho() :
            echoes()
    {
    }

    void clear()
    {
        // *** echoes ***
        echoes.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** echoes ***
        int len = connection.expectInt();
        echoes.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&echoes[0], sizeof(yarp::os::NetFloat32)*len)) {
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

        // *** echoes ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        echoes.resize(len);
        for (int i=0; i<len; i++) {
            echoes[i] = (yarp::os::NetFloat32)connection.expectDouble();
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
        // *** echoes ***
        connection.appendInt(echoes.size());
        if (echoes.size()>0) {
            connection.appendExternalBlock((char*)&echoes[0], sizeof(yarp::os::NetFloat32)*echoes.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(1);

        // *** echoes ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(echoes.size());
        for (size_t i=0; i<echoes.size(); i++) {
            connection.appendDouble((double)echoes[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::LaserEcho> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::LaserEcho> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# This message is a submessage of MultiEchoLaserScan and is not intended\n\
# to be used separately.\n\
\n\
float32[] echoes  # Multiple values of ranges or intensities.\n\
                  # Each array represents data from the same angle increment.");
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::LaserEcho::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/LaserEcho", "sensor_msgs/LaserEcho");
        typ.addProperty("md5sum", yarp::os::Value("8bc5ae449b200fba4d552b4225586696"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_LaserEcho_h
