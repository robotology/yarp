/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/NavSatFix" msg definition:
//   # Navigation Satellite fix for any Global Navigation Satellite System
//   #
//   # Specified using the WGS 84 reference ellipsoid
//   
//   # header.stamp specifies the ROS time for this measurement (the
//   #        corresponding satellite time may be reported using the
//   #        sensor_msgs/TimeReference message).
//   #
//   # header.frame_id is the frame of reference reported by the satellite
//   #        receiver, usually the location of the antenna.  This is a
//   #        Euclidean frame relative to the vehicle, not a reference
//   #        ellipsoid.
//   Header header
//   
//   # satellite fix status information
//   NavSatStatus status
//   
//   # Latitude [degrees]. Positive is north of equator; negative is south.
//   float64 latitude
//   
//   # Longitude [degrees]. Positive is east of prime meridian; negative is west.
//   float64 longitude
//   
//   # Altitude [m]. Positive is above the WGS 84 ellipsoid
//   # (quiet NaN if no altitude is available).
//   float64 altitude
//   
//   # Position covariance [m^2] defined relative to a tangential plane
//   # through the reported position. The components are East, North, and
//   # Up (ENU), in row-major order.
//   #
//   # Beware: this coordinate system exhibits singularities at the poles.
//   
//   float64[9] position_covariance
//   
//   # If the covariance of the fix is known, fill it in completely. If the
//   # GPS receiver provides the variance of each measurement, put them
//   # along the diagonal. If only Dilution of Precision is available,
//   # estimate an approximate covariance from that.
//   
//   uint8 COVARIANCE_TYPE_UNKNOWN = 0
//   uint8 COVARIANCE_TYPE_APPROXIMATED = 1
//   uint8 COVARIANCE_TYPE_DIAGONAL_KNOWN = 2
//   uint8 COVARIANCE_TYPE_KNOWN = 3
//   
//   uint8 position_covariance_type
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_NavSatFix_h
#define YARP_ROSMSG_sensor_msgs_NavSatFix_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/sensor_msgs/NavSatStatus.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class NavSatFix : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::sensor_msgs::NavSatStatus status;
    yarp::conf::float64_t latitude;
    yarp::conf::float64_t longitude;
    yarp::conf::float64_t altitude;
    std::vector<yarp::conf::float64_t> position_covariance;
    static const std::uint8_t COVARIANCE_TYPE_UNKNOWN = 0;
    static const std::uint8_t COVARIANCE_TYPE_APPROXIMATED = 1;
    static const std::uint8_t COVARIANCE_TYPE_DIAGONAL_KNOWN = 2;
    static const std::uint8_t COVARIANCE_TYPE_KNOWN = 3;
    std::uint8_t position_covariance_type;

    NavSatFix() :
            header(),
            status(),
            latitude(0.0),
            longitude(0.0),
            altitude(0.0),
            position_covariance(),
            position_covariance_type(0)
    {
        position_covariance.resize(9, 0.0);
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** status ***
        status.clear();

        // *** latitude ***
        latitude = 0.0;

        // *** longitude ***
        longitude = 0.0;

        // *** altitude ***
        altitude = 0.0;

        // *** position_covariance ***
        position_covariance.clear();
        position_covariance.resize(9, 0.0);

        // *** COVARIANCE_TYPE_UNKNOWN ***

        // *** COVARIANCE_TYPE_APPROXIMATED ***

        // *** COVARIANCE_TYPE_DIAGONAL_KNOWN ***

        // *** COVARIANCE_TYPE_KNOWN ***

        // *** position_covariance_type ***
        position_covariance_type = 0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** status ***
        if (!status.read(connection)) {
            return false;
        }

        // *** latitude ***
        latitude = connection.expectFloat64();

        // *** longitude ***
        longitude = connection.expectFloat64();

        // *** altitude ***
        altitude = connection.expectFloat64();

        // *** position_covariance ***
        int len = 9;
        position_covariance.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&position_covariance[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        // *** position_covariance_type ***
        position_covariance_type = connection.expectInt8();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(11)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** status ***
        if (!status.read(connection)) {
            return false;
        }

        // *** latitude ***
        latitude = reader.expectFloat64();

        // *** longitude ***
        longitude = reader.expectFloat64();

        // *** altitude ***
        altitude = reader.expectFloat64();

        // *** position_covariance ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        int len = connection.expectInt32();
        position_covariance.resize(len);
        for (int i=0; i<len; i++) {
            position_covariance[i] = (yarp::conf::float64_t)connection.expectFloat64();
        }

        // *** position_covariance_type ***
        position_covariance_type = reader.expectInt8();

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

        // *** status ***
        if (!status.write(connection)) {
            return false;
        }

        // *** latitude ***
        connection.appendFloat64(latitude);

        // *** longitude ***
        connection.appendFloat64(longitude);

        // *** altitude ***
        connection.appendFloat64(altitude);

        // *** position_covariance ***
        if (position_covariance.size()>0) {
            connection.appendExternalBlock((char*)&position_covariance[0], sizeof(yarp::conf::float64_t)*position_covariance.size());
        }

        // *** position_covariance_type ***
        connection.appendInt8(position_covariance_type);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(11);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** status ***
        if (!status.write(connection)) {
            return false;
        }

        // *** latitude ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(latitude);

        // *** longitude ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(longitude);

        // *** altitude ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(altitude);

        // *** position_covariance ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(position_covariance.size());
        for (size_t i=0; i<position_covariance.size(); i++) {
            connection.appendFloat64(position_covariance[i]);
        }

        // *** position_covariance_type ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(position_covariance_type);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::NavSatFix> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::NavSatFix> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/NavSatFix";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "2d3a8cd499b9b4a0249fb98fd05cfa48";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Navigation Satellite fix for any Global Navigation Satellite System\n\
#\n\
# Specified using the WGS 84 reference ellipsoid\n\
\n\
# header.stamp specifies the ROS time for this measurement (the\n\
#        corresponding satellite time may be reported using the\n\
#        sensor_msgs/TimeReference message).\n\
#\n\
# header.frame_id is the frame of reference reported by the satellite\n\
#        receiver, usually the location of the antenna.  This is a\n\
#        Euclidean frame relative to the vehicle, not a reference\n\
#        ellipsoid.\n\
Header header\n\
\n\
# satellite fix status information\n\
NavSatStatus status\n\
\n\
# Latitude [degrees]. Positive is north of equator; negative is south.\n\
float64 latitude\n\
\n\
# Longitude [degrees]. Positive is east of prime meridian; negative is west.\n\
float64 longitude\n\
\n\
# Altitude [m]. Positive is above the WGS 84 ellipsoid\n\
# (quiet NaN if no altitude is available).\n\
float64 altitude\n\
\n\
# Position covariance [m^2] defined relative to a tangential plane\n\
# through the reported position. The components are East, North, and\n\
# Up (ENU), in row-major order.\n\
#\n\
# Beware: this coordinate system exhibits singularities at the poles.\n\
\n\
float64[9] position_covariance\n\
\n\
# If the covariance of the fix is known, fill it in completely. If the\n\
# GPS receiver provides the variance of each measurement, put them\n\
# along the diagonal. If only Dilution of Precision is available,\n\
# estimate an approximate covariance from that.\n\
\n\
uint8 COVARIANCE_TYPE_UNKNOWN = 0\n\
uint8 COVARIANCE_TYPE_APPROXIMATED = 1\n\
uint8 COVARIANCE_TYPE_DIAGONAL_KNOWN = 2\n\
uint8 COVARIANCE_TYPE_KNOWN = 3\n\
\n\
uint8 position_covariance_type\n\
\n\
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
\n\
================================================================================\n\
MSG: sensor_msgs/NavSatStatus\n\
# Navigation Satellite fix status for any Global Navigation Satellite System\n\
\n\
# Whether to output an augmented fix is determined by both the fix\n\
# type and the last time differential corrections were received.  A\n\
# fix is valid when status >= STATUS_FIX.\n\
\n\
int8 STATUS_NO_FIX =  -1        # unable to fix position\n\
int8 STATUS_FIX =      0        # unaugmented fix\n\
int8 STATUS_SBAS_FIX = 1        # with satellite-based augmentation\n\
int8 STATUS_GBAS_FIX = 2        # with ground-based augmentation\n\
\n\
int8 status\n\
\n\
# Bits defining which Global Navigation Satellite System signals were\n\
# used by the receiver.\n\
\n\
uint16 SERVICE_GPS =     1\n\
uint16 SERVICE_GLONASS = 2\n\
uint16 SERVICE_COMPASS = 4      # includes BeiDou.\n\
uint16 SERVICE_GALILEO = 8\n\
\n\
uint16 service\n\
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

#endif // YARP_ROSMSG_sensor_msgs_NavSatFix_h
