// This is an automatically generated file.

// Generated from the following "nav_msgs/GetMap" msg definition:
//   # Get the map as a nav_msgs/OccupancyGrid
//   ---
//   nav_msgs/OccupancyGrid map
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_nav_msgs_GetMap_h
#define YARP_ROSMSG_nav_msgs_GetMap_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace nav_msgs {

class GetMap : public yarp::os::idl::WirePortable
{
public:

    GetMap()
    {
    }

    void clear()
    {
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(0)) {
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

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(0);


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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::GetMap> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::GetMap> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Get the map as a nav_msgs/OccupancyGrid\n\
---\n\
nav_msgs/OccupancyGrid map";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/GetMap", "nav_msgs/GetMap");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_GetMap_h
