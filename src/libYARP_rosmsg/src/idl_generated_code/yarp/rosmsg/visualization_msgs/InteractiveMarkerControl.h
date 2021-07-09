/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "visualization_msgs/InteractiveMarkerControl" msg definition:
//   # Represents a control that is to be displayed together with an interactive marker
//   
//   # Identifying string for this control.
//   # You need to assign a unique value to this to receive feedback from the GUI
//   # on what actions the user performs on this control (e.g. a button click).
//   string name
//   
//   
//   # Defines the local coordinate frame (relative to the pose of the parent
//   # interactive marker) in which is being rotated and translated.
//   # Default: Identity
//   geometry_msgs/Quaternion orientation
//   
//   
//   # Orientation mode: controls how orientation changes.
//   # INHERIT: Follow orientation of interactive marker
//   # FIXED: Keep orientation fixed at initial state
//   # VIEW_FACING: Align y-z plane with screen (x: forward, y:left, z:up).
//   uint8 INHERIT = 0 
//   uint8 FIXED = 1
//   uint8 VIEW_FACING = 2
//   
//   uint8 orientation_mode
//   
//   # Interaction mode for this control
//   # 
//   # NONE: This control is only meant for visualization; no context menu.
//   # MENU: Like NONE, but right-click menu is active.
//   # BUTTON: Element can be left-clicked.
//   # MOVE_AXIS: Translate along local x-axis.
//   # MOVE_PLANE: Translate in local y-z plane.
//   # ROTATE_AXIS: Rotate around local x-axis.
//   # MOVE_ROTATE: Combines MOVE_PLANE and ROTATE_AXIS.
//   uint8 NONE = 0 
//   uint8 MENU = 1
//   uint8 BUTTON = 2
//   uint8 MOVE_AXIS = 3 
//   uint8 MOVE_PLANE = 4
//   uint8 ROTATE_AXIS = 5
//   uint8 MOVE_ROTATE = 6
//   # "3D" interaction modes work with the mouse+SHIFT+CTRL or with 3D cursors.
//   # MOVE_3D: Translate freely in 3D space.
//   # ROTATE_3D: Rotate freely in 3D space about the origin of parent frame.
//   # MOVE_ROTATE_3D: Full 6-DOF freedom of translation and rotation about the cursor origin.
//   uint8 MOVE_3D = 7
//   uint8 ROTATE_3D = 8
//   uint8 MOVE_ROTATE_3D = 9
//   
//   uint8 interaction_mode
//   
//   
//   # If true, the contained markers will also be visible
//   # when the gui is not in interactive mode.
//   bool always_visible
//   
//   
//   # Markers to be displayed as custom visual representation.
//   # Leave this empty to use the default control handles.
//   #
//   # Note: 
//   # - The markers can be defined in an arbitrary coordinate frame,
//   #   but will be transformed into the local frame of the interactive marker.
//   # - If the header of a marker is empty, its pose will be interpreted as 
//   #   relative to the pose of the parent interactive marker.
//   Marker[] markers
//   
//   
//   # In VIEW_FACING mode, set this to true if you don't want the markers
//   # to be aligned with the camera view point. The markers will show up
//   # as in INHERIT mode.
//   bool independent_marker_orientation
//   
//   
//   # Short description (< 40 characters) of what this control does,
//   # e.g. "Move the robot". 
//   # Default: A generic description based on the interaction mode
//   string description
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_InteractiveMarkerControl_h
#define YARP_ROSMSG_visualization_msgs_InteractiveMarkerControl_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Quaternion.h>
#include <yarp/rosmsg/visualization_msgs/Marker.h>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class InteractiveMarkerControl : public yarp::os::idl::WirePortable
{
public:
    std::string name;
    yarp::rosmsg::geometry_msgs::Quaternion orientation;
    static const std::uint8_t INHERIT = 0;
    static const std::uint8_t FIXED = 1;
    static const std::uint8_t VIEW_FACING = 2;
    std::uint8_t orientation_mode;
    static const std::uint8_t NONE = 0;
    static const std::uint8_t MENU = 1;
    static const std::uint8_t BUTTON = 2;
    static const std::uint8_t MOVE_AXIS = 3;
    static const std::uint8_t MOVE_PLANE = 4;
    static const std::uint8_t ROTATE_AXIS = 5;
    static const std::uint8_t MOVE_ROTATE = 6;
    static const std::uint8_t MOVE_3D = 7;
    static const std::uint8_t ROTATE_3D = 8;
    static const std::uint8_t MOVE_ROTATE_3D = 9;
    std::uint8_t interaction_mode;
    bool always_visible;
    std::vector<yarp::rosmsg::visualization_msgs::Marker> markers;
    bool independent_marker_orientation;
    std::string description;

    InteractiveMarkerControl() :
            name(""),
            orientation(),
            orientation_mode(0),
            interaction_mode(0),
            always_visible(false),
            markers(),
            independent_marker_orientation(false),
            description("")
    {
    }

    void clear()
    {
        // *** name ***
        name = "";

        // *** orientation ***
        orientation.clear();

        // *** INHERIT ***

        // *** FIXED ***

        // *** VIEW_FACING ***

        // *** orientation_mode ***
        orientation_mode = 0;

        // *** NONE ***

        // *** MENU ***

        // *** BUTTON ***

        // *** MOVE_AXIS ***

        // *** MOVE_PLANE ***

        // *** ROTATE_AXIS ***

        // *** MOVE_ROTATE ***

        // *** MOVE_3D ***

        // *** ROTATE_3D ***

        // *** MOVE_ROTATE_3D ***

        // *** interaction_mode ***
        interaction_mode = 0;

        // *** always_visible ***
        always_visible = false;

        // *** markers ***
        markers.clear();

        // *** independent_marker_orientation ***
        independent_marker_orientation = false;

        // *** description ***
        description = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** name ***
        int len = connection.expectInt32();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.read(connection)) {
            return false;
        }

        // *** orientation_mode ***
        orientation_mode = connection.expectInt8();

        // *** interaction_mode ***
        interaction_mode = connection.expectInt8();

        // *** always_visible ***
        if (!connection.expectBlock((char*)&always_visible, 1)) {
            return false;
        }

        // *** markers ***
        len = connection.expectInt32();
        markers.resize(len);
        for (int i=0; i<len; i++) {
            if (!markers[i].read(connection)) {
                return false;
            }
        }

        // *** independent_marker_orientation ***
        if (!connection.expectBlock((char*)&independent_marker_orientation, 1)) {
            return false;
        }

        // *** description ***
        len = connection.expectInt32();
        description.resize(len);
        if (!connection.expectBlock((char*)description.c_str(), len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(21)) {
            return false;
        }

        // *** name ***
        if (!reader.readString(name)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.read(connection)) {
            return false;
        }

        // *** orientation_mode ***
        orientation_mode = reader.expectInt8();

        // *** interaction_mode ***
        interaction_mode = reader.expectInt8();

        // *** always_visible ***
        always_visible = reader.expectInt8();

        // *** markers ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        markers.resize(len);
        for (int i=0; i<len; i++) {
            if (!markers[i].read(connection)) {
                return false;
            }
        }

        // *** independent_marker_orientation ***
        independent_marker_orientation = reader.expectInt8();

        // *** description ***
        if (!reader.readString(description)) {
            return false;
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
        // *** name ***
        connection.appendInt32(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** orientation ***
        if (!orientation.write(connection)) {
            return false;
        }

        // *** orientation_mode ***
        connection.appendInt8(orientation_mode);

        // *** interaction_mode ***
        connection.appendInt8(interaction_mode);

        // *** always_visible ***
        connection.appendBlock((char*)&always_visible, 1);

        // *** markers ***
        connection.appendInt32(markers.size());
        for (size_t i=0; i<markers.size(); i++) {
            if (!markers[i].write(connection)) {
                return false;
            }
        }

        // *** independent_marker_orientation ***
        connection.appendBlock((char*)&independent_marker_orientation, 1);

        // *** description ***
        connection.appendInt32(description.length());
        connection.appendExternalBlock((char*)description.c_str(), description.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(21);

        // *** name ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** orientation ***
        if (!orientation.write(connection)) {
            return false;
        }

        // *** orientation_mode ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(orientation_mode);

        // *** interaction_mode ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(interaction_mode);

        // *** always_visible ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(always_visible);

        // *** markers ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(markers.size());
        for (size_t i=0; i<markers.size(); i++) {
            if (!markers[i].write(connection)) {
                return false;
            }
        }

        // *** independent_marker_orientation ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(independent_marker_orientation);

        // *** description ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(description.length());
        connection.appendExternalBlock((char*)description.c_str(), description.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerControl> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerControl> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "visualization_msgs/InteractiveMarkerControl";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "b3c81e785788195d1840b86c28da1aac";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Represents a control that is to be displayed together with an interactive marker\n\
\n\
# Identifying string for this control.\n\
# You need to assign a unique value to this to receive feedback from the GUI\n\
# on what actions the user performs on this control (e.g. a button click).\n\
string name\n\
\n\
\n\
# Defines the local coordinate frame (relative to the pose of the parent\n\
# interactive marker) in which is being rotated and translated.\n\
# Default: Identity\n\
geometry_msgs/Quaternion orientation\n\
\n\
\n\
# Orientation mode: controls how orientation changes.\n\
# INHERIT: Follow orientation of interactive marker\n\
# FIXED: Keep orientation fixed at initial state\n\
# VIEW_FACING: Align y-z plane with screen (x: forward, y:left, z:up).\n\
uint8 INHERIT = 0 \n\
uint8 FIXED = 1\n\
uint8 VIEW_FACING = 2\n\
\n\
uint8 orientation_mode\n\
\n\
# Interaction mode for this control\n\
# \n\
# NONE: This control is only meant for visualization; no context menu.\n\
# MENU: Like NONE, but right-click menu is active.\n\
# BUTTON: Element can be left-clicked.\n\
# MOVE_AXIS: Translate along local x-axis.\n\
# MOVE_PLANE: Translate in local y-z plane.\n\
# ROTATE_AXIS: Rotate around local x-axis.\n\
# MOVE_ROTATE: Combines MOVE_PLANE and ROTATE_AXIS.\n\
uint8 NONE = 0 \n\
uint8 MENU = 1\n\
uint8 BUTTON = 2\n\
uint8 MOVE_AXIS = 3 \n\
uint8 MOVE_PLANE = 4\n\
uint8 ROTATE_AXIS = 5\n\
uint8 MOVE_ROTATE = 6\n\
# \"3D\" interaction modes work with the mouse+SHIFT+CTRL or with 3D cursors.\n\
# MOVE_3D: Translate freely in 3D space.\n\
# ROTATE_3D: Rotate freely in 3D space about the origin of parent frame.\n\
# MOVE_ROTATE_3D: Full 6-DOF freedom of translation and rotation about the cursor origin.\n\
uint8 MOVE_3D = 7\n\
uint8 ROTATE_3D = 8\n\
uint8 MOVE_ROTATE_3D = 9\n\
\n\
uint8 interaction_mode\n\
\n\
\n\
# If true, the contained markers will also be visible\n\
# when the gui is not in interactive mode.\n\
bool always_visible\n\
\n\
\n\
# Markers to be displayed as custom visual representation.\n\
# Leave this empty to use the default control handles.\n\
#\n\
# Note: \n\
# - The markers can be defined in an arbitrary coordinate frame,\n\
#   but will be transformed into the local frame of the interactive marker.\n\
# - If the header of a marker is empty, its pose will be interpreted as \n\
#   relative to the pose of the parent interactive marker.\n\
Marker[] markers\n\
\n\
\n\
# In VIEW_FACING mode, set this to true if you don't want the markers\n\
# to be aligned with the camera view point. The markers will show up\n\
# as in INHERIT mode.\n\
bool independent_marker_orientation\n\
\n\
\n\
# Short description (< 40 characters) of what this control does,\n\
# e.g. \"Move the robot\". \n\
# Default: A generic description based on the interaction mode\n\
string description\n\
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
MSG: visualization_msgs/Marker\n\
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

#endif // YARP_ROSMSG_visualization_msgs_InteractiveMarkerControl_h
