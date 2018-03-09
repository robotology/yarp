// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/KeyValue" msg definition:
//   string key # what to label this value when viewing
//   string value # a value to track over time
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_KeyValue_h
#define YARP_ROSMSG_diagnostic_msgs_KeyValue_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace diagnostic_msgs {

class KeyValue : public yarp::os::idl::WirePortable
{
public:
    std::string key;
    std::string value;

    KeyValue() :
            key(""),
            value("")
    {
    }

    void clear()
    {
        // *** key ***
        key = "";

        // *** value ***
        value = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** key ***
        int len = connection.expectInt();
        key.resize(len);
        if (!connection.expectBlock((char*)key.c_str(), len)) {
            return false;
        }

        // *** value ***
        len = connection.expectInt();
        value.resize(len);
        if (!connection.expectBlock((char*)value.c_str(), len)) {
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

        // *** key ***
        if (!reader.readString(key)) {
            return false;
        }

        // *** value ***
        if (!reader.readString(value)) {
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
        // *** key ***
        connection.appendInt(key.length());
        connection.appendExternalBlock((char*)key.c_str(), key.length());

        // *** value ***
        connection.appendInt(value.length());
        connection.appendExternalBlock((char*)value.c_str(), value.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** key ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(key.length());
        connection.appendExternalBlock((char*)key.c_str(), key.length());

        // *** value ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(value.length());
        connection.appendExternalBlock((char*)value.c_str(), value.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::KeyValue> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::KeyValue> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "string key # what to label this value when viewing\n\
string value # a value to track over time";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("diagnostic_msgs/KeyValue", "diagnostic_msgs/KeyValue");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace diagnostic_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_diagnostic_msgs_KeyValue_h
