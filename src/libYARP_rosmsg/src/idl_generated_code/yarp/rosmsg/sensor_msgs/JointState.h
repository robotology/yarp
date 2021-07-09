/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/JointState" msg definition:
//   # This is a message that holds data to describe the state of a set of torque controlled joints. 
//   #
//   # The state of each joint (revolute or prismatic) is defined by:
//   #  * the position of the joint (rad or m),
//   #  * the velocity of the joint (rad/s or m/s) and 
//   #  * the effort that is applied in the joint (Nm or N).
//   #
//   # Each joint is uniquely identified by its name
//   # The header specifies the time at which the joint states were recorded. All the joint states
//   # in one message have to be recorded at the same time.
//   #
//   # This message consists of a multiple arrays, one for each part of the joint state. 
//   # The goal is to make each of the fields optional. When e.g. your joints have no
//   # effort associated with them, you can leave the effort array empty. 
//   #
//   # All arrays in this message should have the same size, or be empty.
//   # This is the only way to uniquely associate the joint name with the correct
//   # states.
//   
//   
//   Header header
//   
//   string[] name
//   float64[] position
//   float64[] velocity
//   float64[] effort
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_JointState_h
#define YARP_ROSMSG_sensor_msgs_JointState_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class JointState : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<std::string> name;
    std::vector<yarp::conf::float64_t> position;
    std::vector<yarp::conf::float64_t> velocity;
    std::vector<yarp::conf::float64_t> effort;

    JointState() :
            header(),
            name(),
            position(),
            velocity(),
            effort()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** name ***
        name.clear();

        // *** position ***
        position.clear();

        // *** velocity ***
        velocity.clear();

        // *** effort ***
        effort.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** name ***
        int len = connection.expectInt32();
        name.resize(len);
        for (int i=0; i<len; i++) {
            int len2 = connection.expectInt32();
            name[i].resize(len2);
            if (!connection.expectBlock((char*)name[i].c_str(), len2)) {
                return false;
            }
        }

        // *** position ***
        len = connection.expectInt32();
        position.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&position[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        // *** velocity ***
        len = connection.expectInt32();
        velocity.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&velocity[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        // *** effort ***
        len = connection.expectInt32();
        effort.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&effort[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(5)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** name ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) {
            return false;
        }
        int len = connection.expectInt32();
        name.resize(len);
        for (int i=0; i<len; i++) {
            int len2 = connection.expectInt32();
            name[i].resize(len2);
            if (!connection.expectBlock((char*)name[i].c_str(), len2)) {
                return false;
            }
        }

        // *** position ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        len = connection.expectInt32();
        position.resize(len);
        for (int i=0; i<len; i++) {
            position[i] = (yarp::conf::float64_t)connection.expectFloat64();
        }

        // *** velocity ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        len = connection.expectInt32();
        velocity.resize(len);
        for (int i=0; i<len; i++) {
            velocity[i] = (yarp::conf::float64_t)connection.expectFloat64();
        }

        // *** effort ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        len = connection.expectInt32();
        effort.resize(len);
        for (int i=0; i<len; i++) {
            effort[i] = (yarp::conf::float64_t)connection.expectFloat64();
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

        // *** name ***
        connection.appendInt32(name.size());
        for (size_t i=0; i<name.size(); i++) {
            connection.appendInt32(name[i].length());
            connection.appendExternalBlock((char*)name[i].c_str(), name[i].length());
        }

        // *** position ***
        connection.appendInt32(position.size());
        if (position.size()>0) {
            connection.appendExternalBlock((char*)&position[0], sizeof(yarp::conf::float64_t)*position.size());
        }

        // *** velocity ***
        connection.appendInt32(velocity.size());
        if (velocity.size()>0) {
            connection.appendExternalBlock((char*)&velocity[0], sizeof(yarp::conf::float64_t)*velocity.size());
        }

        // *** effort ***
        connection.appendInt32(effort.size());
        if (effort.size()>0) {
            connection.appendExternalBlock((char*)&effort[0], sizeof(yarp::conf::float64_t)*effort.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(5);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** name ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);
        connection.appendInt32(name.size());
        for (size_t i=0; i<name.size(); i++) {
            connection.appendInt32(name[i].length());
            connection.appendExternalBlock((char*)name[i].c_str(), name[i].length());
        }

        // *** position ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(position.size());
        for (size_t i=0; i<position.size(); i++) {
            connection.appendFloat64(position[i]);
        }

        // *** velocity ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(velocity.size());
        for (size_t i=0; i<velocity.size(); i++) {
            connection.appendFloat64(velocity[i]);
        }

        // *** effort ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(effort.size());
        for (size_t i=0; i<effort.size(); i++) {
            connection.appendFloat64(effort[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::JointState> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::JointState> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/JointState";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "3066dcd76a6cfaef579bd0f34173e9fd";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This is a message that holds data to describe the state of a set of torque controlled joints. \n\
#\n\
# The state of each joint (revolute or prismatic) is defined by:\n\
#  * the position of the joint (rad or m),\n\
#  * the velocity of the joint (rad/s or m/s) and \n\
#  * the effort that is applied in the joint (Nm or N).\n\
#\n\
# Each joint is uniquely identified by its name\n\
# The header specifies the time at which the joint states were recorded. All the joint states\n\
# in one message have to be recorded at the same time.\n\
#\n\
# This message consists of a multiple arrays, one for each part of the joint state. \n\
# The goal is to make each of the fields optional. When e.g. your joints have no\n\
# effort associated with them, you can leave the effort array empty. \n\
#\n\
# All arrays in this message should have the same size, or be empty.\n\
# This is the only way to uniquely associate the joint name with the correct\n\
# states.\n\
\n\
\n\
Header header\n\
\n\
string[] name\n\
float64[] position\n\
float64[] velocity\n\
float64[] effort\n\
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

#endif // YARP_ROSMSG_sensor_msgs_JointState_h
