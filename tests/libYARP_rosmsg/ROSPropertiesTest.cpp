/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/rosmsg/std_msgs/Int32.h>
#include <yarp/rosmsg/std_msgs/String.h>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/std_msgs/UInt8MultiArray.h>
#include <yarp/rosmsg/std_msgs/Time.h>
#include <yarp/rosmsg/visualization_msgs/Marker.h>
#include <yarp/rosmsg/visualization_msgs/InteractiveMarker.h>
#include <yarp/rosmsg/visualization_msgs/InteractiveMarkerFeedback.h>

#include <iostream>

#include <catch.hpp>
#include <harness.h>

template <typename T>
void checkName(std::string expectedName)
{
    T msg;
    yarp::os::Type t = msg.getType();
    INFO("Checking name for " << t.getName());
    CHECK(t.getName() == expectedName);
    INFO("Checking name on wire for " << t.getName());
    CHECK(t.getNameOnWire() == expectedName);
}

template <typename T>
void checkMd5sum(std::string expectedMd5sum)
{
    T msg;
    yarp::os::Type t = msg.getType();
    INFO("Checking md5 for " << t.getName());
    CHECK(t.readProperties().find("md5sum").asString() == expectedMd5sum);
}

template <typename T>
void checkDefinition(std::string expectedDefinition)
{
    T msg;
    yarp::os::Type t = msg.getType();
    INFO("Checking message definition for " << t.getName());
    CHECK(t.readProperties().find("message_definition").asString() == expectedDefinition);
}

