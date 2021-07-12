/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
#include <yarp/os/Type.h>
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
        int len = connection.expectInt32();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) const override
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
        connection.appendInt32(source.length());
        connection.appendExternalBlock((char*)source.c_str(), source.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** time_ref ***
        if (!time_ref.write(connection)) {
            return false;
        }

        // *** source ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(source.length());
        connection.appendExternalBlock((char*)source.c_str(), source.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::TimeReference> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::TimeReference> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/TimeReference";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "fded64a0265108ba86c3d38fb11c0c16";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Measurement from an external time source not actively synchronized with the system clock.\n\
\n\
Header header    # stamp is system time for which measurement was valid\n\
                 # frame_id is not used \n\
\n\
time   time_ref  # corresponding time from this external source\n\
string source    # (optional) name of time source\n\
\n\
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

#endif // YARP_ROSMSG_sensor_msgs_TimeReference_h
