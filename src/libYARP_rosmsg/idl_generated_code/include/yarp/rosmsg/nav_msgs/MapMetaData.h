// This is an automatically generated file.

// Generated from the following "nav_msgs/MapMetaData" msg definition:
//   # This hold basic information about the characterists of the OccupancyGrid
//   
//   # The time at which the map was loaded
//   time map_load_time
//   # The map resolution [m/cell]
//   float32 resolution
//   # Map width [cells]
//   uint32 width
//   # Map height [cells]
//   uint32 height
//   # The origin of the map [m, m, rad].  This is the real-world pose of the
//   # cell (0,0) in the map.
//   geometry_msgs/Pose origin// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_nav_msgs_MapMetaData_h
#define YARP_ROSMSG_nav_msgs_MapMetaData_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/TickTime.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>

namespace yarp {
namespace rosmsg {
namespace nav_msgs {

class MapMetaData : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::TickTime map_load_time;
    yarp::os::NetFloat32 resolution;
    yarp::os::NetUint32 width;
    yarp::os::NetUint32 height;
    yarp::rosmsg::geometry_msgs::Pose origin;

    MapMetaData() :
            map_load_time(),
            resolution(0.0),
            width(0),
            height(0),
            origin()
    {
    }

    void clear()
    {
        // *** map_load_time ***
        map_load_time.clear();

        // *** resolution ***
        resolution = 0.0;

        // *** width ***
        width = 0;

        // *** height ***
        height = 0;

        // *** origin ***
        origin.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** map_load_time ***
        if (!map_load_time.read(connection)) {
            return false;
        }

        // *** resolution ***
        if (!connection.expectBlock((char*)&resolution, 4)) {
            return false;
        }

        // *** width ***
        width = connection.expectInt();

        // *** height ***
        height = connection.expectInt();

        // *** origin ***
        if (!origin.read(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(5)) {
            return false;
        }

        // *** map_load_time ***
        if (!map_load_time.read(connection)) {
            return false;
        }

        // *** resolution ***
        resolution = reader.expectDouble();

        // *** width ***
        width = reader.expectInt();

        // *** height ***
        height = reader.expectInt();

        // *** origin ***
        if (!origin.read(connection)) {
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
        // *** map_load_time ***
        if (!map_load_time.write(connection)) {
            return false;
        }

        // *** resolution ***
        connection.appendBlock((char*)&resolution, 4);

        // *** width ***
        connection.appendInt(width);

        // *** height ***
        connection.appendInt(height);

        // *** origin ***
        if (!origin.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(5);

        // *** map_load_time ***
        if (!map_load_time.write(connection)) {
            return false;
        }

        // *** resolution ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)resolution);

        // *** width ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)width);

        // *** height ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)height);

        // *** origin ***
        if (!origin.write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::MapMetaData> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::MapMetaData> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This hold basic information about the characterists of the OccupancyGrid\n\
\n\
# The time at which the map was loaded\n\
time map_load_time\n\
# The map resolution [m/cell]\n\
float32 resolution\n\
# Map width [cells]\n\
uint32 width\n\
# Map height [cells]\n\
uint32 height\n\
# The origin of the map [m, m, rad].  This is the real-world pose of the\n\
# cell (0,0) in the map.\n\
geometry_msgs/Pose origin\n================================================================================\n\
MSG: TickTime\n\
\n================================================================================\n\
MSG: geometry_msgs/Pose\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/MapMetaData", "nav_msgs/MapMetaData");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_MapMetaData_h
