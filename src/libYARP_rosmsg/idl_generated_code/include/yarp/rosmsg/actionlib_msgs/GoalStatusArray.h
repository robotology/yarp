// This is an automatically generated file.

// Generated from the following "actionlib_msgs/GoalStatusArray" msg definition:
//   # Stores the statuses for goals that are currently being tracked
//   # by an action server
//   Header header
//   GoalStatus[] status_list
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_actionlib_msgs_GoalStatusArray_h
#define YARP_ROSMSG_actionlib_msgs_GoalStatusArray_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/actionlib_msgs/GoalStatus.h>

namespace yarp {
namespace rosmsg {
namespace actionlib_msgs {

class GoalStatusArray : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<yarp::rosmsg::actionlib_msgs::GoalStatus> status_list;

    GoalStatusArray() :
            header(),
            status_list()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** status_list ***
        status_list.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** status_list ***
        int len = connection.expectInt();
        status_list.resize(len);
        for (int i=0; i<len; i++) {
            if (!status_list[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(2)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** status_list ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        status_list.resize(len);
        for (int i=0; i<len; i++) {
            if (!status_list[i].read(connection)) {
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

        // *** status_list ***
        connection.appendInt(status_list.size());
        for (size_t i=0; i<status_list.size(); i++) {
            if (!status_list[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** status_list ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(status_list.size());
        for (size_t i=0; i<status_list.size(); i++) {
            if (!status_list[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::actionlib_msgs::GoalStatusArray> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::actionlib_msgs::GoalStatusArray> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Stores the statuses for goals that are currently being tracked\n\
# by an action server\n\
Header header\n\
GoalStatus[] status_list\n\
\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: actionlib_msgs/GoalStatus\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("actionlib_msgs/GoalStatusArray", "actionlib_msgs/GoalStatusArray");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace actionlib_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_actionlib_msgs_GoalStatusArray_h
