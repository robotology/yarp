// This is an automatically generated file.
// Generated from this geometry_msgs_Point.msg definition:
//   # This contains the position of a point in free space
//   float64 x
//   float64 y
//   float64 z// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_geometry_msgs_Point
#define YARPMSG_TYPE_geometry_msgs_Point

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"

class geometry_msgs_Point : public yarp::os::idl::WirePortable
{
public:
    yarp::os::NetFloat64 x;
    yarp::os::NetFloat64 y;
    yarp::os::NetFloat64 z;

    geometry_msgs_Point() :
            x(0.0),
            y(0.0),
            z(0.0)
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
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** x ***
        x = connection.expectDouble();

        // *** y ***
        y = connection.expectDouble();

        // *** z ***
        z = connection.expectDouble();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** x ***
        x = reader.expectDouble();

        // *** y ***
        y = reader.expectDouble();

        // *** z ***
        z = reader.expectDouble();

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

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** x ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)x);

        // *** y ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)y);

        // *** z ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)z);

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
    typedef yarp::os::idl::BareStyle<geometry_msgs_Point> rosStyle;
    typedef yarp::os::idl::BottleStyle<geometry_msgs_Point> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Point", "geometry_msgs/Point");
        typ.addProperty("md5sum", yarp::os::Value("4a842b65f413084dc2b10fb484ea7f17"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

#endif
