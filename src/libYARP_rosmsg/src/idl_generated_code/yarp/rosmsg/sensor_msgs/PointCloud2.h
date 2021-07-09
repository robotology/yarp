/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/PointCloud2" msg definition:
//   # This message holds a collection of N-dimensional points, which may
//   # contain additional information such as normals, intensity, etc. The
//   # point data is stored as a binary blob, its layout described by the
//   # contents of the "fields" array.
//   
//   # The point cloud data may be organized 2d (image-like) or 1d
//   # (unordered). Point clouds organized as 2d images may be produced by
//   # camera depth sensors such as stereo or time-of-flight.
//   
//   # Time of sensor data acquisition, and the coordinate frame ID (for 3d
//   # points).
//   Header header
//   
//   # 2D structure of the point cloud. If the cloud is unordered, height is
//   # 1 and width is the length of the point cloud.
//   uint32 height
//   uint32 width
//   
//   # Describes the channels and their layout in the binary data blob.
//   PointField[] fields
//   
//   bool    is_bigendian # Is this data bigendian?
//   uint32  point_step   # Length of a point in bytes
//   uint32  row_step     # Length of a row in bytes
//   uint8[] data         # Actual point data, size is (row_step*height)
//   
//   bool is_dense        # True if there are no invalid points
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_PointCloud2_h
#define YARP_ROSMSG_sensor_msgs_PointCloud2_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/sensor_msgs/PointField.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class PointCloud2 : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::uint32_t height;
    std::uint32_t width;
    std::vector<yarp::rosmsg::sensor_msgs::PointField> fields;
    bool is_bigendian;
    std::uint32_t point_step;
    std::uint32_t row_step;
    std::vector<std::uint8_t> data;
    bool is_dense;

    PointCloud2() :
            header(),
            height(0),
            width(0),
            fields(),
            is_bigendian(false),
            point_step(0),
            row_step(0),
            data(),
            is_dense(false)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** height ***
        height = 0;

        // *** width ***
        width = 0;

        // *** fields ***
        fields.clear();

        // *** is_bigendian ***
        is_bigendian = false;

        // *** point_step ***
        point_step = 0;

        // *** row_step ***
        row_step = 0;

        // *** data ***
        data.clear();

        // *** is_dense ***
        is_dense = false;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** height ***
        height = connection.expectInt32();

        // *** width ***
        width = connection.expectInt32();

        // *** fields ***
        int len = connection.expectInt32();
        fields.resize(len);
        for (int i=0; i<len; i++) {
            if (!fields[i].read(connection)) {
                return false;
            }
        }

        // *** is_bigendian ***
        if (!connection.expectBlock((char*)&is_bigendian, 1)) {
            return false;
        }

        // *** point_step ***
        point_step = connection.expectInt32();

        // *** row_step ***
        row_step = connection.expectInt32();

        // *** data ***
        len = connection.expectInt32();
        data.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&data[0], sizeof(std::uint8_t)*len)) {
            return false;
        }

        // *** is_dense ***
        if (!connection.expectBlock((char*)&is_dense, 1)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(9)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** height ***
        height = reader.expectInt32();

        // *** width ***
        width = reader.expectInt32();

        // *** fields ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        fields.resize(len);
        for (int i=0; i<len; i++) {
            if (!fields[i].read(connection)) {
                return false;
            }
        }

        // *** is_bigendian ***
        is_bigendian = reader.expectInt8();

        // *** point_step ***
        point_step = reader.expectInt32();

        // *** row_step ***
        row_step = reader.expectInt32();

        // *** data ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT8)) {
            return false;
        }
        len = connection.expectInt32();
        data.resize(len);
        for (int i=0; i<len; i++) {
            data[i] = (std::uint8_t)connection.expectInt8();
        }

        // *** is_dense ***
        is_dense = reader.expectInt8();

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

        // *** height ***
        connection.appendInt32(height);

        // *** width ***
        connection.appendInt32(width);

        // *** fields ***
        connection.appendInt32(fields.size());
        for (size_t i=0; i<fields.size(); i++) {
            if (!fields[i].write(connection)) {
                return false;
            }
        }

        // *** is_bigendian ***
        connection.appendBlock((char*)&is_bigendian, 1);

        // *** point_step ***
        connection.appendInt32(point_step);

        // *** row_step ***
        connection.appendInt32(row_step);

        // *** data ***
        connection.appendInt32(data.size());
        if (data.size()>0) {
            connection.appendExternalBlock((char*)&data[0], sizeof(std::uint8_t)*data.size());
        }

        // *** is_dense ***
        connection.appendBlock((char*)&is_dense, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(9);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** height ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(height);

        // *** width ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(width);

        // *** fields ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(fields.size());
        for (size_t i=0; i<fields.size(); i++) {
            if (!fields[i].write(connection)) {
                return false;
            }
        }

        // *** is_bigendian ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(is_bigendian);

        // *** point_step ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(point_step);

        // *** row_step ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(row_step);

        // *** data ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_INT8);
        connection.appendInt32(data.size());
        for (size_t i=0; i<data.size(); i++) {
            connection.appendInt8(data[i]);
        }

        // *** is_dense ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(is_dense);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::PointCloud2> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::PointCloud2> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/PointCloud2";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "1158d486dd51d683ce2f1be655c3c181";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This message holds a collection of N-dimensional points, which may\n\
# contain additional information such as normals, intensity, etc. The\n\
# point data is stored as a binary blob, its layout described by the\n\
# contents of the \"fields\" array.\n\
\n\
# The point cloud data may be organized 2d (image-like) or 1d\n\
# (unordered). Point clouds organized as 2d images may be produced by\n\
# camera depth sensors such as stereo or time-of-flight.\n\
\n\
# Time of sensor data acquisition, and the coordinate frame ID (for 3d\n\
# points).\n\
Header header\n\
\n\
# 2D structure of the point cloud. If the cloud is unordered, height is\n\
# 1 and width is the length of the point cloud.\n\
uint32 height\n\
uint32 width\n\
\n\
# Describes the channels and their layout in the binary data blob.\n\
PointField[] fields\n\
\n\
bool    is_bigendian # Is this data bigendian?\n\
uint32  point_step   # Length of a point in bytes\n\
uint32  row_step     # Length of a row in bytes\n\
uint8[] data         # Actual point data, size is (row_step*height)\n\
\n\
bool is_dense        # True if there are no invalid points\n\
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
MSG: sensor_msgs/PointField\n\
# This message holds the description of one point entry in the\n\
# PointCloud2 message format.\n\
uint8 INT8    = 1\n\
uint8 UINT8   = 2\n\
uint8 INT16   = 3\n\
uint8 UINT16  = 4\n\
uint8 INT32   = 5\n\
uint8 UINT32  = 6\n\
uint8 FLOAT32 = 7\n\
uint8 FLOAT64 = 8\n\
\n\
string name      # Name of field\n\
uint32 offset    # Offset from start of point struct\n\
uint8  datatype  # Datatype enumeration, see above\n\
uint32 count     # How many elements in the field\n\
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

#endif // YARP_ROSMSG_sensor_msgs_PointCloud2_h
