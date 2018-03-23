/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    static const unsigned char OK = 0;
    static const unsigned char WARN = 1;
    static const unsigned char ERROR = 2;
    static const unsigned char STALE = 3;
    unsigned char level;
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
        if (!connection.expectBlock((char*)&level, 1)) {
            return false;
        }

        // *** name ***
        int len = connection.expectInt();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
            return false;
        }

        // *** message ***
        len = connection.expectInt();
        message.resize(len);
        if (!connection.expectBlock((char*)message.c_str(), len)) {
            return false;
        }

        // *** hardware_id ***
        len = connection.expectInt();
        hardware_id.resize(len);
        if (!connection.expectBlock((char*)hardware_id.c_str(), len)) {
            return false;
        }

        // *** values ***
        len = connection.expectInt();
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
        level = reader.expectInt();

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
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** level ***
        connection.appendBlock((char*)&level, 1);

        // *** name ***
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** message ***
        connection.appendInt(message.length());
        connection.appendExternalBlock((char*)message.c_str(), message.length());

        // *** hardware_id ***
        connection.appendInt(hardware_id.length());
        connection.appendExternalBlock((char*)hardware_id.c_str(), hardware_id.length());

        // *** values ***
        connection.appendInt(values.size());
        for (size_t i=0; i<values.size(); i++) {
            if (!values[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(9);

        // *** level ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)level);

        // *** name ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** message ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(message.length());
        connection.appendExternalBlock((char*)message.c_str(), message.length());

        // *** hardware_id ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(hardware_id.length());
        connection.appendExternalBlock((char*)hardware_id.c_str(), hardware_id.length());

        // *** values ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(values.size());
        for (size_t i=0; i<values.size(); i++) {
            if (!values[i].write(connection)) {
                return false;
            }
        }

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::DiagnosticStatus> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::DiagnosticStatus> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
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
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: diagnostic_msgs/KeyValue\n\
") + yarp::rosmsg::diagnostic_msgs::KeyValue::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::diagnostic_msgs::DiagnosticStatus::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("diagnostic_msgs/DiagnosticStatus", "diagnostic_msgs/DiagnosticStatus");
        typ.addProperty("md5sum", yarp::os::Value("5a412cd6e2444883e5ed4ed18a863d6c"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace diagnostic_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_diagnostic_msgs_DiagnosticStatus_h
