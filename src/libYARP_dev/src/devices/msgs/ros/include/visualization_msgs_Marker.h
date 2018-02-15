// This is an automatically generated file.
// Generated from this visualization_msgs_Marker.msg definition:
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
//   int32 id                           # object ID useful in conjunction with the namespace for manipulating and deleting the object later
//   int32 type                         # Type of object
//   int32 action                         # 0 add/modify an object, 1 (deprecated), 2 deletes an object, 3 deletes all objects
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

#ifndef YARPMSG_TYPE_visualization_msgs_Marker
#define YARPMSG_TYPE_visualization_msgs_Marker

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "geometry_msgs_Point.h"
#include "geometry_msgs_Quaternion.h"
#include "geometry_msgs_Pose.h"
#include "geometry_msgs_Vector3.h"
#include "std_msgs_ColorRGBA.h"
#include "TickDuration.h"

class visualization_msgs_Marker : public yarp::os::idl::WirePortable
{
public:
    static const unsigned char ARROW = 0;
    static const unsigned char CUBE = 1;
    static const unsigned char SPHERE = 2;
    static const unsigned char CYLINDER = 3;
    static const unsigned char LINE_STRIP = 4;
    static const unsigned char LINE_LIST = 5;
    static const unsigned char CUBE_LIST = 6;
    static const unsigned char SPHERE_LIST = 7;
    static const unsigned char POINTS = 8;
    static const unsigned char TEXT_VIEW_FACING = 9;
    static const unsigned char MESH_RESOURCE = 10;
    static const unsigned char TRIANGLE_LIST = 11;
    static const unsigned char ADD = 0;
    static const unsigned char MODIFY = 0;
    static const unsigned char DELETE = 2;
    static const unsigned char DELETEALL = 3;
    std_msgs_Header header;
    std::string ns;
    yarp::os::NetInt32 id;
    yarp::os::NetInt32 type;
    yarp::os::NetInt32 action;
    geometry_msgs_Pose pose;
    geometry_msgs_Vector3 scale;
    std_msgs_ColorRGBA color;
    TickDuration lifetime;
    bool frame_locked;
    std::vector<geometry_msgs_Point> points;
    std::vector<std_msgs_ColorRGBA> colors;
    std::string text;
    std::string mesh_resource;
    bool mesh_use_embedded_materials;

    visualization_msgs_Marker() :
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
        len = connection.expectInt();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        // *** colors ***
        len = connection.expectInt();
        colors.resize(len);
        for (int i=0; i<len; i++) {
            if (!colors[i].read(connection)) {
                return false;
            }
        }

        // *** text ***
        len = connection.expectInt();
        text.resize(len);
        if (!connection.expectBlock((char*)text.c_str(), len)) {
            return false;
        }

        // *** mesh_resource ***
        len = connection.expectInt();
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
        id = reader.expectInt();

        // *** type ***
        type = reader.expectInt();

        // *** action ***
        action = reader.expectInt();

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
        frame_locked = reader.expectInt();

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

        // *** colors ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
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
        mesh_use_embedded_materials = reader.expectInt();

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
        connection.appendInt(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** colors ***
        connection.appendInt(colors.size());
        for (size_t i=0; i<colors.size(); i++) {
            if (!colors[i].write(connection)) {
                return false;
            }
        }

        // *** text ***
        connection.appendInt(text.length());
        connection.appendExternalBlock((char*)text.c_str(), text.length());

        // *** mesh_resource ***
        connection.appendInt(mesh_resource.length());
        connection.appendExternalBlock((char*)mesh_resource.c_str(), mesh_resource.length());

        // *** mesh_use_embedded_materials ***
        connection.appendBlock((char*)&mesh_use_embedded_materials, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(31);

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
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)frame_locked);

        // *** points ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** colors ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(colors.size());
        for (size_t i=0; i<colors.size(); i++) {
            if (!colors[i].write(connection)) {
                return false;
            }
        }

        // *** text ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(text.length());
        connection.appendExternalBlock((char*)text.c_str(), text.length());

        // *** mesh_resource ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(mesh_resource.length());
        connection.appendExternalBlock((char*)mesh_resource.c_str(), mesh_resource.length());

        // *** mesh_use_embedded_materials ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)mesh_use_embedded_materials);

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
    typedef yarp::os::idl::BareStyle<visualization_msgs_Marker> rosStyle;
    typedef yarp::os::idl::BottleStyle<visualization_msgs_Marker> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# See http://www.ros.org/wiki/rviz/DisplayTypes/Marker and http://www.ros.org/wiki/rviz/Tutorials/Markers%3A%20Basic%20Shapes for more information on using this message with rviz\n\
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
int32 id                           # object ID useful in conjunction with the namespace for manipulating and deleting the object later\n\
int32 type                         # Type of object\n\
int32 action                         # 0 add/modify an object, 1 (deprecated), 2 deletes an object, 3 deletes all objects\n\
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
bool mesh_use_embedded_materials\n================================================================================\n\
MSG: std_msgs/Header\n\
[std_msgs/Header]:\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data\n\
# in a particular coordinate frame.\n\
#\n\
# sequence ID: consecutively increasing ID\n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n\
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n================================================================================\n\
MSG: geometry_msgs/Pose\n\
# A representation of pose in free space, composed of postion and orientation. \n\
geometry_msgs/Point position\n\
geometry_msgs/Quaternion orientation\n================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n================================================================================\n\
MSG: geometry_msgs/Vector3\n\
# This represents a vector in free space.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n================================================================================\n\
MSG: std_msgs/ColorRGBA\n\
float32 r\n\
float32 g\n\
float32 b\n\
float32 a";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/Marker", "visualization_msgs/Marker");
        typ.addProperty("md5sum", yarp::os::Value("4048c9de2a16f4ae8e0538085ebf1b97"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

#endif
