/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    yarp::os::NetFloat32 f;
    yarp::os::NetFloat32 T;
    yarp::rosmsg::sensor_msgs::RegionOfInterest valid_window;
    yarp::os::NetFloat32 min_disparity;
    yarp::os::NetFloat32 max_disparity;
    yarp::os::NetFloat32 delta_d;

    DisparityImage() :
            header(),
            image(),
            f(0.0),
            T(0.0),
            valid_window(),
            min_disparity(0.0),
            max_disparity(0.0),
            delta_d(0.0)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** image ***
        image.clear();

        // *** f ***
        f = 0.0;

        // *** T ***
        T = 0.0;

        // *** valid_window ***
        valid_window.clear();

        // *** min_disparity ***
        min_disparity = 0.0;

        // *** max_disparity ***
        max_disparity = 0.0;

        // *** delta_d ***
        delta_d = 0.0;
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
        if (!connection.expectBlock((char*)&f, 4)) {
            return false;
        }

        // *** T ***
        if (!connection.expectBlock((char*)&T, 4)) {
            return false;
        }

        // *** valid_window ***
        if (!valid_window.read(connection)) {
            return false;
        }

        // *** min_disparity ***
        if (!connection.expectBlock((char*)&min_disparity, 4)) {
            return false;
        }

        // *** max_disparity ***
        if (!connection.expectBlock((char*)&max_disparity, 4)) {
            return false;
        }

        // *** delta_d ***
        if (!connection.expectBlock((char*)&delta_d, 4)) {
            return false;
        }

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
        f = reader.expectDouble();

        // *** T ***
        T = reader.expectDouble();

        // *** valid_window ***
        if (!valid_window.read(connection)) {
            return false;
        }

        // *** min_disparity ***
        min_disparity = reader.expectDouble();

        // *** max_disparity ***
        max_disparity = reader.expectDouble();

        // *** delta_d ***
        delta_d = reader.expectDouble();

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

        // *** image ***
        if (!image.write(connection)) {
            return false;
        }

        // *** f ***
        connection.appendBlock((char*)&f, 4);

        // *** T ***
        connection.appendBlock((char*)&T, 4);

        // *** valid_window ***
        if (!valid_window.write(connection)) {
            return false;
        }

        // *** min_disparity ***
        connection.appendBlock((char*)&min_disparity, 4);

        // *** max_disparity ***
        connection.appendBlock((char*)&max_disparity, 4);

        // *** delta_d ***
        connection.appendBlock((char*)&delta_d, 4);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(8);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** image ***
        if (!image.write(connection)) {
            return false;
        }

        // *** f ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)f);

        // *** T ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)T);

        // *** valid_window ***
        if (!valid_window.write(connection)) {
            return false;
        }

        // *** min_disparity ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)min_disparity);

        // *** max_disparity ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)max_disparity);

        // *** delta_d ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)delta_d);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::stereo_msgs::DisparityImage> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::stereo_msgs::DisparityImage> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
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
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: sensor_msgs/Image\n\
") + yarp::rosmsg::sensor_msgs::Image::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: sensor_msgs/RegionOfInterest\n\
") + yarp::rosmsg::sensor_msgs::RegionOfInterest::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::stereo_msgs::DisparityImage::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("stereo_msgs/DisparityImage", "stereo_msgs/DisparityImage");
        typ.addProperty("md5sum", yarp::os::Value("04a177815f75271039fa21f16acad8c9"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace stereo_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_stereo_msgs_DisparityImage_h
