/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "std_msgs/Header" msg definition:
//   # Standard metadata for higher-level stamped data types.
//   # This is generally used to communicate timestamped data 
//   # in a particular coordinate frame.
//   # 
//   # sequence ID: consecutively increasing ID 
//   uint32 seq
//   #Two-integer timestamp that is expressed as:
//   # * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
//   # * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
//   # time-handling sugar is provided by the client library
//   time stamp
//   #Frame this data is associated with
//   # 0: no frame
//   # 1: global frame
//   string frame_id
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_std_msgs_Header_h
#define YARP_ROSMSG_std_msgs_Header_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/TickTime.h>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class Header : public yarp::os::idl::WirePortable
{
public:
    std::uint32_t seq;
    yarp::rosmsg::TickTime stamp;
    std::string frame_id;

    Header() :
            seq(0),
            stamp(),
            frame_id("")
    {
    }

    void clear()
    {
        // *** seq ***
        seq = 0;

        // *** stamp ***
        stamp.clear();

        // *** frame_id ***
        frame_id = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** seq ***
        seq = connection.expectInt32();

        // *** stamp ***
        if (!stamp.read(connection)) {
            return false;
        }

        // *** frame_id ***
        int len = connection.expectInt32();
        frame_id.resize(len);
        if (!connection.expectBlock((char*)frame_id.c_str(), len)) {
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

        // *** seq ***
        seq = reader.expectInt32();

        // *** stamp ***
        if (!stamp.read(connection)) {
            return false;
        }

        // *** frame_id ***
        if (!reader.readString(frame_id)) {
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
        // *** seq ***
        connection.appendInt32(seq);

        // *** stamp ***
        if (!stamp.write(connection)) {
            return false;
        }

        // *** frame_id ***
        connection.appendInt32(frame_id.length());
        connection.appendExternalBlock((char*)frame_id.c_str(), frame_id.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** seq ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(seq);

        // *** stamp ***
        if (!stamp.write(connection)) {
            return false;
        }

        // *** frame_id ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(frame_id.length());
        connection.appendExternalBlock((char*)frame_id.c_str(), frame_id.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::Header> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::Header> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "std_msgs/Header";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "2176decaecbce78abc3b96ef049fabed";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
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

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_Header_h
