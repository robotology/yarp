/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "actionlib_msgs/GoalStatus" msg definition:
//   GoalID goal_id
//   uint8 status
//   uint8 PENDING         = 0   # The goal has yet to be processed by the action server
//   uint8 ACTIVE          = 1   # The goal is currently being processed by the action server
//   uint8 PREEMPTED       = 2   # The goal received a cancel request after it started executing
//                               #   and has since completed its execution (Terminal State)
//   uint8 SUCCEEDED       = 3   # The goal was achieved successfully by the action server (Terminal State)
//   uint8 ABORTED         = 4   # The goal was aborted during execution by the action server due
//                               #    to some failure (Terminal State)
//   uint8 REJECTED        = 5   # The goal was rejected by the action server without being processed,
//                               #    because the goal was unattainable or invalid (Terminal State)
//   uint8 PREEMPTING      = 6   # The goal received a cancel request after it started executing
//                               #    and has not yet completed execution
//   uint8 RECALLING       = 7   # The goal received a cancel request before it started executing,
//                               #    but the action server has not yet confirmed that the goal is canceled
//   uint8 RECALLED        = 8   # The goal received a cancel request before it started executing
//                               #    and was successfully cancelled (Terminal State)
//   uint8 LOST            = 9   # An action client can determine that a goal is LOST. This should not be
//                               #    sent over the wire by an action server
//   
//   #Allow for the user to associate a string with GoalStatus for debugging
//   string text
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_actionlib_msgs_GoalStatus_h
#define YARP_ROSMSG_actionlib_msgs_GoalStatus_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/actionlib_msgs/GoalID.h>

namespace yarp {
namespace rosmsg {
namespace actionlib_msgs {

class GoalStatus : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::actionlib_msgs::GoalID goal_id;
    std::uint8_t status;
    static const std::uint8_t PENDING = 0;
    static const std::uint8_t ACTIVE = 1;
    static const std::uint8_t PREEMPTED = 2;
    static const std::uint8_t SUCCEEDED = 3;
    static const std::uint8_t ABORTED = 4;
    static const std::uint8_t REJECTED = 5;
    static const std::uint8_t PREEMPTING = 6;
    static const std::uint8_t RECALLING = 7;
    static const std::uint8_t RECALLED = 8;
    static const std::uint8_t LOST = 9;
    std::string text;

    GoalStatus() :
            goal_id(),
            status(0),
            text("")
    {
    }

    void clear()
    {
        // *** goal_id ***
        goal_id.clear();

        // *** status ***
        status = 0;

        // *** PENDING ***

        // *** ACTIVE ***

        // *** PREEMPTED ***

        // *** SUCCEEDED ***

        // *** ABORTED ***

        // *** REJECTED ***

        // *** PREEMPTING ***

        // *** RECALLING ***

        // *** RECALLED ***

        // *** LOST ***

        // *** text ***
        text = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** goal_id ***
        if (!goal_id.read(connection)) {
            return false;
        }

        // *** status ***
        status = connection.expectInt8();

        // *** text ***
        int len = connection.expectInt32();
        text.resize(len);
        if (!connection.expectBlock((char*)text.c_str(), len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(13)) {
            return false;
        }

        // *** goal_id ***
        if (!goal_id.read(connection)) {
            return false;
        }

        // *** status ***
        status = reader.expectInt8();

        // *** text ***
        if (!reader.readString(text)) {
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
        // *** goal_id ***
        if (!goal_id.write(connection)) {
            return false;
        }

        // *** status ***
        connection.appendInt8(status);

        // *** text ***
        connection.appendInt32(text.length());
        connection.appendExternalBlock((char*)text.c_str(), text.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(13);

        // *** goal_id ***
        if (!goal_id.write(connection)) {
            return false;
        }

        // *** status ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(status);

        // *** text ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(text.length());
        connection.appendExternalBlock((char*)text.c_str(), text.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::actionlib_msgs::GoalStatus> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::actionlib_msgs::GoalStatus> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "actionlib_msgs/GoalStatus";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "d388f9b87b3c471f784434d671988d4a";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
GoalID goal_id\n\
uint8 status\n\
uint8 PENDING         = 0   # The goal has yet to be processed by the action server\n\
uint8 ACTIVE          = 1   # The goal is currently being processed by the action server\n\
uint8 PREEMPTED       = 2   # The goal received a cancel request after it started executing\n\
                            #   and has since completed its execution (Terminal State)\n\
uint8 SUCCEEDED       = 3   # The goal was achieved successfully by the action server (Terminal State)\n\
uint8 ABORTED         = 4   # The goal was aborted during execution by the action server due\n\
                            #    to some failure (Terminal State)\n\
uint8 REJECTED        = 5   # The goal was rejected by the action server without being processed,\n\
                            #    because the goal was unattainable or invalid (Terminal State)\n\
uint8 PREEMPTING      = 6   # The goal received a cancel request after it started executing\n\
                            #    and has not yet completed execution\n\
uint8 RECALLING       = 7   # The goal received a cancel request before it started executing,\n\
                            #    but the action server has not yet confirmed that the goal is canceled\n\
uint8 RECALLED        = 8   # The goal received a cancel request before it started executing\n\
                            #    and was successfully cancelled (Terminal State)\n\
uint8 LOST            = 9   # An action client can determine that a goal is LOST. This should not be\n\
                            #    sent over the wire by an action server\n\
\n\
#Allow for the user to associate a string with GoalStatus for debugging\n\
string text\n\
\n\
\n\
================================================================================\n\
MSG: actionlib_msgs/GoalID\n\
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

#endif // YARP_ROSMSG_actionlib_msgs_GoalStatus_h
