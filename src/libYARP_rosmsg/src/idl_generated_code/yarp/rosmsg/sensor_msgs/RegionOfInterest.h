/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/RegionOfInterest" msg definition:
//   # This message is used to specify a region of interest within an image.
//   #
//   # When used to specify the ROI setting of the camera when the image was
//   # taken, the height and width fields should either match the height and
//   # width fields for the associated image; or height = width = 0
//   # indicates that the full resolution image was captured.
//   
//   uint32 x_offset  # Leftmost pixel of the ROI
//                    # (0 if the ROI includes the left edge of the image)
//   uint32 y_offset  # Topmost pixel of the ROI
//                    # (0 if the ROI includes the top edge of the image)
//   uint32 height    # Height of ROI
//   uint32 width     # Width of ROI
//   
//   # True if a distinct rectified ROI should be calculated from the "raw"
//   # ROI in this message. Typically this should be False if the full image
//   # is captured (ROI not used), and True if a subwindow is captured (ROI
//   # used).
//   bool do_rectify
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_RegionOfInterest_h
#define YARP_ROSMSG_sensor_msgs_RegionOfInterest_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class RegionOfInterest : public yarp::os::idl::WirePortable
{
public:
    std::uint32_t x_offset;
    std::uint32_t y_offset;
    std::uint32_t height;
    std::uint32_t width;
    bool do_rectify;

    RegionOfInterest() :
            x_offset(0),
            y_offset(0),
            height(0),
            width(0),
            do_rectify(false)
    {
    }

    void clear()
    {
        // *** x_offset ***
        x_offset = 0;

        // *** y_offset ***
        y_offset = 0;

        // *** height ***
        height = 0;

        // *** width ***
        width = 0;

        // *** do_rectify ***
        do_rectify = false;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** x_offset ***
        x_offset = connection.expectInt32();

        // *** y_offset ***
        y_offset = connection.expectInt32();

        // *** height ***
        height = connection.expectInt32();

        // *** width ***
        width = connection.expectInt32();

        // *** do_rectify ***
        if (!connection.expectBlock((char*)&do_rectify, 1)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(5)) {
            return false;
        }

        // *** x_offset ***
        x_offset = reader.expectInt32();

        // *** y_offset ***
        y_offset = reader.expectInt32();

        // *** height ***
        height = reader.expectInt32();

        // *** width ***
        width = reader.expectInt32();

        // *** do_rectify ***
        do_rectify = reader.expectInt8();

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
        // *** x_offset ***
        connection.appendInt32(x_offset);

        // *** y_offset ***
        connection.appendInt32(y_offset);

        // *** height ***
        connection.appendInt32(height);

        // *** width ***
        connection.appendInt32(width);

        // *** do_rectify ***
        connection.appendBlock((char*)&do_rectify, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(5);

        // *** x_offset ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(x_offset);

        // *** y_offset ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(y_offset);

        // *** height ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(height);

        // *** width ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(width);

        // *** do_rectify ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(do_rectify);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::RegionOfInterest> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::RegionOfInterest> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/RegionOfInterest";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "bdb633039d588fcccb441a4d43ccfe09";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This message is used to specify a region of interest within an image.\n\
#\n\
# When used to specify the ROI setting of the camera when the image was\n\
# taken, the height and width fields should either match the height and\n\
# width fields for the associated image; or height = width = 0\n\
# indicates that the full resolution image was captured.\n\
\n\
uint32 x_offset  # Leftmost pixel of the ROI\n\
                 # (0 if the ROI includes the left edge of the image)\n\
uint32 y_offset  # Topmost pixel of the ROI\n\
                 # (0 if the ROI includes the top edge of the image)\n\
uint32 height    # Height of ROI\n\
uint32 width     # Width of ROI\n\
\n\
# True if a distinct rectified ROI should be calculated from the \"raw\"\n\
# ROI in this message. Typically this should be False if the full image\n\
# is captured (ROI not used), and True if a subwindow is captured (ROI\n\
# used).\n\
bool do_rectify\n\
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

#endif // YARP_ROSMSG_sensor_msgs_RegionOfInterest_h
