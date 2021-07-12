/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "visualization_msgs/ImageMarker" msg definition:
//   uint8 CIRCLE=0
//   uint8 LINE_STRIP=1
//   uint8 LINE_LIST=2
//   uint8 POLYGON=3
//   uint8 POINTS=4
//   
//   uint8 ADD=0
//   uint8 REMOVE=1
//   
//   Header header
//   string ns		# namespace, used with id to form a unique id
//   int32 id          	# unique id within the namespace
//   int32 type        	# CIRCLE/LINE_STRIP/etc.
//   int32 action      	# ADD/REMOVE
//   geometry_msgs/Point position # 2D, in pixel-coords
//   float32 scale	 	# the diameter for a circle, etc.
//   std_msgs/ColorRGBA outline_color
//   uint8 filled		# whether to fill in the shape with color
//   std_msgs/ColorRGBA fill_color # color [0.0-1.0]
//   duration lifetime       # How long the object should last before being automatically deleted.  0 means forever
//   
//   
//   geometry_msgs/Point[] points # used for LINE_STRIP/LINE_LIST/POINTS/etc., 2D in pixel coords
//   std_msgs/ColorRGBA[] outline_colors # a color for each line, point, etc.// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_ImageMarker_h
#define YARP_ROSMSG_visualization_msgs_ImageMarker_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Point.h>
#include <yarp/rosmsg/std_msgs/ColorRGBA.h>
#include <yarp/rosmsg/TickDuration.h>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class ImageMarker : public yarp::os::idl::WirePortable
{
public:
    static const std::uint8_t CIRCLE = 0;
    static const std::uint8_t LINE_STRIP = 1;
    static const std::uint8_t LINE_LIST = 2;
    static const std::uint8_t POLYGON = 3;
    static const std::uint8_t POINTS = 4;
    static const std::uint8_t ADD = 0;
    static const std::uint8_t REMOVE = 1;
    yarp::rosmsg::std_msgs::Header header;
    std::string ns;
    std::int32_t id;
    std::int32_t type;
    std::int32_t action;
    yarp::rosmsg::geometry_msgs::Point position;
    yarp::conf::float32_t scale;
    yarp::rosmsg::std_msgs::ColorRGBA outline_color;
    std::uint8_t filled;
    yarp::rosmsg::std_msgs::ColorRGBA fill_color;
    yarp::rosmsg::TickDuration lifetime;
    std::vector<yarp::rosmsg::geometry_msgs::Point> points;
    std::vector<yarp::rosmsg::std_msgs::ColorRGBA> outline_colors;

    ImageMarker() :
            header(),
            ns(""),
            id(0),
            type(0),
            action(0),
            position(),
            scale(0.0f),
            outline_color(),
            filled(0),
            fill_color(),
            lifetime(),
            points(),
            outline_colors()
    {
    }

    void clear()
    {
        // *** CIRCLE ***

        // *** LINE_STRIP ***

        // *** LINE_LIST ***

        // *** POLYGON ***

        // *** POINTS ***

        // *** ADD ***

        // *** REMOVE ***

        // *** header ***
        header.clear();

        // *** ns ***
        ns = "";

        // *** id ***
        id = 0;

        // *** type ***
        type = 0;

        // *** action ***
        action = 0;

        // *** position ***
        position.clear();

        // *** scale ***
        scale = 0.0f;

        // *** outline_color ***
        outline_color.clear();

        // *** filled ***
        filled = 0;

        // *** fill_color ***
        fill_color.clear();

        // *** lifetime ***
        lifetime.clear();

        // *** points ***
        points.clear();

        // *** outline_colors ***
        outline_colors.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** ns ***
        int len = connection.expectInt32();
        ns.resize(len);
        if (!connection.expectBlock((char*)ns.c_str(), len)) {
            return false;
        }

        // *** id ***
        id = connection.expectInt32();

        // *** type ***
        type = connection.expectInt32();

        // *** action ***
        action = connection.expectInt32();

        // *** position ***
        if (!position.read(connection)) {
            return false;
        }

        // *** scale ***
        scale = connection.expectFloat32();

        // *** outline_color ***
        if (!outline_color.read(connection)) {
            return false;
        }

        // *** filled ***
        filled = connection.expectInt8();

        // *** fill_color ***
        if (!fill_color.read(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.read(connection)) {
            return false;
        }

        // *** points ***
        len = connection.expectInt32();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        // *** outline_colors ***
        len = connection.expectInt32();
        outline_colors.resize(len);
        for (int i=0; i<len; i++) {
            if (!outline_colors[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(20)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** ns ***
        if (!reader.readString(ns)) {
            return false;
        }

        // *** id ***
        id = reader.expectInt32();

        // *** type ***
        type = reader.expectInt32();

        // *** action ***
        action = reader.expectInt32();

        // *** position ***
        if (!position.read(connection)) {
            return false;
        }

        // *** scale ***
        scale = reader.expectFloat32();

        // *** outline_color ***
        if (!outline_color.read(connection)) {
            return false;
        }

        // *** filled ***
        filled = reader.expectInt8();

        // *** fill_color ***
        if (!fill_color.read(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.read(connection)) {
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

        // *** outline_colors ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        outline_colors.resize(len);
        for (int i=0; i<len; i++) {
            if (!outline_colors[i].read(connection)) {
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

        // *** ns ***
        connection.appendInt32(ns.length());
        connection.appendExternalBlock((char*)ns.c_str(), ns.length());

        // *** id ***
        connection.appendInt32(id);

        // *** type ***
        connection.appendInt32(type);

        // *** action ***
        connection.appendInt32(action);

        // *** position ***
        if (!position.write(connection)) {
            return false;
        }

        // *** scale ***
        connection.appendFloat32(scale);

        // *** outline_color ***
        if (!outline_color.write(connection)) {
            return false;
        }

        // *** filled ***
        connection.appendInt8(filled);

        // *** fill_color ***
        if (!fill_color.write(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.write(connection)) {
            return false;
        }

        // *** points ***
        connection.appendInt32(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** outline_colors ***
        connection.appendInt32(outline_colors.size());
        for (size_t i=0; i<outline_colors.size(); i++) {
            if (!outline_colors[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(20);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** ns ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(ns.length());
        connection.appendExternalBlock((char*)ns.c_str(), ns.length());

        // *** id ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(id);

        // *** type ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(type);

        // *** action ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(action);

        // *** position ***
        if (!position.write(connection)) {
            return false;
        }

        // *** scale ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(scale);

        // *** outline_color ***
        if (!outline_color.write(connection)) {
            return false;
        }

        // *** filled ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(filled);

        // *** fill_color ***
        if (!fill_color.write(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.write(connection)) {
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

        // *** outline_colors ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(outline_colors.size());
        for (size_t i=0; i<outline_colors.size(); i++) {
            if (!outline_colors[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::ImageMarker> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::ImageMarker> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "visualization_msgs/ImageMarker";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "1de93c67ec8858b831025a08fbf1b35c";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
uint8 CIRCLE=0\n\
uint8 LINE_STRIP=1\n\
uint8 LINE_LIST=2\n\
uint8 POLYGON=3\n\
uint8 POINTS=4\n\
\n\
uint8 ADD=0\n\
uint8 REMOVE=1\n\
\n\
Header header\n\
string ns		# namespace, used with id to form a unique id\n\
int32 id          	# unique id within the namespace\n\
int32 type        	# CIRCLE/LINE_STRIP/etc.\n\
int32 action      	# ADD/REMOVE\n\
geometry_msgs/Point position # 2D, in pixel-coords\n\
float32 scale	 	# the diameter for a circle, etc.\n\
std_msgs/ColorRGBA outline_color\n\
uint8 filled		# whether to fill in the shape with color\n\
std_msgs/ColorRGBA fill_color # color [0.0-1.0]\n\
duration lifetime       # How long the object should last before being automatically deleted.  0 means forever\n\
\n\
\n\
geometry_msgs/Point[] points # used for LINE_STRIP/LINE_LIST/POINTS/etc., 2D in pixel coords\n\
std_msgs/ColorRGBA[] outline_colors # a color for each line, point, etc.\n\
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
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
\n\
================================================================================\n\
MSG: std_msgs/ColorRGBA\n\
float32 r\n\
float32 g\n\
float32 b\n\
float32 a\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_ImageMarker_h
