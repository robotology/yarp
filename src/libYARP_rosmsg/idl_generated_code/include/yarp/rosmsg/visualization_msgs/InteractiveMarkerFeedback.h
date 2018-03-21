// This is an automatically generated file.

// Generated from the following "visualization_msgs/InteractiveMarkerFeedback" msg definition:
//   # Time/frame info.
//   Header header
//   
//   # Identifying string. Must be unique in the topic namespace.
//   string client_id
//   
//   # Feedback message sent back from the GUI, e.g.
//   # when the status of an interactive marker was modified by the user.
//   
//   # Specifies which interactive marker and control this message refers to
//   string marker_name
//   string control_name
//   
//   # Type of the event
//   # KEEP_ALIVE: sent while dragging to keep up control of the marker
//   # MENU_SELECT: a menu entry has been selected
//   # BUTTON_CLICK: a button control has been clicked
//   # POSE_UPDATE: the pose has been changed using one of the controls
//   uint8 KEEP_ALIVE = 0
//   uint8 POSE_UPDATE = 1
//   uint8 MENU_SELECT = 2
//   uint8 BUTTON_CLICK = 3
//   
//   uint8 MOUSE_DOWN = 4
//   uint8 MOUSE_UP = 5
//   
//   uint8 event_type
//   
//   # Current pose of the marker
//   # Note: Has to be valid for all feedback types.
//   geometry_msgs/Pose pose
//   
//   # Contains the ID of the selected menu entry
//   # Only valid for MENU_SELECT events.
//   uint32 menu_entry_id
//   
//   # If event_type is BUTTON_CLICK, MOUSE_DOWN, or MOUSE_UP, mouse_point
//   # may contain the 3 dimensional position of the event on the
//   # control.  If it does, mouse_point_valid will be true.  mouse_point
//   # will be relative to the frame listed in the header.
//   geometry_msgs/Point mouse_point
//   bool mouse_point_valid
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_visualization_msgs_InteractiveMarkerFeedback_h
#define YARP_ROSMSG_visualization_msgs_InteractiveMarkerFeedback_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/geometry_msgs/Point.h>

namespace yarp {
namespace rosmsg {
namespace visualization_msgs {

class InteractiveMarkerFeedback : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::string client_id;
    std::string marker_name;
    std::string control_name;
    static const unsigned char KEEP_ALIVE = 0;
    static const unsigned char POSE_UPDATE = 1;
    static const unsigned char MENU_SELECT = 2;
    static const unsigned char BUTTON_CLICK = 3;
    static const unsigned char MOUSE_DOWN = 4;
    static const unsigned char MOUSE_UP = 5;
    unsigned char event_type;
    yarp::rosmsg::geometry_msgs::Pose pose;
    yarp::os::NetUint32 menu_entry_id;
    yarp::rosmsg::geometry_msgs::Point mouse_point;
    bool mouse_point_valid;

    InteractiveMarkerFeedback() :
            header(),
            client_id(""),
            marker_name(""),
            control_name(""),
            event_type(0),
            pose(),
            menu_entry_id(0),
            mouse_point(),
            mouse_point_valid(false)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** client_id ***
        client_id = "";

        // *** marker_name ***
        marker_name = "";

        // *** control_name ***
        control_name = "";

        // *** KEEP_ALIVE ***

        // *** POSE_UPDATE ***

        // *** MENU_SELECT ***

        // *** BUTTON_CLICK ***

        // *** MOUSE_DOWN ***

        // *** MOUSE_UP ***

        // *** event_type ***
        event_type = 0;

        // *** pose ***
        pose.clear();

        // *** menu_entry_id ***
        menu_entry_id = 0;

        // *** mouse_point ***
        mouse_point.clear();

        // *** mouse_point_valid ***
        mouse_point_valid = false;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** client_id ***
        int len = connection.expectInt();
        client_id.resize(len);
        if (!connection.expectBlock((char*)client_id.c_str(), len)) {
            return false;
        }

        // *** marker_name ***
        len = connection.expectInt();
        marker_name.resize(len);
        if (!connection.expectBlock((char*)marker_name.c_str(), len)) {
            return false;
        }

        // *** control_name ***
        len = connection.expectInt();
        control_name.resize(len);
        if (!connection.expectBlock((char*)control_name.c_str(), len)) {
            return false;
        }

        // *** event_type ***
        if (!connection.expectBlock((char*)&event_type, 1)) {
            return false;
        }

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** menu_entry_id ***
        menu_entry_id = connection.expectInt();

        // *** mouse_point ***
        if (!mouse_point.read(connection)) {
            return false;
        }

        // *** mouse_point_valid ***
        if (!connection.expectBlock((char*)&mouse_point_valid, 1)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(15)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** client_id ***
        if (!reader.readString(client_id)) {
            return false;
        }

        // *** marker_name ***
        if (!reader.readString(marker_name)) {
            return false;
        }

        // *** control_name ***
        if (!reader.readString(control_name)) {
            return false;
        }

        // *** event_type ***
        event_type = reader.expectInt();

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** menu_entry_id ***
        menu_entry_id = reader.expectInt();

        // *** mouse_point ***
        if (!mouse_point.read(connection)) {
            return false;
        }

        // *** mouse_point_valid ***
        mouse_point_valid = reader.expectInt();

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

        // *** client_id ***
        connection.appendInt(client_id.length());
        connection.appendExternalBlock((char*)client_id.c_str(), client_id.length());

        // *** marker_name ***
        connection.appendInt(marker_name.length());
        connection.appendExternalBlock((char*)marker_name.c_str(), marker_name.length());

        // *** control_name ***
        connection.appendInt(control_name.length());
        connection.appendExternalBlock((char*)control_name.c_str(), control_name.length());

        // *** event_type ***
        connection.appendBlock((char*)&event_type, 1);

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** menu_entry_id ***
        connection.appendInt(menu_entry_id);

        // *** mouse_point ***
        if (!mouse_point.write(connection)) {
            return false;
        }

        // *** mouse_point_valid ***
        connection.appendBlock((char*)&mouse_point_valid, 1);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(15);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** client_id ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(client_id.length());
        connection.appendExternalBlock((char*)client_id.c_str(), client_id.length());

        // *** marker_name ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(marker_name.length());
        connection.appendExternalBlock((char*)marker_name.c_str(), marker_name.length());

        // *** control_name ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(control_name.length());
        connection.appendExternalBlock((char*)control_name.c_str(), control_name.length());

        // *** event_type ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)event_type);

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** menu_entry_id ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)menu_entry_id);

        // *** mouse_point ***
        if (!mouse_point.write(connection)) {
            return false;
        }

        // *** mouse_point_valid ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)mouse_point_valid);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerFeedback> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::InteractiveMarkerFeedback> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Time/frame info.\n\
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
bool mouse_point_valid\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: geometry_msgs/Pose\n\
\n================================================================================\n\
MSG: geometry_msgs/Point\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/InteractiveMarkerFeedback", "visualization_msgs/InteractiveMarkerFeedback");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_InteractiveMarkerFeedback_h
