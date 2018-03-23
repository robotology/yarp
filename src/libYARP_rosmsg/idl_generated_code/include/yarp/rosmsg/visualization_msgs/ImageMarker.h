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
    static const unsigned char CIRCLE = 0;
    static const unsigned char LINE_STRIP = 1;
    static const unsigned char LINE_LIST = 2;
    static const unsigned char POLYGON = 3;
    static const unsigned char POINTS = 4;
    static const unsigned char ADD = 0;
    static const unsigned char REMOVE = 1;
    yarp::rosmsg::std_msgs::Header header;
    std::string ns;
    yarp::os::NetInt32 id;
    yarp::os::NetInt32 type;
    yarp::os::NetInt32 action;
    yarp::rosmsg::geometry_msgs::Point position;
    yarp::os::NetFloat32 scale;
    yarp::rosmsg::std_msgs::ColorRGBA outline_color;
    unsigned char filled;
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
            scale(0.0),
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
        scale = 0.0;

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
        int len = connection.expectInt();
        ns.resize(len);
        if (!connection.expectBlock((char*)ns.c_str(), len)) {
            return false;
        }

        // *** id ***
        id = connection.expectInt();

        // *** type ***
        type = connection.expectInt();

        // *** action ***
        action = connection.expectInt();

        // *** position ***
        if (!position.read(connection)) {
            return false;
        }

        // *** scale ***
        if (!connection.expectBlock((char*)&scale, 4)) {
            return false;
        }

        // *** outline_color ***
        if (!outline_color.read(connection)) {
            return false;
        }

        // *** filled ***
        if (!connection.expectBlock((char*)&filled, 1)) {
            return false;
        }

        // *** fill_color ***
        if (!fill_color.read(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.read(connection)) {
            return false;
        }

        // *** points ***
        len = connection.expectInt();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        // *** outline_colors ***
        len = connection.expectInt();
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
        id = reader.expectInt();

        // *** type ***
        type = reader.expectInt();

        // *** action ***
        action = reader.expectInt();

        // *** position ***
        if (!position.read(connection)) {
            return false;
        }

        // *** scale ***
        scale = reader.expectDouble();

        // *** outline_color ***
        if (!outline_color.read(connection)) {
            return false;
        }

        // *** filled ***
        filled = reader.expectInt();

        // *** fill_color ***
        if (!fill_color.read(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.read(connection)) {
            return false;
        }

        // *** points ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        // *** outline_colors ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** ns ***
        connection.appendInt(ns.length());
        connection.appendExternalBlock((char*)ns.c_str(), ns.length());

        // *** id ***
        connection.appendInt(id);

        // *** type ***
        connection.appendInt(type);

        // *** action ***
        connection.appendInt(action);

        // *** position ***
        if (!position.write(connection)) {
            return false;
        }

        // *** scale ***
        connection.appendBlock((char*)&scale, 4);

        // *** outline_color ***
        if (!outline_color.write(connection)) {
            return false;
        }

        // *** filled ***
        connection.appendBlock((char*)&filled, 1);

        // *** fill_color ***
        if (!fill_color.write(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.write(connection)) {
            return false;
        }

        // *** points ***
        connection.appendInt(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** outline_colors ***
        connection.appendInt(outline_colors.size());
        for (size_t i=0; i<outline_colors.size(); i++) {
            if (!outline_colors[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(20);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** ns ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(ns.length());
        connection.appendExternalBlock((char*)ns.c_str(), ns.length());

        // *** id ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)id);

        // *** type ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)type);

        // *** action ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)action);

        // *** position ***
        if (!position.write(connection)) {
            return false;
        }

        // *** scale ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)scale);

        // *** outline_color ***
        if (!outline_color.write(connection)) {
            return false;
        }

        // *** filled ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)filled);

        // *** fill_color ***
        if (!fill_color.write(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.write(connection)) {
            return false;
        }

        // *** points ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** outline_colors ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(outline_colors.size());
        for (size_t i=0; i<outline_colors.size(); i++) {
            if (!outline_colors[i].write(connection)) {
                return false;
            }
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::ImageMarker> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::ImageMarker> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
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
std_msgs/ColorRGBA[] outline_colors # a color for each line, point, etc.") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
") + yarp::rosmsg::geometry_msgs::Point::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/ColorRGBA\n\
") + yarp::rosmsg::std_msgs::ColorRGBA::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::visualization_msgs::ImageMarker::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/ImageMarker", "visualization_msgs/ImageMarker");
        typ.addProperty("md5sum", yarp::os::Value("2b65f14ce69621f37ce1f10b7d2635a9"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_ImageMarker_h
