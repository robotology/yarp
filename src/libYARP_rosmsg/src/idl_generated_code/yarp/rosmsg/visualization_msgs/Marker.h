/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "visualization_msgs/Marker" msg definition:
//   # See http://www.ros.org/wiki/rviz/DisplayTypes/Marker and http://www.ros.org/wiki/rviz/Tutorials/Markers%3A%20Basic%20Shapes for more information on using this message with rviz
//   
//   uint8 ARROW=0
//   uint8 CUBE=1
//   uint8 SPHERE=2
//   uint8 CYLINDER=3
//   uint8 LINE_STRIP=4
//   uint8 LINE_LIST=5
//   uint8 CUBE_LIST=6
//   uint8 SPHERE_LIST=7
//   uint8 POINTS=8
//   uint8 TEXT_VIEW_FACING=9
//   uint8 MESH_RESOURCE=10
//   uint8 TRIANGLE_LIST=11
//   
//   uint8 ADD=0
//   uint8 MODIFY=0
//   uint8 DELETE=2
//   uint8 DELETEALL=3
//   
//   Header header                        # header for time/frame information
//   string ns                            # Namespace to place this object in... used in conjunction with id to create a unique name for the object
//   int32 id 		                         # object ID useful in conjunction with the namespace for manipulating and deleting the object later
//   int32 type 		                       # Type of object
//   int32 action 	                       # 0 add/modify an object, 1 (deprecated), 2 deletes an object, 3 deletes all objects
//   geometry_msgs/Pose pose                 # Pose of the object
//   geometry_msgs/Vector3 scale             # Scale of the object 1,1,1 means default (usually 1 meter square)
//   std_msgs/ColorRGBA color             # Color [0.0-1.0]
//   duration lifetime                    # How long the object should last before being automatically deleted.  0 means forever
//   bool frame_locked                    # If this marker should be frame-locked, i.e. retransformed into its frame every timestep
//   
//   #Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ...)
//   geometry_msgs/Point[] points
//   #Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ...)
//   #number of colors must either be 0 or equal to the number of points
//   #NOTE: alpha is not yet used
//   std_msgs/ColorRGBA[] colors
//   
//   # NOTE: only used for text markers
//   string text
//   
//   # NOTE: only used for MESH_RESOURCE markers
//   string mesh_resource
//   bool mesh_use_embedded_materials
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_Marker_h
#define YARP_ROSMSG_visualization_msgs_Marker_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/geometry_msgs/Vector3.h>
#include <yarp/rosmsg/std_msgs/ColorRGBA.h>
#include <yarp/rosmsg/TickDuration.h>
#include <yarp/rosmsg/geometry_msgs/Point.h>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class Marker : public yarp::os::idl::WirePortable
{
public:
    static const std::uint8_t ARROW = 0;
    static const std::uint8_t CUBE = 1;
    static const std::uint8_t SPHERE = 2;
    static const std::uint8_t CYLINDER = 3;
    static const std::uint8_t LINE_STRIP = 4;
    static const std::uint8_t LINE_LIST = 5;
    static const std::uint8_t CUBE_LIST = 6;
    static const std::uint8_t SPHERE_LIST = 7;
    static const std::uint8_t POINTS = 8;
    static const std::uint8_t TEXT_VIEW_FACING = 9;
    static const std::uint8_t MESH_RESOURCE = 10;
    static const std::uint8_t TRIANGLE_LIST = 11;
    static const std::uint8_t ADD = 0;
    static const std::uint8_t MODIFY = 0;
    static const std::uint8_t DELETE = 2;
    static const std::uint8_t DELETEALL = 3;
    yarp::rosmsg::std_msgs::Header header;
    std::string ns;
    std::int32_t id;
    std::int32_t type;
    std::int32_t action;
    yarp::rosmsg::geometry_msgs::Pose pose;
    yarp::rosmsg::geometry_msgs::Vector3 scale;
    yarp::rosmsg::std_msgs::ColorRGBA color;
    yarp::rosmsg::TickDuration lifetime;
    bool frame_locked;
    std::vector<yarp::rosmsg::geometry_msgs::Point> points;
    std::vector<yarp::rosmsg::std_msgs::ColorRGBA> colors;
    std::string text;
    std::string mesh_resource;
    bool mesh_use_embedded_materials;

    Marker() :
            header(),
            ns(""),
            id(0),
            type(0),
            action(0),
            pose(),
            scale(),
            color(),
            lifetime(),
            frame_locked(false),
            points(),
            colors(),
            text(""),
            mesh_resource(""),
            mesh_use_embedded_materials(false)
    {
    }

    void clear()
    {
        // *** ARROW ***

        // *** CUBE ***

        // *** SPHERE ***

        // *** CYLINDER ***

        // *** LINE_STRIP ***

        // *** LINE_LIST ***

        // *** CUBE_LIST ***

        // *** SPHERE_LIST ***

        // *** POINTS ***

        // *** TEXT_VIEW_FACING ***

        // *** MESH_RESOURCE ***

        // *** TRIANGLE_LIST ***

        // *** ADD ***

        // *** MODIFY ***

        // *** DELETE ***

        // *** DELETEALL ***

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

        // *** pose ***
        pose.clear();

        // *** scale ***
        scale.clear();

        // *** color ***
        color.clear();

        // *** lifetime ***
        lifetime.clear();

        // *** frame_locked ***
        frame_locked = false;

        // *** points ***
        points.clear();

        // *** colors ***
        colors.clear();

        // *** text ***
        text = "";

        // *** mesh_resource ***
        mesh_resource = "";

        // *** mesh_use_embedded_materials ***
        mesh_use_embedded_materials = false;
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

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** scale ***
        if (!scale.read(connection)) {
            return false;
        }

        // *** color ***
        if (!color.read(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.read(connection)) {
            return false;
        }

        // *** frame_locked ***
        if (!connection.expectBlock((char*)&frame_locked, 1)) {
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

        // *** colors ***
        len = connection.expectInt32();
        colors.resize(len);
        for (int i=0; i<len; i++) {
            if (!colors[i].read(connection)) {
                return false;
            }
        }

        // *** text ***
        len = connection.expectInt32();
        text.resize(len);
        if (!connection.expectBlock((char*)text.c_str(), len)) {
            return false;
        }

        // *** mesh_resource ***
        len = connection.expectInt32();
        mesh_resource.resize(len);
        if (!connection.expectBlock((char*)mesh_resource.c_str(), len)) {
            return false;
        }

        // *** mesh_use_embedded_materials ***
        if (!connection.expectBlock((char*)&mesh_use_embedded_materials, 1)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(31)) {
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

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** scale ***
        if (!scale.read(connection)) {
            return false;
        }

        // *** color ***
        if (!color.read(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.read(connection)) {
            return false;
        }

        // *** frame_locked ***
        frame_locked = reader.expectInt8();

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

        // *** colors ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        colors.resize(len);
        for (int i=0; i<len; i++) {
            if (!colors[i].read(connection)) {
                return false;
            }
        }

        // *** text ***
        if (!reader.readString(text)) {
            return false;
        }

        // *** mesh_resource ***
        if (!reader.readString(mesh_resource)) {
            return false;
        }

        // *** mesh_use_embedded_materials ***
        mesh_use_embedded_materials = reader.expectInt8();

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

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** scale ***
        if (!scale.write(connection)) {
            return false;
        }

        // *** color ***
        if (!color.write(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.write(connection)) {
            return false;
        }

        // *** frame_locked ***
        connection.appendBlock((char*)&frame_locked, 1);

        // *** points ***
        connection.appendInt32(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** colors ***
        connection.appendInt32(colors.size());
        for (size_t i=0; i<colors.size(); i++) {
            if (!colors[i].write(connection)) {
                return false;
            }
        }

        // *** text ***
        connection.appendInt32(text.length());
        connection.appendExternalBlock((char*)text.c_str(), text.length());

        // *** mesh_resource ***
        connection.appendInt32(mesh_resource.length());
        connection.appendExternalBlock((char*)mesh_resource.c_str(), mesh_resource.length());

        // *** mesh_use_embedded_materials ***
        connection.appendBlock((char*)&mesh_use_embedded_materials, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(31);

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

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** scale ***
        if (!scale.write(connection)) {
            return false;
        }

        // *** color ***
        if (!color.write(connection)) {
            return false;
        }

        // *** lifetime ***
        if (!lifetime.write(connection)) {
            return false;
        }

        // *** frame_locked ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(frame_locked);

        // *** points ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** colors ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(colors.size());
        for (size_t i=0; i<colors.size(); i++) {
            if (!colors[i].write(connection)) {
                return false;
            }
        }

        // *** text ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(text.length());
        connection.appendExternalBlock((char*)text.c_str(), text.length());

        // *** mesh_resource ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(mesh_resource.length());
        connection.appendExternalBlock((char*)mesh_resource.c_str(), mesh_resource.length());

        // *** mesh_use_embedded_materials ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(mesh_use_embedded_materials);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::Marker> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::Marker> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "visualization_msgs/Marker";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "4048c9de2a16f4ae8e0538085ebf1b97";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# See http://www.ros.org/wiki/rviz/DisplayTypes/Marker and http://www.ros.org/wiki/rviz/Tutorials/Markers%3A%20Basic%20Shapes for more information on using this message with rviz\n\
\n\
uint8 ARROW=0\n\
uint8 CUBE=1\n\
uint8 SPHERE=2\n\
uint8 CYLINDER=3\n\
uint8 LINE_STRIP=4\n\
uint8 LINE_LIST=5\n\
uint8 CUBE_LIST=6\n\
uint8 SPHERE_LIST=7\n\
uint8 POINTS=8\n\
uint8 TEXT_VIEW_FACING=9\n\
uint8 MESH_RESOURCE=10\n\
uint8 TRIANGLE_LIST=11\n\
\n\
uint8 ADD=0\n\
uint8 MODIFY=0\n\
uint8 DELETE=2\n\
uint8 DELETEALL=3\n\
\n\
Header header                        # header for time/frame information\n\
string ns                            # Namespace to place this object in... used in conjunction with id to create a unique name for the object\n\
int32 id 		                         # object ID useful in conjunction with the namespace for manipulating and deleting the object later\n\
int32 type 		                       # Type of object\n\
int32 action 	                       # 0 add/modify an object, 1 (deprecated), 2 deletes an object, 3 deletes all objects\n\
geometry_msgs/Pose pose                 # Pose of the object\n\
geometry_msgs/Vector3 scale             # Scale of the object 1,1,1 means default (usually 1 meter square)\n\
std_msgs/ColorRGBA color             # Color [0.0-1.0]\n\
duration lifetime                    # How long the object should last before being automatically deleted.  0 means forever\n\
bool frame_locked                    # If this marker should be frame-locked, i.e. retransformed into its frame every timestep\n\
\n\
#Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ...)\n\
geometry_msgs/Point[] points\n\
#Only used if the type specified has some use for them (eg. POINTS, LINE_STRIP, ...)\n\
#number of colors must either be 0 or equal to the number of points\n\
#NOTE: alpha is not yet used\n\
std_msgs/ColorRGBA[] colors\n\
\n\
# NOTE: only used for text markers\n\
string text\n\
\n\
# NOTE: only used for MESH_RESOURCE markers\n\
string mesh_resource\n\
bool mesh_use_embedded_materials\n\
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
MSG: geometry_msgs/Pose\n\
# A representation of pose in free space, composed of position and orientation. \n\
Point position\n\
Quaternion orientation\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
# This represents a vector in free space. \n\
# It is only meant to represent a direction. Therefore, it does not\n\
# make sense to apply a translation to it (e.g., when applying a \n\
# generic rigid transformation to a Vector3, tf2 will only apply the\n\
# rotation). If you want your data to be translatable too, use the\n\
# geometry_msgs/Point message instead.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
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

#endif // YARP_ROSMSG_visualization_msgs_Marker_h
