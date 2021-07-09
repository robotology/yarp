/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/SelfTestReply" msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_SelfTestReply_h
#define YARP_ROSMSG_diagnostic_msgs_SelfTestReply_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/diagnostic_msgs/DiagnosticStatus.h>

namespace yarp {
namespace rosmsg {
namespace diagnostic_msgs {

class SelfTestReply : public yarp::os::idl::WirePortable
{
public:
    std::string id;
    std::uint8_t passed;
    std::vector<yarp::rosmsg::diagnostic_msgs::DiagnosticStatus> status;

    SelfTestReply() :
            id(""),
            passed(0),
            status()
    {
    }

    void clear()
    {
        // *** id ***
        id = "";

        // *** passed ***
        passed = 0;

        // *** status ***
        status.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** id ***
        int len = connection.expectInt32();
        id.resize(len);
        if (!connection.expectBlock((char*)id.c_str(), len)) {
            return false;
        }

        // *** passed ***
        passed = connection.expectInt8();

        // *** status ***
        len = connection.expectInt32();
        status.resize(len);
        for (int i=0; i<len; i++) {
            if (!status[i].read(connection)) {
                return false;
            }
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

        // *** id ***
        if (!reader.readString(id)) {
            return false;
        }

        // *** passed ***
        passed = reader.expectInt8();

        // *** status ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        status.resize(len);
        for (int i=0; i<len; i++) {
            if (!status[i].read(connection)) {
                return false;
            }
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
        // *** id ***
        connection.appendInt32(id.length());
        connection.appendExternalBlock((char*)id.c_str(), id.length());

        // *** passed ***
        connection.appendInt8(passed);

        // *** status ***
        connection.appendInt32(status.size());
        for (size_t i=0; i<status.size(); i++) {
            if (!status[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** id ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(id.length());
        connection.appendExternalBlock((char*)id.c_str(), id.length());

        // *** passed ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(passed);

        // *** status ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(status.size());
        for (size_t i=0; i<status.size(); i++) {
            if (!status[i].write(connection)) {
                return false;
            }
        }

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::SelfTestReply> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::SelfTestReply> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "diagnostic_msgs/SelfTestReply";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "ac21b1bab7ab17546986536c22eb34e9";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
\n\
================================================================================\n\
MSG: diagnostic_msgs/DiagnosticStatus\n\
# This message holds the status of an individual component of the robot.\n\
# \n\
\n\
# Possible levels of operations\n\
byte OK=0\n\
byte WARN=1\n\
byte ERROR=2\n\
byte STALE=3\n\
\n\
byte level # level of operation enumerated above \n\
string name # a description of the test/component reporting\n\
string message # a description of the status\n\
string hardware_id # a hardware unique string\n\
KeyValue[] values # an array of values associated with the status\n\
\n\
\n\
================================================================================\n\
MSG: diagnostic_msgs/KeyValue\n\
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

#endif // YARP_ROSMSG_diagnostic_msgs_SelfTestReply_h
