// This is an automatically generated file.

// Generated from the following "geometry_msgs/Quaternion" msg definition:
//   # This represents an orientation in free space in quaternion form.
//   
//   float64 x
//   float64 y
//   float64 z
//   float64 w
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Quaternion_h
#define YARP_ROSMSG_geometry_msgs_Quaternion_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Quaternion : public yarp::os::idl::WirePortable
{
public:
    yarp::os::NetFloat64 x;
    yarp::os::NetFloat64 y;
    yarp::os::NetFloat64 z;
    yarp::os::NetFloat64 w;

    Quaternion() :
            x(0.0),
            y(0.0),
            z(0.0),
            w(0.0)
    {
    }

    void clear()
    {
        // *** x ***
        x = 0.0;

        // *** y ***
        y = 0.0;

        // *** z ***
        z = 0.0;

        // *** w ***
        w = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** x ***
        x = connection.expectDouble();

        // *** y ***
        y = connection.expectDouble();

        // *** z ***
        z = connection.expectDouble();

        // *** w ***
        w = connection.expectDouble();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(4)) {
            return false;
        }

        // *** x ***
        x = reader.expectDouble();

        // *** y ***
        y = reader.expectDouble();

        // *** z ***
        z = reader.expectDouble();

        // *** w ***
        w = reader.expectDouble();

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
        // *** x ***
        connection.appendDouble(x);

        // *** y ***
        connection.appendDouble(y);

        // *** z ***
        connection.appendDouble(z);

        // *** w ***
        connection.appendDouble(w);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(4);

        // *** x ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)x);

        // *** y ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)y);

        // *** z ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)z);

        // *** w ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)w);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Quaternion> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Quaternion> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Quaternion", "geometry_msgs/Quaternion");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_Quaternion_h
