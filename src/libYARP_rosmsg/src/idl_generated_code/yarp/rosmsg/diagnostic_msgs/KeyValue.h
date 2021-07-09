/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/KeyValue" msg definition:
//   string key # what to label this value when viewing
//   string value # a value to track over time
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_KeyValue_h
#define YARP_ROSMSG_diagnostic_msgs_KeyValue_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
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
        int len = connection.expectInt32();
        key.resize(len);
        if (!connection.expectBlock((char*)key.c_str(), len)) {
            return false;
        }

        // *** value ***
        len = connection.expectInt32();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) const override
    {
        // *** key ***
        connection.appendInt32(key.length());
        connection.appendExternalBlock((char*)key.c_str(), key.length());

        // *** value ***
        connection.appendInt32(value.length());
        connection.appendExternalBlock((char*)value.c_str(), value.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** key ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(key.length());
        connection.appendExternalBlock((char*)key.c_str(), key.length());

        // *** value ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(value.length());
        connection.appendExternalBlock((char*)value.c_str(), value.length());

        connection.convertTextMode();
        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::write;
    bool write(yarp::os::ConnectionWriter& connection) const override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::KeyValue> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::KeyValue> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "diagnostic_msgs/KeyValue";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "cf57fdc6617a881a88c16e768132149c";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
string key # what to label this value when viewing\n\
string value # a value to track over time\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace diagnostic_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_diagnostic_msgs_KeyValue_h
