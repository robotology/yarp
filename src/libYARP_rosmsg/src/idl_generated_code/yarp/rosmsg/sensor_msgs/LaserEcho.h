/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class LaserEcho : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::conf::float32_t> echoes;

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
        int len = connection.expectInt32();
        echoes.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&echoes[0], sizeof(yarp::conf::float32_t)*len)) {
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
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32)) {
            return false;
        }
        int len = connection.expectInt32();
        echoes.resize(len);
        for (int i=0; i<len; i++) {
            echoes[i] = (yarp::conf::float32_t)connection.expectFloat32();
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
        // *** echoes ***
        connection.appendInt32(echoes.size());
        if (echoes.size()>0) {
            connection.appendExternalBlock((char*)&echoes[0], sizeof(yarp::conf::float32_t)*echoes.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(1);

        // *** echoes ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32);
        connection.appendInt32(echoes.size());
        for (size_t i=0; i<echoes.size(); i++) {
            connection.appendFloat32(echoes[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::LaserEcho> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::LaserEcho> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/LaserEcho";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "8bc5ae449b200fba4d552b4225586696";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This message is a submessage of MultiEchoLaserScan and is not intended\n\
# to be used separately.\n\
\n\
float32[] echoes  # Multiple values of ranges or intensities.\n\
                  # Each array represents data from the same angle increment.\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_LaserEcho_h
