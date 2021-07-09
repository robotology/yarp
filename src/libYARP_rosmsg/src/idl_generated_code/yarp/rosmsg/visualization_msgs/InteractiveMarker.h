/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "visualization_msgs/InteractiveMarker" msg definition:
//   # Time/frame info.
//   # If header.time is set to 0, the marker will be retransformed into
//   # its frame on each timestep. You will receive the pose feedback
//   # in the same frame.
//   # Otherwise, you might receive feedback in a different frame.
//   # For rviz, this will be the current 'fixed frame' set by the user.
//   Header header
//   
//   # Initial pose. Also, defines the pivot point for rotations.
//   geometry_msgs/Pose pose
//   
//   # Identifying string. Must be globally unique in
//   # the topic that this message is sent through.
//   string name
//   
//   # Short description (< 40 characters).
//   string description
//   
//   # Scale to be used for default controls (default=1).
//   float32 scale
//   
//   # All menu and submenu entries associated with this marker.
//   MenuEntry[] menu_entries
//   
//   # List of controls displayed for this marker.
//   InteractiveMarkerControl[] controls
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_InteractiveMarker_h
#define YARP_ROSMSG_visualization_msgs_InteractiveMarker_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/visualization_msgs/MenuEntry.h>
#include <yarp/rosmsg/visualization_msgs/InteractiveMarkerControl.h>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class InteractiveMarker : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::geometry_msgs::Pose pose;
    std::string name;
    std::string description;
    yarp::conf::float32_t scale;
    std::vector<yarp::rosmsg::visualization_msgs::MenuEntry> menu_entries;
    std::vector<yarp::rosmsg::visualization_msgs::InteractiveMarkerControl> controls;

    InteractiveMarker() :
            header(),
            pose(),
            name(""),
            description(""),
            scale(0.0f),
            menu_entries(),
            controls()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** pose ***
        pose.clear();

        // *** name ***
        name = "";

        // *** description ***
        description = "";

        // *** scale ***
        scale = 0.0f;

        // *** menu_entries ***
        menu_entries.clear();

        // *** controls ***
        controls.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** name ***
        int len = connection.expectInt32();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
            return false;
        }

        // *** description ***
        len = connection.expectInt32();
        description.resize(len);
        if (!connection.expectBlock((char*)description.c_str(), len)) {
            return false;
        }

        // *** scale ***
        scale = connection.expectFloat32();

        // *** menu_entries ***
        len = connection.expectInt32();
        menu_entries.resize(len);
        for (int i=0; i<len; i++) {
            if (!menu_entries[i].read(connection)) {
                return false;
            }
        }

        // *** controls ***
        len = connection.expectInt32();
        controls.resize(len);
        for (int i=0; i<len; i++) {
            if (!controls[i].read(connection)) {
                return false;
            }
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

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** name ***
        if (!reader.readString(name)) {
            return false;
        }

        // *** description ***
        if (!reader.readString(description)) {
            return false;
        }

        // *** scale ***
        scale = reader.expectFloat32();

        // *** menu_entries ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        menu_entries.resize(len);
        for (int i=0; i<len; i++) {
            if (!menu_entries[i].read(connection)) {
                return false;
            }
        }

        // *** controls ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        controls.resize(len);
        for (int i=0; i<len; i++) {
            if (!controls[i].read(connection)) {
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

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** name ***
        connection.appendInt32(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** description ***
        connection.appendInt32(description.length());
        connection.appendExternalBlock((char*)description.c_str(), description.length());

        // *** scale ***
        connection.appendFloat32(scale);

        // *** menu_entries ***
        connection.appendInt32(menu_entries.size());
        for (size_t i=0; i<menu_entries.size(); i++) {
            if (!menu_entries[i].write(connection)) {
                return false;
            }
        }

        // *** controls ***
        connection.appendInt32(controls.size());
        for (size_t i=0; i<controls.size(); i++) {
            if (!controls[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(7);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** name ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** description ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(description.length());
        connection.appendExternalBlock((char*)description.c_str(), description.length());

        // *** scale ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(scale);

        // *** menu_entries ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(menu_entries.size());
        for (size_t i=0; i<menu_entries.size(); i++) {
            if (!menu_entries[i].write(connection)) {
                return false;
            }
        }

        // *** controls ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(controls.size());
        for (size_t i=0; i<controls.size(); i++) {
            if (!controls[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::InteractiveMarker> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::InteractiveMarker> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "visualization_msgs/InteractiveMarker";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "dd86d22909d5a3364b384492e35c10af";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Time/frame info.\n\
# If header.time is set to 0, the marker will be retransformed into\n\
# its frame on each timestep. You will receive the pose feedback\n\
# in the same frame.\n\
# Otherwise, you might receive feedback in a different frame.\n\
# For rviz, this will be the current 'fixed frame' set by the user.\n\
Header header\n\
\n\
# Initial pose. Also, defines the pivot point for rotations.\n\
geometry_msgs/Pose pose\n\
\n\
# Identifying string. Must be globally unique in\n\
# the topic that this message is sent through.\n\
string name\n\
\n\
# Short description (< 40 characters).\n\
string description\n\
\n\
# Scale to be used for default controls (default=1).\n\
float32 scale\n\
\n\
# All menu and submenu entries associated with this marker.\n\
MenuEntry[] menu_entries\n\
\n\
# List of controls displayed for this marker.\n\
InteractiveMarkerControl[] controls\n\
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
MSG: visualization_msgs/MenuEntry\n\
# MenuEntry message.\n\
\n\
# Each InteractiveMarker message has an array of MenuEntry messages.\n\
# A collection of MenuEntries together describe a\n\
# menu/submenu/subsubmenu/etc tree, though they are stored in a flat\n\
# array.  The tree structure is represented by giving each menu entry\n\
# an ID number and a \"parent_id\" field.  Top-level entries are the\n\
# ones with parent_id = 0.  Menu entries are ordered within their\n\
# level the same way they are ordered in the containing array.  Parent\n\
# entries must appear before their children.\n\
\n\
# Example:\n\
# - id = 3\n\
#   parent_id = 0\n\
#   title = \"fun\"\n\
# - id = 2\n\
#   parent_id = 0\n\
#   title = \"robot\"\n\
# - id = 4\n\
#   parent_id = 2\n\
#   title = \"pr2\"\n\
# - id = 5\n\
#   parent_id = 2\n\
#   title = \"turtle\"\n\
#\n\
# Gives a menu tree like this:\n\
#  - fun\n\
#  - robot\n\
#    - pr2\n\
#    - turtle\n\
\n\
# ID is a number for each menu entry.  Must be unique within the\n\
# control, and should never be 0.\n\
uint32 id\n\
\n\
# ID of the parent of this menu entry, if it is a submenu.  If this\n\
# menu entry is a top-level entry, set parent_id to 0.\n\
uint32 parent_id\n\
\n\
# menu / entry title\n\
string title\n\
\n\
# Arguments to command indicated by command_type (below)\n\
string command\n\
\n\
# Command_type stores the type of response desired when this menu\n\
# entry is clicked.\n\
# FEEDBACK: send an InteractiveMarkerFeedback message with menu_entry_id set to this entry's id.\n\
# ROSRUN: execute \"rosrun\" with arguments given in the command field (above).\n\
# ROSLAUNCH: execute \"roslaunch\" with arguments given in the command field (above).\n\
uint8 FEEDBACK=0\n\
uint8 ROSRUN=1\n\
uint8 ROSLAUNCH=2\n\
uint8 command_type\n\
\n\
================================================================================\n\
MSG: visualization_msgs/InteractiveMarkerControl\n\
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

#endif // YARP_ROSMSG_visualization_msgs_InteractiveMarker_h
