// This is an automatically generated file.

// Generated from the following "nav_msgs/OccupancyGrid" msg definition:
//   # This represents a 2-D grid map, in which each cell represents the probability of
//   # occupancy.
//   
//   Header header 
//   
//   #MetaData for the map
//   MapMetaData info
//   
//   # The map data, in row-major order, starting with (0,0).  Occupancy
//   # probabilities are in the range [0,100].  Unknown is -1.
//   int8[] data
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_nav_msgs_OccupancyGrid_h
#define YARP_ROSMSG_nav_msgs_OccupancyGrid_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/nav_msgs/MapMetaData.h>

namespace yarp {
namespace rosmsg {
namespace nav_msgs {

class OccupancyGrid : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::nav_msgs::MapMetaData info;
    std::vector<char> data;

    OccupancyGrid() :
            header(),
            info(),
            data()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** info ***
        info.clear();

        // *** data ***
        data.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** info ***
        if (!info.read(connection)) {
            return false;
        }

        // *** data ***
        int len = connection.expectInt();
        data.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&data[0], sizeof(char)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** info ***
        if (!info.read(connection)) {
            return false;
        }

        // *** data ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) {
            return false;
        }
        int len = connection.expectInt();
        data.resize(len);
        for (int i=0; i<len; i++) {
            data[i] = (char)connection.expectInt();
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

        // *** info ***
        if (!info.write(connection)) {
            return false;
        }

        // *** data ***
        connection.appendInt(data.size());
        if (data.size()>0) {
            connection.appendExternalBlock((char*)&data[0], sizeof(char)*data.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** info ***
        if (!info.write(connection)) {
            return false;
        }

        // *** data ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_INT);
        connection.appendInt(data.size());
        for (size_t i=0; i<data.size(); i++) {
            connection.appendInt((int)data[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::OccupancyGrid> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::OccupancyGrid> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This represents a 2-D grid map, in which each cell represents the probability of\n\
# occupancy.\n\
\n\
Header header \n\
\n\
#MetaData for the map\n\
MapMetaData info\n\
\n\
# The map data, in row-major order, starting with (0,0).  Occupancy\n\
# probabilities are in the range [0,100].  Unknown is -1.\n\
int8[] data\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: nav_msgs/MapMetaData\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/OccupancyGrid", "nav_msgs/OccupancyGrid");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_OccupancyGrid_h
