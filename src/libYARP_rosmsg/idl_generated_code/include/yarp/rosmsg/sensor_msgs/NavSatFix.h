/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    yarp::os::NetFloat64 latitude;
    yarp::os::NetFloat64 longitude;
    yarp::os::NetFloat64 altitude;
    std::vector<yarp::os::NetFloat64> position_covariance;
    static const unsigned char COVARIANCE_TYPE_UNKNOWN = 0;
    static const unsigned char COVARIANCE_TYPE_APPROXIMATED = 1;
    static const unsigned char COVARIANCE_TYPE_DIAGONAL_KNOWN = 2;
    static const unsigned char COVARIANCE_TYPE_KNOWN = 3;
    unsigned char position_covariance_type;

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
        latitude = connection.expectDouble();

        // *** longitude ***
        longitude = connection.expectDouble();

        // *** altitude ***
        altitude = connection.expectDouble();

        // *** position_covariance ***
        int len = 9;
        position_covariance.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&position_covariance[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** position_covariance_type ***
        if (!connection.expectBlock((char*)&position_covariance_type, 1)) {
            return false;
        }

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
        latitude = reader.expectDouble();

        // *** longitude ***
        longitude = reader.expectDouble();

        // *** altitude ***
        altitude = reader.expectDouble();

        // *** position_covariance ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        position_covariance.resize(len);
        for (int i=0; i<len; i++) {
            position_covariance[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** position_covariance_type ***
        position_covariance_type = reader.expectInt();

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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** status ***
        if (!status.write(connection)) {
            return false;
        }

        // *** latitude ***
        connection.appendDouble(latitude);

        // *** longitude ***
        connection.appendDouble(longitude);

        // *** altitude ***
        connection.appendDouble(altitude);

        // *** position_covariance ***
        if (position_covariance.size()>0) {
            connection.appendExternalBlock((char*)&position_covariance[0], sizeof(yarp::os::NetFloat64)*position_covariance.size());
        }

        // *** position_covariance_type ***
        connection.appendBlock((char*)&position_covariance_type, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(11);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** status ***
        if (!status.write(connection)) {
            return false;
        }

        // *** latitude ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)latitude);

        // *** longitude ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)longitude);

        // *** altitude ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)altitude);

        // *** position_covariance ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(position_covariance.size());
        for (size_t i=0; i<position_covariance.size(); i++) {
            connection.appendDouble((double)position_covariance[i]);
        }

        // *** position_covariance_type ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)position_covariance_type);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::NavSatFix> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::NavSatFix> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
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
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: sensor_msgs/NavSatStatus\n\
") + yarp::rosmsg::sensor_msgs::NavSatStatus::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::NavSatFix::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/NavSatFix", "sensor_msgs/NavSatFix");
        typ.addProperty("md5sum", yarp::os::Value("122998cc9a34a5b4e4b83952e42242b4"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_NavSatFix_h
