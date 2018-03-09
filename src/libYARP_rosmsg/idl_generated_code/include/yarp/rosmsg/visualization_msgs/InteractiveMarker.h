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
    yarp::os::NetFloat32 scale;
    std::vector<yarp::rosmsg::visualization_msgs::MenuEntry> menu_entries;
    std::vector<yarp::rosmsg::visualization_msgs::InteractiveMarkerControl> controls;

    InteractiveMarker() :
            header(),
            pose(),
            name(""),
            description(""),
            scale(0.0),
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
        scale = 0.0;

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
        int len = connection.expectInt();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
            return false;
        }

        // *** description ***
        len = connection.expectInt();
        description.resize(len);
        if (!connection.expectBlock((char*)description.c_str(), len)) {
            return false;
        }

        // *** scale ***
        if (!connection.expectBlock((char*)&scale, 4)) {
            return false;
        }

        // *** menu_entries ***
        len = connection.expectInt();
        menu_entries.resize(len);
        for (int i=0; i<len; i++) {
            if (!menu_entries[i].read(connection)) {
                return false;
            }
        }

        // *** controls ***
        len = connection.expectInt();
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
        scale = reader.expectDouble();

        // *** menu_entries ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        menu_entries.resize(len);
        for (int i=0; i<len; i++) {
            if (!menu_entries[i].read(connection)) {
                return false;
            }
        }

        // *** controls ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) override
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
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** description ***
        connection.appendInt(description.length());
        connection.appendExternalBlock((char*)description.c_str(), description.length());

        // *** scale ***
        connection.appendBlock((char*)&scale, 4);

        // *** menu_entries ***
        connection.appendInt(menu_entries.size());
        for (size_t i=0; i<menu_entries.size(); i++) {
            if (!menu_entries[i].write(connection)) {
                return false;
            }
        }

        // *** controls ***
        connection.appendInt(controls.size());
        for (size_t i=0; i<controls.size(); i++) {
            if (!controls[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(7);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** name ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** description ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(description.length());
        connection.appendExternalBlock((char*)description.c_str(), description.length());

        // *** scale ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)scale);

        // *** menu_entries ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(menu_entries.size());
        for (size_t i=0; i<menu_entries.size(); i++) {
            if (!menu_entries[i].write(connection)) {
                return false;
            }
        }

        // *** controls ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(controls.size());
        for (size_t i=0; i<controls.size(); i++) {
            if (!controls[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::visualization_msgs::InteractiveMarker> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::visualization_msgs::InteractiveMarker> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Time/frame info.\n\
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
InteractiveMarkerControl[] controls\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: geometry_msgs/Pose\n\
\n================================================================================\n\
MSG: visualization_msgs/MenuEntry\n\
\n================================================================================\n\
MSG: visualization_msgs/InteractiveMarkerControl\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/InteractiveMarker", "visualization_msgs/InteractiveMarker");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace visualization_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_visualization_msgs_InteractiveMarker_h
