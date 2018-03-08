// This is an automatically generated file.

// Generated from the following "sensor_msgs/FluidPressure" msg definition:
//    # Single pressure reading.  This message is appropriate for measuring the
//    # pressure inside of a fluid (air, water, etc).  This also includes
//    # atmospheric or barometric pressure.
//   
//    # This message is not appropriate for force/pressure contact sensors.
//   
//    Header header           # timestamp of the measurement
//                            # frame_id is the location of the pressure sensor
//   
//    float64 fluid_pressure  # Absolute pressure reading in Pascals.
//   
//    float64 variance        # 0 is interpreted as variance unknown// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_FluidPressure_h
#define YARP_ROSMSG_sensor_msgs_FluidPressure_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class FluidPressure : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::os::NetFloat64 fluid_pressure;
    yarp::os::NetFloat64 variance;

    FluidPressure() :
            header(),
            fluid_pressure(0.0),
            variance(0.0)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** fluid_pressure ***
        fluid_pressure = 0.0;

        // *** variance ***
        variance = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** fluid_pressure ***
        fluid_pressure = connection.expectDouble();

        // *** variance ***
        variance = connection.expectDouble();

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

        // *** fluid_pressure ***
        fluid_pressure = reader.expectDouble();

        // *** variance ***
        variance = reader.expectDouble();

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

        // *** fluid_pressure ***
        connection.appendDouble(fluid_pressure);

        // *** variance ***
        connection.appendDouble(variance);

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

        // *** fluid_pressure ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)fluid_pressure);

        // *** variance ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)variance);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::FluidPressure> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::FluidPressure> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return " # Single pressure reading.  This message is appropriate for measuring the\n\
 # pressure inside of a fluid (air, water, etc).  This also includes\n\
 # atmospheric or barometric pressure.\n\
\n\
 # This message is not appropriate for force/pressure contact sensors.\n\
\n\
 Header header           # timestamp of the measurement\n\
                         # frame_id is the location of the pressure sensor\n\
\n\
 float64 fluid_pressure  # Absolute pressure reading in Pascals.\n\
\n\
 float64 variance        # 0 is interpreted as variance unknown\n================================================================================\n\
MSG: std_msgs/Header\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/FluidPressure", "sensor_msgs/FluidPressure");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_FluidPressure_h
