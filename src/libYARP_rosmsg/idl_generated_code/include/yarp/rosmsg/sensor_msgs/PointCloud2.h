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
    yarp::os::NetUint32 height;
    yarp::os::NetUint32 width;
    std::vector<yarp::rosmsg::sensor_msgs::PointField> fields;
    bool is_bigendian;
    yarp::os::NetUint32 point_step;
    yarp::os::NetUint32 row_step;
    std::vector<unsigned char> data;
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
        height = connection.expectInt();

        // *** width ***
        width = connection.expectInt();

        // *** fields ***
        int len = connection.expectInt();
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
        point_step = connection.expectInt();

        // *** row_step ***
        row_step = connection.expectInt();

        // *** data ***
        len = connection.expectInt();
        data.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&data[0], sizeof(unsigned char)*len)) {
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
        height = reader.expectInt();

        // *** width ***
        width = reader.expectInt();

        // *** fields ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        fields.resize(len);
        for (int i=0; i<len; i++) {
            if (!fields[i].read(connection)) {
                return false;
            }
        }

        // *** is_bigendian ***
        is_bigendian = reader.expectInt();

        // *** point_step ***
        point_step = reader.expectInt();

        // *** row_step ***
        row_step = reader.expectInt();

        // *** data ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) {
            return false;
        }
        len = connection.expectInt();
        data.resize(len);
        for (int i=0; i<len; i++) {
            data[i] = (unsigned char)connection.expectInt();
        }

        // *** is_dense ***
        is_dense = reader.expectInt();

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

        // *** height ***
        connection.appendInt(height);

        // *** width ***
        connection.appendInt(width);

        // *** fields ***
        connection.appendInt(fields.size());
        for (size_t i=0; i<fields.size(); i++) {
            if (!fields[i].write(connection)) {
                return false;
            }
        }

        // *** is_bigendian ***
        connection.appendBlock((char*)&is_bigendian, 1);

        // *** point_step ***
        connection.appendInt(point_step);

        // *** row_step ***
        connection.appendInt(row_step);

        // *** data ***
        connection.appendInt(data.size());
        if (data.size()>0) {
            connection.appendExternalBlock((char*)&data[0], sizeof(unsigned char)*data.size());
        }

        // *** is_dense ***
        connection.appendBlock((char*)&is_dense, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(9);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** height ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)height);

        // *** width ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)width);

        // *** fields ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(fields.size());
        for (size_t i=0; i<fields.size(); i++) {
            if (!fields[i].write(connection)) {
                return false;
            }
        }

        // *** is_bigendian ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)is_bigendian);

        // *** point_step ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)point_step);

        // *** row_step ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)row_step);

        // *** data ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_INT);
        connection.appendInt(data.size());
        for (size_t i=0; i<data.size(); i++) {
            connection.appendInt((int)data[i]);
        }

        // *** is_dense ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)is_dense);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::PointCloud2> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::PointCloud2> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This message holds a collection of N-dimensional points, which may\n\
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
bool is_dense        # True if there are no invalid points\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: sensor_msgs/PointField\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/PointCloud2", "sensor_msgs/PointCloud2");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_PointCloud2_h
