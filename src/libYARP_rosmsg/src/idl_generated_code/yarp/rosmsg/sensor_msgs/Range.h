/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/Range" msg definition:
//   # Single range reading from an active ranger that emits energy and reports
//   # one range reading that is valid along an arc at the distance measured. 
//   # This message is  not appropriate for laser scanners. See the LaserScan
//   # message if you are working with a laser scanner.
//   
//   # This message also can represent a fixed-distance (binary) ranger.  This
//   # sensor will have min_range===max_range===distance of detection.
//   # These sensors follow REP 117 and will output -Inf if the object is detected
//   # and +Inf if the object is outside of the detection range.
//   
//   Header header           # timestamp in the header is the time the ranger
//                           # returned the distance reading
//   
//   # Radiation type enums
//   # If you want a value added to this list, send an email to the ros-users list
//   uint8 ULTRASOUND=0
//   uint8 INFRARED=1
//   
//   uint8 radiation_type    # the type of radiation used by the sensor
//                           # (sound, IR, etc) [enum]
//   
//   float32 field_of_view   # the size of the arc that the distance reading is
//                           # valid for [rad]
//                           # the object causing the range reading may have
//                           # been anywhere within -field_of_view/2 and
//                           # field_of_view/2 at the measured range. 
//                           # 0 angle corresponds to the x-axis of the sensor.
//   
//   float32 min_range       # minimum range value [m]
//   float32 max_range       # maximum range value [m]
//                           # Fixed distance rangers require min_range==max_range
//   
//   float32 range           # range data [m]
//                           # (Note: values < range_min or > range_max
//                           # should be discarded)
//                           # Fixed distance rangers only output -Inf or +Inf.
//                           # -Inf represents a detection within fixed distance.
//                           # (Detection too close to the sensor to quantify)
//                           # +Inf represents no detection within the fixed distance.
//                           # (Object out of range)// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_Range_h
#define YARP_ROSMSG_sensor_msgs_Range_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class Range : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    static const std::uint8_t ULTRASOUND = 0;
    static const std::uint8_t INFRARED = 1;
    std::uint8_t radiation_type;
    yarp::conf::float32_t field_of_view;
    yarp::conf::float32_t min_range;
    yarp::conf::float32_t max_range;
    yarp::conf::float32_t range;

    Range() :
            header(),
            radiation_type(0),
            field_of_view(0.0f),
            min_range(0.0f),
            max_range(0.0f),
            range(0.0f)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** ULTRASOUND ***

        // *** INFRARED ***

        // *** radiation_type ***
        radiation_type = 0;

        // *** field_of_view ***
        field_of_view = 0.0f;

        // *** min_range ***
        min_range = 0.0f;

        // *** max_range ***
        max_range = 0.0f;

        // *** range ***
        range = 0.0f;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** radiation_type ***
        radiation_type = connection.expectInt8();

        // *** field_of_view ***
        field_of_view = connection.expectFloat32();

        // *** min_range ***
        min_range = connection.expectFloat32();

        // *** max_range ***
        max_range = connection.expectFloat32();

        // *** range ***
        range = connection.expectFloat32();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(8)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** radiation_type ***
        radiation_type = reader.expectInt8();

        // *** field_of_view ***
        field_of_view = reader.expectFloat32();

        // *** min_range ***
        min_range = reader.expectFloat32();

        // *** max_range ***
        max_range = reader.expectFloat32();

        // *** range ***
        range = reader.expectFloat32();

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

        // *** radiation_type ***
        connection.appendInt8(radiation_type);

        // *** field_of_view ***
        connection.appendFloat32(field_of_view);

        // *** min_range ***
        connection.appendFloat32(min_range);

        // *** max_range ***
        connection.appendFloat32(max_range);

        // *** range ***
        connection.appendFloat32(range);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(8);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** radiation_type ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(radiation_type);

        // *** field_of_view ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(field_of_view);

        // *** min_range ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(min_range);

        // *** max_range ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(max_range);

        // *** range ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(range);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::Range> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::Range> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/Range";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "c005c34273dc426c67a020a87bc24148";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Single range reading from an active ranger that emits energy and reports\n\
# one range reading that is valid along an arc at the distance measured. \n\
# This message is  not appropriate for laser scanners. See the LaserScan\n\
# message if you are working with a laser scanner.\n\
\n\
# This message also can represent a fixed-distance (binary) ranger.  This\n\
# sensor will have min_range===max_range===distance of detection.\n\
# These sensors follow REP 117 and will output -Inf if the object is detected\n\
# and +Inf if the object is outside of the detection range.\n\
\n\
Header header           # timestamp in the header is the time the ranger\n\
                        # returned the distance reading\n\
\n\
# Radiation type enums\n\
# If you want a value added to this list, send an email to the ros-users list\n\
uint8 ULTRASOUND=0\n\
uint8 INFRARED=1\n\
\n\
uint8 radiation_type    # the type of radiation used by the sensor\n\
                        # (sound, IR, etc) [enum]\n\
\n\
float32 field_of_view   # the size of the arc that the distance reading is\n\
                        # valid for [rad]\n\
                        # the object causing the range reading may have\n\
                        # been anywhere within -field_of_view/2 and\n\
                        # field_of_view/2 at the measured range. \n\
                        # 0 angle corresponds to the x-axis of the sensor.\n\
\n\
float32 min_range       # minimum range value [m]\n\
float32 max_range       # maximum range value [m]\n\
                        # Fixed distance rangers require min_range==max_range\n\
\n\
float32 range           # range data [m]\n\
                        # (Note: values < range_min or > range_max\n\
                        # should be discarded)\n\
                        # Fixed distance rangers only output -Inf or +Inf.\n\
                        # -Inf represents a detection within fixed distance.\n\
                        # (Detection too close to the sensor to quantify)\n\
                        # +Inf represents no detection within the fixed distance.\n\
                        # (Object out of range)\n\
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

#endif // YARP_ROSMSG_sensor_msgs_Range_h
