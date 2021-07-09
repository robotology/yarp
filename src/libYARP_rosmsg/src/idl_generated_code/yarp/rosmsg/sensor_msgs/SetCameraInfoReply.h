/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/SetCameraInfoReply" msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_SetCameraInfoReply_h
#define YARP_ROSMSG_sensor_msgs_SetCameraInfoReply_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class SetCameraInfoReply : public yarp::os::idl::WirePortable
{
public:
    bool success;
    std::string status_message;

    SetCameraInfoReply() :
            success(false),
            status_message("")
    {
    }

    void clear()
    {
        // *** success ***
        success = false;

        // *** status_message ***
        status_message = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** success ***
        if (!connection.expectBlock((char*)&success, 1)) {
            return false;
        }

        // *** status_message ***
        int len = connection.expectInt32();
        status_message.resize(len);
        if (!connection.expectBlock((char*)status_message.c_str(), len)) {
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

        // *** success ***
        success = reader.expectInt8();

        // *** status_message ***
        if (!reader.readString(status_message)) {
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
        // *** success ***
        connection.appendBlock((char*)&success, 1);

        // *** status_message ***
        connection.appendInt32(status_message.length());
        connection.appendExternalBlock((char*)status_message.c_str(), status_message.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** success ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(success);

        // *** status_message ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(status_message.length());
        connection.appendExternalBlock((char*)status_message.c_str(), status_message.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::SetCameraInfoReply> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::SetCameraInfoReply> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/SetCameraInfoReply";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "2ec6f3eff0161f4257b808b12bc830c2";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_SetCameraInfoReply_h
