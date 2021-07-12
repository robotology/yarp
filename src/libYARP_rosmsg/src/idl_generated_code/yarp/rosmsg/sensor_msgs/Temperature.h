/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/Temperature" msg definition:
//    # Single temperature reading.
//   
//    Header header           # timestamp is the time the temperature was measured
//                            # frame_id is the location of the temperature reading
//   
//    float64 temperature     # Measurement of the Temperature in Degrees Celsius
//   
//    float64 variance        # 0 is interpreted as variance unknown// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_Temperature_h
#define YARP_ROSMSG_sensor_msgs_Temperature_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class Temperature : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::conf::float64_t temperature;
    yarp::conf::float64_t variance;

    Temperature() :
            header(),
            temperature(0.0),
            variance(0.0)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** temperature ***
        temperature = 0.0;

        // *** variance ***
        variance = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** temperature ***
        temperature = connection.expectFloat64();

        // *** variance ***
        variance = connection.expectFloat64();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** temperature ***
        temperature = reader.expectFloat64();

        // *** variance ***
        variance = reader.expectFloat64();

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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** temperature ***
        connection.appendFloat64(temperature);

        // *** variance ***
        connection.appendFloat64(variance);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** temperature ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(temperature);

        // *** variance ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(variance);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::Temperature> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::Temperature> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/Temperature";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "ff71b307acdbe7c871a5a6d7ed359100";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
 # Single temperature reading.\n\
\n\
 Header header           # timestamp is the time the temperature was measured\n\
                         # frame_id is the location of the temperature reading\n\
\n\
 float64 temperature     # Measurement of the Temperature in Degrees Celsius\n\
\n\
 float64 variance        # 0 is interpreted as variance unknown\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n\
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
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

#endif // YARP_ROSMSG_sensor_msgs_Temperature_h
