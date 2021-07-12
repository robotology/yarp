/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "stereo_msgs/DisparityImage" msg definition:
//   # Separate header for compatibility with current TimeSynchronizer.
//   # Likely to be removed in a later release, use image.header instead.
//   Header header
//   
//   # Floating point disparity image. The disparities are pre-adjusted for any
//   # x-offset between the principal points of the two cameras (in the case
//   # that they are verged). That is: d = x_l - x_r - (cx_l - cx_r)
//   sensor_msgs/Image image
//   
//   # Stereo geometry. For disparity d, the depth from the camera is Z = fT/d.
//   float32 f # Focal length, pixels
//   float32 T # Baseline, world units
//   
//   # Subwindow of (potentially) valid disparity values.
//   sensor_msgs/RegionOfInterest valid_window
//   
//   # The range of disparities searched.
//   # In the disparity image, any disparity less than min_disparity is invalid.
//   # The disparity search range defines the horopter, or 3D volume that the
//   # stereo algorithm can "see". Points with Z outside of:
//   #     Z_min = fT / max_disparity
//   #     Z_max = fT / min_disparity
//   # could not be found.
//   float32 min_disparity
//   float32 max_disparity
//   
//   # Smallest allowed disparity increment. The smallest achievable depth range
//   # resolution is delta_Z = (Z^2/fT)*delta_d.
//   float32 delta_d
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_stereo_msgs_DisparityImage_h
#define YARP_ROSMSG_stereo_msgs_DisparityImage_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/sensor_msgs/Image.h>
#include <yarp/rosmsg/sensor_msgs/RegionOfInterest.h>

namespace yarp {
namespace rosmsg {
namespace stereo_msgs {

class DisparityImage : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::sensor_msgs::Image image;
    yarp::conf::float32_t f;
    yarp::conf::float32_t T;
    yarp::rosmsg::sensor_msgs::RegionOfInterest valid_window;
    yarp::conf::float32_t min_disparity;
    yarp::conf::float32_t max_disparity;
    yarp::conf::float32_t delta_d;

    DisparityImage() :
            header(),
            image(),
            f(0.0f),
            T(0.0f),
            valid_window(),
            min_disparity(0.0f),
            max_disparity(0.0f),
            delta_d(0.0f)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** image ***
        image.clear();

        // *** f ***
        f = 0.0f;

        // *** T ***
        T = 0.0f;

        // *** valid_window ***
        valid_window.clear();

        // *** min_disparity ***
        min_disparity = 0.0f;

        // *** max_disparity ***
        max_disparity = 0.0f;

        // *** delta_d ***
        delta_d = 0.0f;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** image ***
        if (!image.read(connection)) {
            return false;
        }

        // *** f ***
        f = connection.expectFloat32();

        // *** T ***
        T = connection.expectFloat32();

        // *** valid_window ***
        if (!valid_window.read(connection)) {
            return false;
        }

        // *** min_disparity ***
        min_disparity = connection.expectFloat32();

        // *** max_disparity ***
        max_disparity = connection.expectFloat32();

        // *** delta_d ***
        delta_d = connection.expectFloat32();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(8)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** image ***
        if (!image.read(connection)) {
            return false;
        }

        // *** f ***
        f = reader.expectFloat32();

        // *** T ***
        T = reader.expectFloat32();

        // *** valid_window ***
        if (!valid_window.read(connection)) {
            return false;
        }

        // *** min_disparity ***
        min_disparity = reader.expectFloat32();

        // *** max_disparity ***
        max_disparity = reader.expectFloat32();

        // *** delta_d ***
        delta_d = reader.expectFloat32();

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

        // *** image ***
        if (!image.write(connection)) {
            return false;
        }

        // *** f ***
        connection.appendFloat32(f);

        // *** T ***
        connection.appendFloat32(T);

        // *** valid_window ***
        if (!valid_window.write(connection)) {
            return false;
        }

        // *** min_disparity ***
        connection.appendFloat32(min_disparity);

        // *** max_disparity ***
        connection.appendFloat32(max_disparity);

        // *** delta_d ***
        connection.appendFloat32(delta_d);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(8);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** image ***
        if (!image.write(connection)) {
            return false;
        }

        // *** f ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(f);

        // *** T ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(T);

        // *** valid_window ***
        if (!valid_window.write(connection)) {
            return false;
        }

        // *** min_disparity ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(min_disparity);

        // *** max_disparity ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(max_disparity);

        // *** delta_d ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(delta_d);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::stereo_msgs::DisparityImage> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::stereo_msgs::DisparityImage> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "stereo_msgs/DisparityImage";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "04a177815f75271039fa21f16acad8c9";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Separate header for compatibility with current TimeSynchronizer.\n\
# Likely to be removed in a later release, use image.header instead.\n\
Header header\n\
\n\
# Floating point disparity image. The disparities are pre-adjusted for any\n\
# x-offset between the principal points of the two cameras (in the case\n\
# that they are verged). That is: d = x_l - x_r - (cx_l - cx_r)\n\
sensor_msgs/Image image\n\
\n\
# Stereo geometry. For disparity d, the depth from the camera is Z = fT/d.\n\
float32 f # Focal length, pixels\n\
float32 T # Baseline, world units\n\
\n\
# Subwindow of (potentially) valid disparity values.\n\
sensor_msgs/RegionOfInterest valid_window\n\
\n\
# The range of disparities searched.\n\
# In the disparity image, any disparity less than min_disparity is invalid.\n\
# The disparity search range defines the horopter, or 3D volume that the\n\
# stereo algorithm can \"see\". Points with Z outside of:\n\
#     Z_min = fT / max_disparity\n\
#     Z_max = fT / min_disparity\n\
# could not be found.\n\
float32 min_disparity\n\
float32 max_disparity\n\
\n\
# Smallest allowed disparity increment. The smallest achievable depth range\n\
# resolution is delta_Z = (Z^2/fT)*delta_d.\n\
float32 delta_d\n\
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
MSG: sensor_msgs/Image\n\
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
MSG: sensor_msgs/RegionOfInterest\n\
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

} // namespace stereo_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_stereo_msgs_DisparityImage_h
