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
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class MenuEntry : public yarp::os::idl::WirePortable
{
public:
    yarp::os::NetUint32 id;
    yarp::os::NetUint32 parent_id;
    std::string title;
    std::string command;
    static const unsigned char FEEDBACK = 0;
    static const unsigned char ROSRUN = 1;
    static const unsigned char ROSLAUNCH = 2;
    unsigned char command_type;

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
        id = connection.expectInt();

        // *** parent_id ***
        parent_id = connection.expectInt();

        // *** title ***
        int len = connection.expectInt();
        title.resize(len);
        if (!connection.expectBlock((char*)title.c_str(), len)) {
            return false;
        }

        // *** command ***
        len = connection.expectInt();
        command.resize(len);
        if (!connection.expectBlock((char*)command.c_str(), len)) {
            return false;
        }

        // *** command_type ***
        if (!connection.expectBlock((char*)&command_type, 1)) {
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

        // *** id ***
        id = reader.expectInt();

        // *** parent_id ***
        parent_id = reader.expectInt();

        // *** title ***
        if (!reader.readString(title)) {
            return false;
        }

        // *** command ***
        if (!reader.readString(command)) {
            return false;
        }

        // *** command_type ***
        command_type = reader.expectInt();

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
        // *** id ***
        connection.appendInt(id);

        // *** parent_id ***
        connection.appendInt(parent_id);

        // *** title ***
        connection.appendInt(title.length());
        connection.appendExternalBlock((char*)title.c_str(), title.length());

        // *** command ***
        connection.appendInt(command.length());
        connection.appendExternalBlock((char*)command.c_str(), command.length());

        // *** command_type ***
        connection.appendBlock((char*)&command_type, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(8);

        // *** id ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)id);

        // *** parent_id ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)parent_id);

        // *** title ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(title.length());
        connection.appendExternalBlock((char*)title.c_str(), title.length());

        // *** command ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(command.length());
        connection.appendExternalBlock((char*)command.c_str(), command.length());

        // *** command_type ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)command_type);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::MenuEntry> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::MenuEntry> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
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
");
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::visualization_msgs::MenuEntry::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/MenuEntry", "visualization_msgs/MenuEntry");
        typ.addProperty("md5sum", yarp::os::Value("b90ec63024573de83b57aa93eb39be2d"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_MenuEntry_h
