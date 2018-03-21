// This is an automatically generated file.

// Generated from the following "sensor_msgs/RelativeHumidity" msg definition:
//    # Single reading from a relative humidity sensor.  Defines the ratio of partial
//    # pressure of water vapor to the saturated vapor pressure at a temperature.
//   
//    Header header             # timestamp of the measurement
//                              # frame_id is the location of the humidity sensor
//   
//    float64 relative_humidity # Expression of the relative humidity
//                              # from 0.0 to 1.0.
//                              # 0.0 is no partial pressure of water vapor
//                              # 1.0 represents partial pressure of saturation
//   
//    float64 variance          # 0 is interpreted as variance unknown// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_RelativeHumidity_h
#define YARP_ROSMSG_sensor_msgs_RelativeHumidity_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class RelativeHumidity : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::os::NetFloat64 relative_humidity;
    yarp::os::NetFloat64 variance;

    RelativeHumidity() :
            header(),
            relative_humidity(0.0),
            variance(0.0)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** relative_humidity ***
        relative_humidity = 0.0;

        // *** variance ***
        variance = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** relative_humidity ***
        relative_humidity = connection.expectDouble();

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

        // *** relative_humidity ***
        relative_humidity = reader.expectDouble();

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

        // *** relative_humidity ***
        connection.appendDouble(relative_humidity);

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

        // *** relative_humidity ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)relative_humidity);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::RelativeHumidity> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::RelativeHumidity> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return " # Single reading from a relative humidity sensor.  Defines the ratio of partial\n\
 # pressure of water vapor to the saturated vapor pressure at a temperature.\n\
\n\
 Header header             # timestamp of the measurement\n\
                           # frame_id is the location of the humidity sensor\n\
\n\
 float64 relative_humidity # Expression of the relative humidity\n\
                           # from 0.0 to 1.0.\n\
                           # 0.0 is no partial pressure of water vapor\n\
                           # 1.0 represents partial pressure of saturation\n\
\n\
 float64 variance          # 0 is interpreted as variance unknown\n================================================================================\n\
MSG: std_msgs/Header\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/RelativeHumidity", "sensor_msgs/RelativeHumidity");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_RelativeHumidity_h
