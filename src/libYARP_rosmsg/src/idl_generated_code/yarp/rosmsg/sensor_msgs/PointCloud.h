/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/PointCloud" msg definition:
//   # This message holds a collection of 3d points, plus optional additional
//   # information about each point.
//   
//   # Time of sensor data acquisition, coordinate frame ID.
//   Header header
//   
//   # Array of 3d points. Each Point32 should be interpreted as a 3d point
//   # in the frame given in the header.
//   geometry_msgs/Point32[] points
//   
//   # Each channel should have the same number of elements as points array,
//   # and the data in each channel should correspond 1:1 with each point.
//   # Channel names in common practice are listed in ChannelFloat32.msg.
//   ChannelFloat32[] channels
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_PointCloud_h
#define YARP_ROSMSG_sensor_msgs_PointCloud_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Point32.h>
#include <yarp/rosmsg/sensor_msgs/ChannelFloat32.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class PointCloud : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<yarp::rosmsg::geometry_msgs::Point32> points;
    std::vector<yarp::rosmsg::sensor_msgs::ChannelFloat32> channels;

    PointCloud() :
            header(),
            points(),
            channels()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** points ***
        points.clear();

        // *** channels ***
        channels.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** points ***
        int len = connection.expectInt32();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        // *** channels ***
        len = connection.expectInt32();
        channels.resize(len);
        for (int i=0; i<len; i++) {
            if (!channels[i].read(connection)) {
                return false;
            }
        }

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

        // *** points ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        // *** channels ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        channels.resize(len);
        for (int i=0; i<len; i++) {
            if (!channels[i].read(connection)) {
                return false;
            }
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

        // *** points ***
        connection.appendInt32(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** channels ***
        connection.appendInt32(channels.size());
        for (size_t i=0; i<channels.size(); i++) {
            if (!channels[i].write(connection)) {
                return false;
            }
        }

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

        // *** points ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** channels ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(channels.size());
        for (size_t i=0; i<channels.size(); i++) {
            if (!channels[i].write(connection)) {
                return false;
            }
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::PointCloud> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::PointCloud> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/PointCloud";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "d8e9c3f5afbdd8a130fd1d2763945fca";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This message holds a collection of 3d points, plus optional additional\n\
# information about each point.\n\
\n\
# Time of sensor data acquisition, coordinate frame ID.\n\
Header header\n\
\n\
# Array of 3d points. Each Point32 should be interpreted as a 3d point\n\
# in the frame given in the header.\n\
geometry_msgs/Point32[] points\n\
\n\
# Each channel should have the same number of elements as points array,\n\
# and the data in each channel should correspond 1:1 with each point.\n\
# Channel names in common practice are listed in ChannelFloat32.msg.\n\
ChannelFloat32[] channels\n\
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
MSG: geometry_msgs/Point32\n\
# This contains the position of a point in free space(with 32 bits of precision).\n\
# It is recommeded to use Point wherever possible instead of Point32.  \n\
# \n\
# This recommendation is to promote interoperability.  \n\
#\n\
# This message is designed to take up less space when sending\n\
# lots of points at once, as in the case of a PointCloud.  \n\
\n\
float32 x\n\
float32 y\n\
float32 z\n\
================================================================================\n\
MSG: sensor_msgs/ChannelFloat32\n\
# This message is used by the PointCloud message to hold optional data\n\
# associated with each point in the cloud. The length of the values\n\
# array should be the same as the length of the points array in the\n\
# PointCloud, and each value should be associated with the corresponding\n\
# point.\n\
\n\
# Channel names in existing practice include:\n\
#   \"u\", \"v\" - row and column (respectively) in the left stereo image.\n\
#              This is opposite to usual conventions but remains for\n\
#              historical reasons. The newer PointCloud2 message has no\n\
#              such problem.\n\
#   \"rgb\" - For point clouds produced by color stereo cameras. uint8\n\
#           (R,G,B) values packed into the least significant 24 bits,\n\
#           in order.\n\
#   \"intensity\" - laser or pixel intensity.\n\
#   \"distance\"\n\
\n\
# The channel name should give semantics of the channel (e.g.\n\
# \"intensity\" instead of \"value\").\n\
string name\n\
\n\
# The values array should be 1-1 with the elements of the associated\n\
# PointCloud.\n\
float32[] values\n\
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

#endif // YARP_ROSMSG_sensor_msgs_PointCloud_h
