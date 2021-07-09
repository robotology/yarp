/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "actionlib_msgs/GoalID" msg definition:
//   # The stamp should store the time at which this goal was requested.
//   # It is used by an action server when it tries to preempt all
//   # goals that were requested before a certain time
//   time stamp
//   
//   # The id provides a way to associate feedback and
//   # result message with specific goal requests. The id
//   # specified must be unique.
//   string id
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_actionlib_msgs_GoalID_h
#define YARP_ROSMSG_actionlib_msgs_GoalID_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/TickTime.h>

namespace yarp {
namespace rosmsg {
namespace actionlib_msgs {

class GoalID : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::TickTime stamp;
    std::string id;

    GoalID() :
            stamp(),
            id("")
    {
    }

    void clear()
    {
        // *** stamp ***
        stamp.clear();

        // *** id ***
        id = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** stamp ***
        if (!stamp.read(connection)) {
            return false;
        }

        // *** id ***
        int len = connection.expectInt32();
        id.resize(len);
        if (!connection.expectBlock((char*)id.c_str(), len)) {
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

        // *** stamp ***
        if (!stamp.read(connection)) {
            return false;
        }

        // *** id ***
        if (!reader.readString(id)) {
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
        // *** stamp ***
        if (!stamp.write(connection)) {
            return false;
        }

        // *** id ***
        connection.appendInt32(id.length());
        connection.appendExternalBlock((char*)id.c_str(), id.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** stamp ***
        if (!stamp.write(connection)) {
            return false;
        }

        // *** id ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(id.length());
        connection.appendExternalBlock((char*)id.c_str(), id.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::actionlib_msgs::GoalID> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::actionlib_msgs::GoalID> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "actionlib_msgs/GoalID";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "302881f31927c1df708a2dbab0e80ee8";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# The stamp should store the time at which this goal was requested.\n\
# It is used by an action server when it tries to preempt all\n\
# goals that were requested before a certain time\n\
time stamp\n\
\n\
# The id provides a way to associate feedback and\n\
# result message with specific goal requests. The id\n\
# specified must be unique.\n\
string id\n\
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

} // namespace actionlib_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_actionlib_msgs_GoalID_h
