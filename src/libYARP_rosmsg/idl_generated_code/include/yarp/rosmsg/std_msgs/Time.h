// This is an automatically generated file.

// Generated from the following "std_msgs/Time" msg definition:
//   time data
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_std_msgs_Time_h
#define YARP_ROSMSG_std_msgs_Time_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/TickTime.h>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class Time : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::TickTime data;

    Time() :
            data()
    {
    }

    void clear()
    {
        // *** data ***
        data.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** data ***
        if (!data.read(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(1)) {
            return false;
        }

        // *** data ***
        if (!data.read(connection)) {
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
        // *** data ***
        if (!data.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(1);

        // *** data ***
        if (!data.write(connection)) {
            return false;
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::Time> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::Time> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "time data\n================================================================================\n\
MSG: TickTime\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("std_msgs/Time", "std_msgs/Time");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_Time_h
