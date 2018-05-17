/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    static const unsigned char INHERIT = 0;
    static const unsigned char FIXED = 1;
    static const unsigned char VIEW_FACING = 2;
    unsigned char orientation_mode;
    static const unsigned char NONE = 0;
    static const unsigned char MENU = 1;
    static const unsigned char BUTTON = 2;
    static const unsigned char MOVE_AXIS = 3;
    static const unsigned char MOVE_PLANE = 4;
    static const unsigned char ROTATE_AXIS = 5;
    static const unsigned char MOVE_ROTATE = 6;
    static const unsigned char MOVE_3D = 7;
    static const unsigned char ROTATE_3D = 8;
    static const unsigned char MOVE_ROTATE_3D = 9;
    unsigned char interaction_mode;
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
        int len = connection.expectInt();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.read(connection)) {
            return false;
        }

        // *** orientation_mode ***
        if (!connection.expectBlock((char*)&orientation_mode, 1)) {
            return false;
        }

        // *** interaction_mode ***
        if (!connection.expectBlock((char*)&interaction_mode, 1)) {
            return false;
        }

        // *** always_visible ***
        if (!connection.expectBlock((char*)&always_visible, 1)) {
            return false;
        }

        // *** markers ***
        len = connection.expectInt();
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
        len = connection.expectInt();
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
        orientation_mode = reader.expectInt();

        // *** interaction_mode ***
        interaction_mode = reader.expectInt();

        // *** always_visible ***
        always_visible = reader.expectInt();

        // *** markers ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        markers.resize(len);
        for (int i=0; i<len; i++) {
            if (!markers[i].read(connection)) {
                return false;
            }
        }

        // *** independent_marker_orientation ***
        independent_marker_orientation = reader.expectInt();

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

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** name ***
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** orientation ***
        if (!orientation.write(connection)) {
            return false;
        }

        // *** orientation_mode ***
        connection.appendBlock((char*)&orientation_mode, 1);

        // *** interaction_mode ***
        connection.appendBlock((char*)&interaction_mode, 1);

        // *** always_visible ***
        connection.appendBlock((char*)&always_visible, 1);

        // *** markers ***
        connection.appendInt(markers.size());
        for (size_t i=0; i<markers.size(); i++) {
            if (!markers[i].write(connection)) {
                return false;
            }
        }

        // *** independent_marker_orientation ***
        connection.appendBlock((char*)&independent_marker_orientation, 1);

        // *** description ***
        connection.appendInt(description.length());
        connection.appendExternalBlock((char*)description.c_str(), description.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(21);

        // *** name ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** orientation ***
        if (!orientation.write(connection)) {
            return false;
        }

        // *** orientation_mode ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)orientation_mode);

        // *** interaction_mode ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)interaction_mode);

        // *** always_visible ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)always_visible);

        // *** markers ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(markers.size());
        for (size_t i=0; i<markers.size(); i++) {
            if (!markers[i].write(connection)) {
                return false;
            }
        }

        // *** independent_marker_orientation ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)independent_marker_orientation);

        // *** description ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(description.length());
        connection.appendExternalBlock((char*)description.c_str(), description.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerControl> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerControl> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
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
") + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
") + yarp::rosmsg::geometry_msgs::Quaternion::typeText() + std::string("\n\
================================================================================\n\
MSG: visualization_msgs/Marker\n\
") + yarp::rosmsg::visualization_msgs::Marker::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::visualization_msgs::InteractiveMarkerControl::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/InteractiveMarkerControl", "visualization_msgs/InteractiveMarkerControl");
        typ.addProperty("md5sum", yarp::os::Value("b3c81e785788195d1840b86c28da1aac"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_InteractiveMarkerControl_h
