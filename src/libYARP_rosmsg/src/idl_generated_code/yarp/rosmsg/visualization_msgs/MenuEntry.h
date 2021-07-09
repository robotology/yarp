/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "visualization_msgs/MenuEntry" msg definition:
//   # MenuEntry message.
//   
//   # Each InteractiveMarker message has an array of MenuEntry messages.
//   # A collection of MenuEntries together describe a
//   # menu/submenu/subsubmenu/etc tree, though they are stored in a flat
//   # array.  The tree structure is represented by giving each menu entry
//   # an ID number and a "parent_id" field.  Top-level entries are the
//   # ones with parent_id = 0.  Menu entries are ordered within their
//   # level the same way they are ordered in the containing array.  Parent
//   # entries must appear before their children.
//   
//   # Example:
//   # - id = 3
//   #   parent_id = 0
//   #   title = "fun"
//   # - id = 2
//   #   parent_id = 0
//   #   title = "robot"
//   # - id = 4
//   #   parent_id = 2
//   #   title = "pr2"
//   # - id = 5
//   #   parent_id = 2
//   #   title = "turtle"
//   #
//   # Gives a menu tree like this:
//   #  - fun
//   #  - robot
//   #    - pr2
//   #    - turtle
//   
//   # ID is a number for each menu entry.  Must be unique within the
//   # control, and should never be 0.
//   uint32 id
//   
//   # ID of the parent of this menu entry, if it is a submenu.  If this
//   # menu entry is a top-level entry, set parent_id to 0.
//   uint32 parent_id
//   
//   # menu / entry title
//   string title
//   
//   # Arguments to command indicated by command_type (below)
//   string command
//   
//   # Command_type stores the type of response desired when this menu
//   # entry is clicked.
//   # FEEDBACK: send an InteractiveMarkerFeedback message with menu_entry_id set to this entry's id.
//   # ROSRUN: execute "rosrun" with arguments given in the command field (above).
//   # ROSLAUNCH: execute "roslaunch" with arguments given in the command field (above).
//   uint8 FEEDBACK=0
//   uint8 ROSRUN=1
//   uint8 ROSLAUNCH=2
//   uint8 command_type
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_MenuEntry_h
#define YARP_ROSMSG_visualization_msgs_MenuEntry_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class MenuEntry : public yarp::os::idl::WirePortable
{
public:
    std::uint32_t id;
    std::uint32_t parent_id;
    std::string title;
    std::string command;
    static const std::uint8_t FEEDBACK = 0;
    static const std::uint8_t ROSRUN = 1;
    static const std::uint8_t ROSLAUNCH = 2;
    std::uint8_t command_type;

    MenuEntry() :
            id(0),
            parent_id(0),
            title(""),
            command(""),
            command_type(0)
    {
    }

    void clear()
    {
        // *** id ***
        id = 0;

        // *** parent_id ***
        parent_id = 0;

        // *** title ***
        title = "";

        // *** command ***
        command = "";

        // *** FEEDBACK ***

        // *** ROSRUN ***

        // *** ROSLAUNCH ***

        // *** command_type ***
        command_type = 0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** id ***
        id = connection.expectInt32();

        // *** parent_id ***
        parent_id = connection.expectInt32();

        // *** title ***
        int len = connection.expectInt32();
        title.resize(len);
        if (!connection.expectBlock((char*)title.c_str(), len)) {
            return false;
        }

        // *** command ***
        len = connection.expectInt32();
        command.resize(len);
        if (!connection.expectBlock((char*)command.c_str(), len)) {
            return false;
        }

        // *** command_type ***
        command_type = connection.expectInt8();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(8)) {
            return false;
        }

        // *** id ***
        id = reader.expectInt32();

        // *** parent_id ***
        parent_id = reader.expectInt32();

        // *** title ***
        if (!reader.readString(title)) {
            return false;
        }

        // *** command ***
        if (!reader.readString(command)) {
            return false;
        }

        // *** command_type ***
        command_type = reader.expectInt8();

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
        // *** id ***
        connection.appendInt32(id);

        // *** parent_id ***
        connection.appendInt32(parent_id);

        // *** title ***
        connection.appendInt32(title.length());
        connection.appendExternalBlock((char*)title.c_str(), title.length());

        // *** command ***
        connection.appendInt32(command.length());
        connection.appendExternalBlock((char*)command.c_str(), command.length());

        // *** command_type ***
        connection.appendInt8(command_type);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(8);

        // *** id ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(id);

        // *** parent_id ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(parent_id);

        // *** title ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(title.length());
        connection.appendExternalBlock((char*)title.c_str(), title.length());

        // *** command ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(command.length());
        connection.appendExternalBlock((char*)command.c_str(), command.length());

        // *** command_type ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(command_type);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::MenuEntry> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::MenuEntry> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "visualization_msgs/MenuEntry";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "b90ec63024573de83b57aa93eb39be2d";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
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

#endif // YARP_ROSMSG_visualization_msgs_MenuEntry_h
