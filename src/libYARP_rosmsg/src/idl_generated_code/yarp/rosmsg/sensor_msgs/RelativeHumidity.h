/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/RelativeHumidity" msg definition:
//    # Single reading from a relative humidity sensor.  Defines the ratio of partial
//    # pressure of water vapor to the saturated vapor pressure at a temperature.
//   
//    Header header             # timestamp of the measurement
//                              # frame_id is the location of the humidity sensor
//   
//    float64 relative_humidity # Expression of the relative humidity
//                              # from 0.0 to 1.0.
//                              # 0.0 is no partial pressure of water vapor
//                              # 1.0 represents partial pressure of saturation
//   
//    float64 variance          # 0 is interpreted as variance unknown// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_RelativeHumidity_h
#define YARP_ROSMSG_sensor_msgs_RelativeHumidity_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class RelativeHumidity : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::conf::float64_t relative_humidity;
    yarp::conf::float64_t variance;

    RelativeHumidity() :
            header(),
            relative_humidity(0.0),
            variance(0.0)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** relative_humidity ***
        relative_humidity = 0.0;

        // *** variance ***
        variance = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** relative_humidity ***
        relative_humidity = connection.expectFloat64();

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

        // *** relative_humidity ***
        relative_humidity = reader.expectFloat64();

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

        // *** relative_humidity ***
        connection.appendFloat64(relative_humidity);

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

        // *** relative_humidity ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(relative_humidity);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::RelativeHumidity> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::RelativeHumidity> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/RelativeHumidity";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "8730015b05955b7e992ce29a2678d90f";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
 # Single reading from a relative humidity sensor.  Defines the ratio of partial\n\
 # pressure of water vapor to the saturated vapor pressure at a temperature.\n\
\n\
 Header header             # timestamp of the measurement\n\
                           # frame_id is the location of the humidity sensor\n\
\n\
 float64 relative_humidity # Expression of the relative humidity\n\
                           # from 0.0 to 1.0.\n\
                           # 0.0 is no partial pressure of water vapor\n\
                           # 1.0 represents partial pressure of saturation\n\
\n\
 float64 variance          # 0 is interpreted as variance unknown\n\
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

#endif // YARP_ROSMSG_sensor_msgs_RelativeHumidity_h
