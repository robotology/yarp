/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/DiagnosticArray" msg definition:
//   # This message is used to send diagnostic information about the state of the robot
//   Header header #for timestamp
//   DiagnosticStatus[] status # an array of components being reported on// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_DiagnosticArray_h
#define YARP_ROSMSG_diagnostic_msgs_DiagnosticArray_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/diagnostic_msgs/DiagnosticStatus.h>

namespace yarp {
namespace rosmsg {
namespace diagnostic_msgs {

class DiagnosticArray : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<yarp::rosmsg::diagnostic_msgs::DiagnosticStatus> status;

    DiagnosticArray() :
            header(),
            status()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** status ***
        status.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** status ***
        int len = connection.expectInt32();
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
        if (!reader.readListHeader(2)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

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
        connection.appendInt32(2);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::DiagnosticArray> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::DiagnosticArray> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "diagnostic_msgs/DiagnosticArray";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "60810da900de1dd6ddd437c3503511da";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This message is used to send diagnostic information about the state of the robot\n\
Header header #for timestamp\n\
DiagnosticStatus[] status # an array of components being reported on\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n\
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
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

#endif // YARP_ROSMSG_diagnostic_msgs_DiagnosticArray_h
