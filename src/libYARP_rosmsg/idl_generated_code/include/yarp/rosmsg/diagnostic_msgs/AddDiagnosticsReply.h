/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/AddDiagnosticsReply" msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_AddDiagnosticsReply_h
#define YARP_ROSMSG_diagnostic_msgs_AddDiagnosticsReply_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace diagnostic_msgs {

class AddDiagnosticsReply : public yarp::os::idl::WirePortable
{
public:
    bool success;
    std::string message;

    AddDiagnosticsReply() :
            success(false),
            message("")
    {
    }

    void clear()
    {
        // *** success ***
        success = false;

        // *** message ***
        message = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** success ***
        if (!connection.expectBlock((char*)&success, 1)) {
            return false;
        }

        // *** message ***
        int len = connection.expectInt32();
        message.resize(len);
        if (!connection.expectBlock((char*)message.c_str(), len)) {
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

        // *** message ***
        if (!reader.readString(message)) {
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
        // *** success ***
        connection.appendBlock((char*)&success, 1);

        // *** message ***
        connection.appendInt32(message.length());
        connection.appendExternalBlock((char*)message.c_str(), message.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** success ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(success);

        // *** message ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(message.length());
        connection.appendExternalBlock((char*)message.c_str(), message.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::AddDiagnosticsReply> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::AddDiagnosticsReply> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
");
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::diagnostic_msgs::AddDiagnosticsReply::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName("diagnostic_msgs/AddDiagnosticsReply", "diagnostic_msgs/AddDiagnosticsReply");
        typ.addProperty("md5sum", yarp::os::Value("937c9679a518e3a18d831e57125ea522"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace diagnostic_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_diagnostic_msgs_AddDiagnosticsReply_h