TEST_CASE("rosmsg::ROSPropertiesTest", "[yarp::rosmsg]")
{


    // The expected md5sums and definitions can be found in ROS headers
    SECTION("std_msgs/Int32")
    {
        checkName<yarp::rosmsg::std_msgs::Int32>("std_msgs/Int32");
        checkMd5sum<yarp::rosmsg::std_msgs::Int32>("da5909fbe378aeaf85e547e830cc1bb7");
        checkDefinition<yarp::rosmsg::std_msgs::Int32>(
"int32 data\n\
");
    }

    SECTION("std_msgs/String")
    {
        checkName<yarp::rosmsg::std_msgs::String>("std_msgs/String");
        checkMd5sum<yarp::rosmsg::std_msgs::String>("992ce8a1687cec8c8bd883ec73ca41d1");
        checkDefinition<yarp::rosmsg::std_msgs::String>(
"string data\n\
");
    }

    SECTION("std_msgs/Header")
    {
        checkName<yarp::rosmsg::std_msgs::Header>("std_msgs/Header");
        checkMd5sum<yarp::rosmsg::std_msgs::Header>("2176decaecbce78abc3b96ef049fabed");
        checkDefinition<yarp::rosmsg::std_msgs::Header>(
"# Standard metadata for higher-level stamped data types.\n\
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
");
    }

    SECTION("std_msgs/UInt8MultiArray")
    {
        checkName<yarp::rosmsg::std_msgs::UInt8MultiArray>("std_msgs/UInt8MultiArray");
        checkMd5sum<yarp::rosmsg::std_msgs::UInt8MultiArray>("82373f1612381bb6ee473b5cd6f5d89c");
        checkDefinition<yarp::rosmsg::std_msgs::UInt8MultiArray>(
"# Please look at the MultiArrayLayout message definition for\n\
# documentation on all multiarrays.\n\
\n\
MultiArrayLayout  layout        # specification of data layout\n\
uint8[]           data          # array of data\n\
\n\
\n\
================================================================================\n\
MSG: std_msgs/MultiArrayLayout\n\
# The multiarray declares a generic multi-dimensional array of a\n\
# particular data type.  Dimensions are ordered from outer most\n\
# to inner most.\n\
\n\
MultiArrayDimension[] dim # Array of dimension properties\n\
uint32 data_offset        # padding elements at front of data\n\
\n\
# Accessors should ALWAYS be written in terms of dimension stride\n\
# and specified outer-most dimension first.\n\
# \n\
# multiarray(i,j,k) = data[data_offset + dim_stride[1]*i + dim_stride[2]*j + k]\n\
#\n\
# A standard, 3-channel 640x480 image with interleaved color channels\n\
# would be specified as:\n\
#\n\
# dim[0].label  = \"height\"\n\
# dim[0].size   = 480\n\
# dim[0].stride = 3*640*480 = 921600  (note dim[0] stride is just size of image)\n\
# dim[1].label  = \"width\"\n\
# dim[1].size   = 640\n\
# dim[1].stride = 3*640 = 1920\n\
# dim[2].label  = \"channel\"\n\
# dim[2].size   = 3\n\
# dim[2].stride = 3\n\
#\n\
# multiarray(i,j,k) refers to the ith row, jth column, and kth channel.\n\
\n\
================================================================================\n\
MSG: std_msgs/MultiArrayDimension\n\
string label   # label of given dimension\n\
uint32 size    # size of given dimension (in type units)\n\
uint32 stride  # stride of given dimension\n\
");
    }

    SECTION("std_msgs/Time")
    {
        checkName<yarp::rosmsg::std_msgs::Time>("std_msgs/Time");
        checkMd5sum<yarp::rosmsg::std_msgs::Time>("cd7166c74c552c311fbcc2fe5a7bc289");
        checkDefinition<yarp::rosmsg::std_msgs::Time>(
"time data\n\
");
    }

    SECTION("visualization_msgs/Marker")
    {
        checkName<yarp::rosmsg::visualization_msgs::Marker>("visualization_msgs/Marker");
        checkMd5sum<yarp::rosmsg::visualization_msgs::Marker>("4048c9de2a16f4ae8e0538085ebf1b97");
        checkDefinition<yarp::rosmsg::visualization_msgs::Marker>(
"# See http://www.ros.org/wiki/rviz/DisplayTypes/Marker and http://www.ros.org/wiki/rviz/Tutorials/Markers%3A%20Basic%20Shapes for more information on using this message with rviz\n\
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
");
    }

    SECTION("visualization_msgs/InteractiveMarker")
    {
        checkName<yarp::rosmsg::visualization_msgs::InteractiveMarker>("visualization_msgs/InteractiveMarker");
        checkMd5sum<yarp::rosmsg::visualization_msgs::InteractiveMarker>("dd86d22909d5a3364b384492e35c10af");
        checkDefinition<yarp::rosmsg::visualization_msgs::InteractiveMarker>(
"# Time/frame info.\n\
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
");
    }

    SECTION("visualization_msgs/InteractiveMarkerFeedback")
    {
        checkName<yarp::rosmsg::visualization_msgs::InteractiveMarkerFeedback>("visualization_msgs/InteractiveMarkerFeedback");
        checkMd5sum<yarp::rosmsg::visualization_msgs::InteractiveMarkerFeedback>("ab0f1eee058667e28c19ff3ffc3f4b78");
        checkDefinition<yarp::rosmsg::visualization_msgs::InteractiveMarkerFeedback>(
"# Time/frame info.\n\
Header header\n\
\n\
# Identifying string. Must be unique in the topic namespace.\n\
string client_id\n\
\n\
# Feedback message sent back from the GUI, e.g.\n\
# when the status of an interactive marker was modified by the user.\n\
\n\
# Specifies which interactive marker and control this message refers to\n\
string marker_name\n\
string control_name\n\
\n\
# Type of the event\n\
# KEEP_ALIVE: sent while dragging to keep up control of the marker\n\
# MENU_SELECT: a menu entry has been selected\n\
# BUTTON_CLICK: a button control has been clicked\n\
# POSE_UPDATE: the pose has been changed using one of the controls\n\
uint8 KEEP_ALIVE = 0\n\
uint8 POSE_UPDATE = 1\n\
uint8 MENU_SELECT = 2\n\
uint8 BUTTON_CLICK = 3\n\
\n\
uint8 MOUSE_DOWN = 4\n\
uint8 MOUSE_UP = 5\n\
\n\
uint8 event_type\n\
\n\
# Current pose of the marker\n\
# Note: Has to be valid for all feedback types.\n\
geometry_msgs/Pose pose\n\
\n\
# Contains the ID of the selected menu entry\n\
# Only valid for MENU_SELECT events.\n\
uint32 menu_entry_id\n\
\n\
# If event_type is BUTTON_CLICK, MOUSE_DOWN, or MOUSE_UP, mouse_point\n\
# may contain the 3 dimensional position of the event on the\n\
# control.  If it does, mouse_point_valid will be true.  mouse_point\n\
# will be relative to the frame listed in the header.\n\
geometry_msgs/Point mouse_point\n\
bool mouse_point_valid\n\
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
");
    }
}
