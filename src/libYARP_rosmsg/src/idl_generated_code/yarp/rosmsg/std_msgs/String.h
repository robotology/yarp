/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "std_msgs/String" msg definition:
//   string data
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_std_msgs_String_h
#define YARP_ROSMSG_std_msgs_String_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class String : public yarp::os::idl::WirePortable
{
public:
    std::string data;

    String() :
            data("")
    {
    }

    void clear()
    {
        // *** data ***
        data = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** data ***
        int len = connection.expectInt32();
        data.resize(len);
        if (!connection.expectBlock((char*)data.c_str(), len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(1)) {
            return false;
        }

        // *** data ***
        if (!reader.readString(data)) {
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
        // *** data ***
        connection.appendInt32(data.length());
        connection.appendExternalBlock((char*)data.c_str(), data.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(1);

        // *** data ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(data.length());
        connection.appendExternalBlock((char*)data.c_str(), data.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::String> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::String> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "std_msgs/String";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "992ce8a1687cec8c8bd883ec73ca41d1";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
string data\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_String_h
