// This is an automatically generated file.

// Generated from the following "sensor_msgs/TimeReference" msg definition:
//   # Measurement from an external time source not actively synchronized with the system clock.
//   
//   Header header    # stamp is system time for which measurement was valid
//                    # frame_id is not used 
//   
//   time   time_ref  # corresponding time from this external source
//   string source    # (optional) name of time source
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_TimeReference_h
#define YARP_ROSMSG_sensor_msgs_TimeReference_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/TickTime.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class TimeReference : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::TickTime time_ref;
    std::string source;

    TimeReference() :
            header(),
            time_ref(),
            source("")
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** time_ref ***
        time_ref.clear();

        // *** source ***
        source = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** time_ref ***
        if (!time_ref.read(connection)) {
            return false;
        }

        // *** source ***
        int len = connection.expectInt();
        source.resize(len);
        if (!connection.expectBlock((char*)source.c_str(), len)) {
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

        // *** time_ref ***
        if (!time_ref.read(connection)) {
            return false;
        }

        // *** source ***
        if (!reader.readString(source)) {
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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** time_ref ***
        if (!time_ref.write(connection)) {
            return false;
        }

        // *** source ***
        connection.appendInt(source.length());
        connection.appendExternalBlock((char*)source.c_str(), source.length());

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

        // *** time_ref ***
        if (!time_ref.write(connection)) {
            return false;
        }

        // *** source ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(source.length());
        connection.appendExternalBlock((char*)source.c_str(), source.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::TimeReference> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::TimeReference> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Measurement from an external time source not actively synchronized with the system clock.\n\
\n\
Header header    # stamp is system time for which measurement was valid\n\
                 # frame_id is not used \n\
\n\
time   time_ref  # corresponding time from this external source\n\
string source    # (optional) name of time source\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: TickTime\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/TimeReference", "sensor_msgs/TimeReference");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_TimeReference_h
