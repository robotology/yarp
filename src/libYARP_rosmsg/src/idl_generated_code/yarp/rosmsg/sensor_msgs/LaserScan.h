/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/LaserScan" msg definition:
//   # Single scan from a planar laser range-finder
//   #
//   # If you have another ranging device with different behavior (e.g. a sonar
//   # array), please find or create a different message, since applications
//   # will make fairly laser-specific assumptions about this data
//   
//   Header header            # timestamp in the header is the acquisition time of 
//                            # the first ray in the scan.
//                            #
//                            # in frame frame_id, angles are measured around 
//                            # the positive Z axis (counterclockwise, if Z is up)
//                            # with zero angle being forward along the x axis
//                            
//   float32 angle_min        # start angle of the scan [rad]
//   float32 angle_max        # end angle of the scan [rad]
//   float32 angle_increment  # angular distance between measurements [rad]
//   
//   float32 time_increment   # time between measurements [seconds] - if your scanner
//                            # is moving, this will be used in interpolating position
//                            # of 3d points
//   float32 scan_time        # time between scans [seconds]
//   
//   float32 range_min        # minimum range value [m]
//   float32 range_max        # maximum range value [m]
//   
//   float32[] ranges         # range data [m] (Note: values < range_min or > range_max should be discarded)
//   float32[] intensities    # intensity data [device-specific units].  If your
//                            # device does not provide intensities, please leave
//                            # the array empty.
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_LaserScan_h
#define YARP_ROSMSG_sensor_msgs_LaserScan_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class LaserScan : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::conf::float32_t angle_min;
    yarp::conf::float32_t angle_max;
    yarp::conf::float32_t angle_increment;
    yarp::conf::float32_t time_increment;
    yarp::conf::float32_t scan_time;
    yarp::conf::float32_t range_min;
    yarp::conf::float32_t range_max;
    std::vector<yarp::conf::float32_t> ranges;
    std::vector<yarp::conf::float32_t> intensities;

    LaserScan() :
            header(),
            angle_min(0.0f),
            angle_max(0.0f),
            angle_increment(0.0f),
            time_increment(0.0f),
            scan_time(0.0f),
            range_min(0.0f),
            range_max(0.0f),
            ranges(),
            intensities()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** angle_min ***
        angle_min = 0.0f;

        // *** angle_max ***
        angle_max = 0.0f;

        // *** angle_increment ***
        angle_increment = 0.0f;

        // *** time_increment ***
        time_increment = 0.0f;

        // *** scan_time ***
        scan_time = 0.0f;

        // *** range_min ***
        range_min = 0.0f;

        // *** range_max ***
        range_max = 0.0f;

        // *** ranges ***
        ranges.clear();

        // *** intensities ***
        intensities.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** angle_min ***
        angle_min = connection.expectFloat32();

        // *** angle_max ***
        angle_max = connection.expectFloat32();

        // *** angle_increment ***
        angle_increment = connection.expectFloat32();

        // *** time_increment ***
        time_increment = connection.expectFloat32();

        // *** scan_time ***
        scan_time = connection.expectFloat32();

        // *** range_min ***
        range_min = connection.expectFloat32();

        // *** range_max ***
        range_max = connection.expectFloat32();

        // *** ranges ***
        int len = connection.expectInt32();
        ranges.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&ranges[0], sizeof(yarp::conf::float32_t)*len)) {
            return false;
        }

        // *** intensities ***
        len = connection.expectInt32();
        intensities.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&intensities[0], sizeof(yarp::conf::float32_t)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(10)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** angle_min ***
        angle_min = reader.expectFloat32();

        // *** angle_max ***
        angle_max = reader.expectFloat32();

        // *** angle_increment ***
        angle_increment = reader.expectFloat32();

        // *** time_increment ***
        time_increment = reader.expectFloat32();

        // *** scan_time ***
        scan_time = reader.expectFloat32();

        // *** range_min ***
        range_min = reader.expectFloat32();

        // *** range_max ***
        range_max = reader.expectFloat32();

        // *** ranges ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32)) {
            return false;
        }
        int len = connection.expectInt32();
        ranges.resize(len);
        for (int i=0; i<len; i++) {
            ranges[i] = (yarp::conf::float32_t)connection.expectFloat32();
        }

        // *** intensities ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32)) {
            return false;
        }
        len = connection.expectInt32();
        intensities.resize(len);
        for (int i=0; i<len; i++) {
            intensities[i] = (yarp::conf::float32_t)connection.expectFloat32();
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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** angle_min ***
        connection.appendFloat32(angle_min);

        // *** angle_max ***
        connection.appendFloat32(angle_max);

        // *** angle_increment ***
        connection.appendFloat32(angle_increment);

        // *** time_increment ***
        connection.appendFloat32(time_increment);

        // *** scan_time ***
        connection.appendFloat32(scan_time);

        // *** range_min ***
        connection.appendFloat32(range_min);

        // *** range_max ***
        connection.appendFloat32(range_max);

        // *** ranges ***
        connection.appendInt32(ranges.size());
        if (ranges.size()>0) {
            connection.appendExternalBlock((char*)&ranges[0], sizeof(yarp::conf::float32_t)*ranges.size());
        }

        // *** intensities ***
        connection.appendInt32(intensities.size());
        if (intensities.size()>0) {
            connection.appendExternalBlock((char*)&intensities[0], sizeof(yarp::conf::float32_t)*intensities.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(10);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** angle_min ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(angle_min);

        // *** angle_max ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(angle_max);

        // *** angle_increment ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(angle_increment);

        // *** time_increment ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(time_increment);

        // *** scan_time ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(scan_time);

        // *** range_min ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(range_min);

        // *** range_max ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(range_max);

        // *** ranges ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32);
        connection.appendInt32(ranges.size());
        for (size_t i=0; i<ranges.size(); i++) {
            connection.appendFloat32(ranges[i]);
        }

        // *** intensities ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32);
        connection.appendInt32(intensities.size());
        for (size_t i=0; i<intensities.size(); i++) {
            connection.appendFloat32(intensities[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::LaserScan> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::LaserScan> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/LaserScan";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "90c7ef2dc6895d81024acba2ac42f369";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Single scan from a planar laser range-finder\n\
#\n\
# If you have another ranging device with different behavior (e.g. a sonar\n\
# array), please find or create a different message, since applications\n\
# will make fairly laser-specific assumptions about this data\n\
\n\
Header header            # timestamp in the header is the acquisition time of \n\
                         # the first ray in the scan.\n\
                         #\n\
                         # in frame frame_id, angles are measured around \n\
                         # the positive Z axis (counterclockwise, if Z is up)\n\
                         # with zero angle being forward along the x axis\n\
                         \n\
float32 angle_min        # start angle of the scan [rad]\n\
float32 angle_max        # end angle of the scan [rad]\n\
float32 angle_increment  # angular distance between measurements [rad]\n\
\n\
float32 time_increment   # time between measurements [seconds] - if your scanner\n\
                         # is moving, this will be used in interpolating position\n\
                         # of 3d points\n\
float32 scan_time        # time between scans [seconds]\n\
\n\
float32 range_min        # minimum range value [m]\n\
float32 range_max        # maximum range value [m]\n\
\n\
float32[] ranges         # range data [m] (Note: values < range_min or > range_max should be discarded)\n\
float32[] intensities    # intensity data [device-specific units].  If your\n\
                         # device does not provide intensities, please leave\n\
                         # the array empty.\n\
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

#endif // YARP_ROSMSG_sensor_msgs_LaserScan_h
