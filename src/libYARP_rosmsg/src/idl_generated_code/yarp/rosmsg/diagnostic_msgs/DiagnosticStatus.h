/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/DiagnosticStatus" msg definition:
//   # This message holds the status of an individual component of the robot.
//   # 
//   
//   # Possible levels of operations
//   byte OK=0
//   byte WARN=1
//   byte ERROR=2
//   byte STALE=3
//   
//   byte level # level of operation enumerated above 
//   string name # a description of the test/component reporting
//   string message # a description of the status
//   string hardware_id # a hardware unique string
//   KeyValue[] values # an array of values associated with the status
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_DiagnosticStatus_h
#define YARP_ROSMSG_diagnostic_msgs_DiagnosticStatus_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/diagnostic_msgs/KeyValue.h>

namespace yarp {
namespace rosmsg {
namespace diagnostic_msgs {

class DiagnosticStatus : public yarp::os::idl::WirePortable
{
public:
    static const std::uint8_t OK = 0;
    static const std::uint8_t WARN = 1;
    static const std::uint8_t ERROR = 2;
    static const std::uint8_t STALE = 3;
    std::uint8_t level;
    std::string name;
    std::string message;
    std::string hardware_id;
    std::vector<yarp::rosmsg::diagnostic_msgs::KeyValue> values;

    DiagnosticStatus() :
            level(0),
            name(""),
            message(""),
            hardware_id(""),
            values()
    {
    }

    void clear()
    {
        // *** OK ***

        // *** WARN ***

        // *** ERROR ***

        // *** STALE ***

        // *** level ***
        level = 0;

        // *** name ***
        name = "";

        // *** message ***
        message = "";

        // *** hardware_id ***
        hardware_id = "";

        // *** values ***
        values.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** level ***
        level = connection.expectInt8();

        // *** name ***
        int len = connection.expectInt32();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
            return false;
        }

        // *** message ***
        len = connection.expectInt32();
        message.resize(len);
        if (!connection.expectBlock((char*)message.c_str(), len)) {
            return false;
        }

        // *** hardware_id ***
        len = connection.expectInt32();
        hardware_id.resize(len);
        if (!connection.expectBlock((char*)hardware_id.c_str(), len)) {
            return false;
        }

        // *** values ***
        len = connection.expectInt32();
        values.resize(len);
        for (int i=0; i<len; i++) {
            if (!values[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(9)) {
            return false;
        }

        // *** level ***
        level = reader.expectInt8();

        // *** name ***
        if (!reader.readString(name)) {
            return false;
        }

        // *** message ***
        if (!reader.readString(message)) {
            return false;
        }

        // *** hardware_id ***
        if (!reader.readString(hardware_id)) {
            return false;
        }

        // *** values ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        values.resize(len);
        for (int i=0; i<len; i++) {
            if (!values[i].read(connection)) {
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
        // *** level ***
        connection.appendInt8(level);

        // *** name ***
        connection.appendInt32(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** message ***
        connection.appendInt32(message.length());
        connection.appendExternalBlock((char*)message.c_str(), message.length());

        // *** hardware_id ***
        connection.appendInt32(hardware_id.length());
        connection.appendExternalBlock((char*)hardware_id.c_str(), hardware_id.length());

        // *** values ***
        connection.appendInt32(values.size());
        for (size_t i=0; i<values.size(); i++) {
            if (!values[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(9);

        // *** level ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(level);

        // *** name ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** message ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(message.length());
        connection.appendExternalBlock((char*)message.c_str(), message.length());

        // *** hardware_id ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(hardware_id.length());
        connection.appendExternalBlock((char*)hardware_id.c_str(), hardware_id.length());

        // *** values ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(values.size());
        for (size_t i=0; i<values.size(); i++) {
            if (!values[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::DiagnosticStatus> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::DiagnosticStatus> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "diagnostic_msgs/DiagnosticStatus";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "d0ce08bc6e5ba34c7754f563a9cabaf1";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
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

#endif // YARP_ROSMSG_diagnostic_msgs_DiagnosticStatus_h
