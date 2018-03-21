// This is an automatically generated file.

// Generated from the following "sensor_msgs/ChannelFloat32" msg definition:
//   # This message is used by the PointCloud message to hold optional data
//   # associated with each point in the cloud. The length of the values
//   # array should be the same as the length of the points array in the
//   # PointCloud, and each value should be associated with the corresponding
//   # point.
//   
//   # Channel names in existing practice include:
//   #   "u", "v" - row and column (respectively) in the left stereo image.
//   #              This is opposite to usual conventions but remains for
//   #              historical reasons. The newer PointCloud2 message has no
//   #              such problem.
//   #   "rgb" - For point clouds produced by color stereo cameras. uint8
//   #           (R,G,B) values packed into the least significant 24 bits,
//   #           in order.
//   #   "intensity" - laser or pixel intensity.
//   #   "distance"
//   
//   # The channel name should give semantics of the channel (e.g.
//   # "intensity" instead of "value").
//   string name
//   
//   # The values array should be 1-1 with the elements of the associated
//   # PointCloud.
//   float32[] values
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_ChannelFloat32_h
#define YARP_ROSMSG_sensor_msgs_ChannelFloat32_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class ChannelFloat32 : public yarp::os::idl::WirePortable
{
public:
    std::string name;
    std::vector<yarp::os::NetFloat32> values;

    ChannelFloat32() :
            name(""),
            values()
    {
    }

    void clear()
    {
        // *** name ***
        name = "";

        // *** values ***
        values.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** name ***
        int len = connection.expectInt();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
            return false;
        }

        // *** values ***
        len = connection.expectInt();
        values.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&values[0], sizeof(yarp::os::NetFloat32)*len)) {
            return false;
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

        // *** name ***
        if (!reader.readString(name)) {
            return false;
        }

        // *** values ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        values.resize(len);
        for (int i=0; i<len; i++) {
            values[i] = (yarp::os::NetFloat32)connection.expectDouble();
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

        // *** values ***
        connection.appendInt(values.size());
        if (values.size()>0) {
            connection.appendExternalBlock((char*)&values[0], sizeof(yarp::os::NetFloat32)*values.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** name ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** values ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(values.size());
        for (size_t i=0; i<values.size(); i++) {
            connection.appendDouble((double)values[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::ChannelFloat32> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::ChannelFloat32> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This message is used by the PointCloud message to hold optional data\n\
# associated with each point in the cloud. The length of the values\n\
# array should be the same as the length of the points array in the\n\
# PointCloud, and each value should be associated with the corresponding\n\
# point.\n\
\n\
# Channel names in existing practice include:\n\
#   \"u\", \"v\" - row and column (respectively) in the left stereo image.\n\
#              This is opposite to usual conventions but remains for\n\
#              historical reasons. The newer PointCloud2 message has no\n\
#              such problem.\n\
#   \"rgb\" - For point clouds produced by color stereo cameras. uint8\n\
#           (R,G,B) values packed into the least significant 24 bits,\n\
#           in order.\n\
#   \"intensity\" - laser or pixel intensity.\n\
#   \"distance\"\n\
\n\
# The channel name should give semantics of the channel (e.g.\n\
# \"intensity\" instead of \"value\").\n\
string name\n\
\n\
# The values array should be 1-1 with the elements of the associated\n\
# PointCloud.\n\
float32[] values";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/ChannelFloat32", "sensor_msgs/ChannelFloat32");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_ChannelFloat32_h
