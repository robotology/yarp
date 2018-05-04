/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    yarp::os::NetUint32 height;
    yarp::os::NetUint32 width;
    std::string encoding;
    unsigned char is_bigendian;
    yarp::os::NetUint32 step;
    std::vector<unsigned char> data;

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
        height = connection.expectInt();

        // *** width ***
        width = connection.expectInt();

        // *** encoding ***
        int len = connection.expectInt();
        encoding.resize(len);
        if (!connection.expectBlock((char*)encoding.c_str(), len)) {
            return false;
        }

        // *** is_bigendian ***
        if (!connection.expectBlock((char*)&is_bigendian, 1)) {
            return false;
        }

        // *** step ***
        step = connection.expectInt();

        // *** data ***
        len = connection.expectInt();
        data.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&data[0], sizeof(unsigned char)*len)) {
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
        height = reader.expectInt();

        // *** width ***
        width = reader.expectInt();

        // *** encoding ***
        if (!reader.readString(encoding)) {
            return false;
        }

        // *** is_bigendian ***
        is_bigendian = reader.expectInt();

        // *** step ***
        step = reader.expectInt();

        // *** data ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) {
            return false;
        }
        int len = connection.expectInt();
        data.resize(len);
        for (int i=0; i<len; i++) {
            data[i] = (unsigned char)connection.expectInt();
        }

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

        // *** encoding ***
        connection.appendInt(encoding.length());
        connection.appendExternalBlock((char*)encoding.c_str(), encoding.length());

        // *** is_bigendian ***
        connection.appendBlock((char*)&is_bigendian, 1);

        // *** step ***
        connection.appendInt(step);

        // *** data ***
        connection.appendInt(data.size());
        if (data.size()>0) {
            connection.appendExternalBlock((char*)&data[0], sizeof(unsigned char)*data.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(7);

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

        // *** encoding ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(encoding.length());
        connection.appendExternalBlock((char*)encoding.c_str(), encoding.length());

        // *** is_bigendian ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)is_bigendian);

        // *** step ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)step);

        // *** data ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_INT);
        connection.appendInt(data.size());
        for (size_t i=0; i<data.size(); i++) {
            connection.appendInt((int)data[i]);
        }

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::Image> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::Image> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
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
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::Image::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/Image", "sensor_msgs/Image");
        typ.addProperty("md5sum", yarp::os::Value("060021388200f6f0f447d0fcd9c64743"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_Image_h
