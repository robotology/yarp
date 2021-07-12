/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/CompressedImage" msg definition:
//   # This message contains a compressed image
//   
//   Header header        # Header timestamp should be acquisition time of image
//                        # Header frame_id should be optical frame of camera
//                        # origin of frame should be optical center of cameara
//                        # +x should point to the right in the image
//                        # +y should point down in the image
//                        # +z should point into to plane of the image
//   
//   string format        # Specifies the format of the data
//                        #   Acceptable values:
//                        #     jpeg, png
//   uint8[] data         # Compressed image buffer
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_CompressedImage_h
#define YARP_ROSMSG_sensor_msgs_CompressedImage_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class CompressedImage : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::string format;
    std::vector<std::uint8_t> data;

    CompressedImage() :
            header(),
            format(""),
            data()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** format ***
        format = "";

        // *** data ***
        data.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** format ***
        int len = connection.expectInt32();
        format.resize(len);
        if (!connection.expectBlock((char*)format.c_str(), len)) {
            return false;
        }

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
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** format ***
        if (!reader.readString(format)) {
            return false;
        }

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

        // *** format ***
        connection.appendInt32(format.length());
        connection.appendExternalBlock((char*)format.c_str(), format.length());

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
        connection.appendInt32(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** format ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(format.length());
        connection.appendExternalBlock((char*)format.c_str(), format.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::CompressedImage> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::CompressedImage> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/CompressedImage";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "8f7a12909da2c9d3332d540a0977563f";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This message contains a compressed image\n\
\n\
Header header        # Header timestamp should be acquisition time of image\n\
                     # Header frame_id should be optical frame of camera\n\
                     # origin of frame should be optical center of cameara\n\
                     # +x should point to the right in the image\n\
                     # +y should point down in the image\n\
                     # +z should point into to plane of the image\n\
\n\
string format        # Specifies the format of the data\n\
                     #   Acceptable values:\n\
                     #     jpeg, png\n\
uint8[] data         # Compressed image buffer\n\
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

#endif // YARP_ROSMSG_sensor_msgs_CompressedImage_h
