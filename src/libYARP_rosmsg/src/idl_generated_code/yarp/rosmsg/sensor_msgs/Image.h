/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/Image" msg definition:
//   # This message contains an uncompressed image
//   # (0, 0) is at top-left corner of image
//   #
//   
//   Header header        # Header timestamp should be acquisition time of image
//                        # Header frame_id should be optical frame of camera
//                        # origin of frame should be optical center of cameara
//                        # +x should point to the right in the image
//                        # +y should point down in the image
//                        # +z should point into to plane of the image
//                        # If the frame_id here and the frame_id of the CameraInfo
//                        # message associated with the image conflict
//                        # the behavior is undefined
//   
//   uint32 height         # image height, that is, number of rows
//   uint32 width          # image width, that is, number of columns
//   
//   # The legal values for encoding are in file src/image_encodings.cpp
//   # If you want to standardize a new string format, join
//   # ros-users@lists.sourceforge.net and send an email proposing a new encoding.
//   
//   string encoding       # Encoding of pixels -- channel meaning, ordering, size
//                         # taken from the list of strings in include/sensor_msgs/image_encodings.h
//   
//   uint8 is_bigendian    # is this data bigendian?
//   uint32 step           # Full row length in bytes
//   uint8[] data          # actual matrix data, size is (step * rows)
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_Image_h
#define YARP_ROSMSG_sensor_msgs_Image_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class Image : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::uint32_t height;
    std::uint32_t width;
    std::string encoding;
    std::uint8_t is_bigendian;
    std::uint32_t step;
    std::vector<std::uint8_t> data;

    Image() :
            header(),
            height(0),
            width(0),
            encoding(""),
            is_bigendian(0),
            step(0),
            data()
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

        // *** encoding ***
        encoding = "";

        // *** is_bigendian ***
        is_bigendian = 0;

        // *** step ***
        step = 0;

        // *** data ***
        data.clear();
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

        // *** encoding ***
        int len = connection.expectInt32();
        encoding.resize(len);
        if (!connection.expectBlock((char*)encoding.c_str(), len)) {
            return false;
        }

        // *** is_bigendian ***
        is_bigendian = connection.expectInt8();

        // *** step ***
        step = connection.expectInt32();

        // *** data ***
        len = connection.expectInt32();
        data.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&data[0], sizeof(std::uint8_t)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(7)) {
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

        // *** encoding ***
        if (!reader.readString(encoding)) {
            return false;
        }

        // *** is_bigendian ***
        is_bigendian = reader.expectInt8();

        // *** step ***
        step = reader.expectInt32();

        // *** data ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT8)) {
            return false;
        }
        int len = connection.expectInt32();
        data.resize(len);
        for (int i=0; i<len; i++) {
            data[i] = (std::uint8_t)connection.expectInt8();
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

        // *** height ***
        connection.appendInt32(height);

        // *** width ***
        connection.appendInt32(width);

        // *** encoding ***
        connection.appendInt32(encoding.length());
        connection.appendExternalBlock((char*)encoding.c_str(), encoding.length());

        // *** is_bigendian ***
        connection.appendInt8(is_bigendian);

        // *** step ***
        connection.appendInt32(step);

        // *** data ***
        connection.appendInt32(data.size());
        if (data.size()>0) {
            connection.appendExternalBlock((char*)&data[0], sizeof(std::uint8_t)*data.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(7);

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

        // *** encoding ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(encoding.length());
        connection.appendExternalBlock((char*)encoding.c_str(), encoding.length());

        // *** is_bigendian ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(is_bigendian);

        // *** step ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(step);

        // *** data ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_INT8);
        connection.appendInt32(data.size());
        for (size_t i=0; i<data.size(); i++) {
            connection.appendInt8(data[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::Image> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::Image> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/Image";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "060021388200f6f0f447d0fcd9c64743";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This message contains an uncompressed image\n\
# (0, 0) is at top-left corner of image\n\
#\n\
\n\
Header header        # Header timestamp should be acquisition time of image\n\
                     # Header frame_id should be optical frame of camera\n\
                     # origin of frame should be optical center of cameara\n\
                     # +x should point to the right in the image\n\
                     # +y should point down in the image\n\
                     # +z should point into to plane of the image\n\
                     # If the frame_id here and the frame_id of the CameraInfo\n\
                     # message associated with the image conflict\n\
                     # the behavior is undefined\n\
\n\
uint32 height         # image height, that is, number of rows\n\
uint32 width          # image width, that is, number of columns\n\
\n\
# The legal values for encoding are in file src/image_encodings.cpp\n\
# If you want to standardize a new string format, join\n\
# ros-users@lists.sourceforge.net and send an email proposing a new encoding.\n\
\n\
string encoding       # Encoding of pixels -- channel meaning, ordering, size\n\
                      # taken from the list of strings in include/sensor_msgs/image_encodings.h\n\
\n\
uint8 is_bigendian    # is this data bigendian?\n\
uint32 step           # Full row length in bytes\n\
uint8[] data          # actual matrix data, size is (step * rows)\n\
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

#endif // YARP_ROSMSG_sensor_msgs_Image_h
