/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/MultiDOFJointState" msg definition:
//   # Representation of state for joints with multiple degrees of freedom, 
//   # following the structure of JointState.
//   #
//   # It is assumed that a joint in a system corresponds to a transform that gets applied 
//   # along the kinematic chain. For example, a planar joint (as in URDF) is 3DOF (x, y, yaw)
//   # and those 3DOF can be expressed as a transformation matrix, and that transformation
//   # matrix can be converted back to (x, y, yaw)
//   #
//   # Each joint is uniquely identified by its name
//   # The header specifies the time at which the joint states were recorded. All the joint states
//   # in one message have to be recorded at the same time.
//   #
//   # This message consists of a multiple arrays, one for each part of the joint state. 
//   # The goal is to make each of the fields optional. When e.g. your joints have no
//   # wrench associated with them, you can leave the wrench array empty. 
//   #
//   # All arrays in this message should have the same size, or be empty.
//   # This is the only way to uniquely associate the joint name with the correct
//   # states.
//   
//   Header header
//   
//   string[] joint_names
//   geometry_msgs/Transform[] transforms
//   geometry_msgs/Twist[] twist
//   geometry_msgs/Wrench[] wrench
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_MultiDOFJointState_h
#define YARP_ROSMSG_sensor_msgs_MultiDOFJointState_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Transform.h>
#include <yarp/rosmsg/geometry_msgs/Twist.h>
#include <yarp/rosmsg/geometry_msgs/Wrench.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class MultiDOFJointState : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<std::string> joint_names;
    std::vector<yarp::rosmsg::geometry_msgs::Transform> transforms;
    std::vector<yarp::rosmsg::geometry_msgs::Twist> twist;
    std::vector<yarp::rosmsg::geometry_msgs::Wrench> wrench;

    MultiDOFJointState() :
            header(),
            joint_names(),
            transforms(),
            twist(),
            wrench()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** joint_names ***
        joint_names.clear();

        // *** transforms ***
        transforms.clear();

        // *** twist ***
        twist.clear();

        // *** wrench ***
        wrench.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** joint_names ***
        int len = connection.expectInt32();
        joint_names.resize(len);
        for (int i=0; i<len; i++) {
            int len2 = connection.expectInt32();
            joint_names[i].resize(len2);
            if (!connection.expectBlock((char*)joint_names[i].c_str(), len2)) {
                return false;
            }
        }

        // *** transforms ***
        len = connection.expectInt32();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
                return false;
            }
        }

        // *** twist ***
        len = connection.expectInt32();
        twist.resize(len);
        for (int i=0; i<len; i++) {
            if (!twist[i].read(connection)) {
                return false;
            }
        }

        // *** wrench ***
        len = connection.expectInt32();
        wrench.resize(len);
        for (int i=0; i<len; i++) {
            if (!wrench[i].read(connection)) {
                return false;
            }
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

        // *** joint_names ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) {
            return false;
        }
        int len = connection.expectInt32();
        joint_names.resize(len);
        for (int i=0; i<len; i++) {
            int len2 = connection.expectInt32();
            joint_names[i].resize(len2);
            if (!connection.expectBlock((char*)joint_names[i].c_str(), len2)) {
                return false;
            }
        }

        // *** transforms ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
                return false;
            }
        }

        // *** twist ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        twist.resize(len);
        for (int i=0; i<len; i++) {
            if (!twist[i].read(connection)) {
                return false;
            }
        }

        // *** wrench ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        wrench.resize(len);
        for (int i=0; i<len; i++) {
            if (!wrench[i].read(connection)) {
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

        // *** joint_names ***
        connection.appendInt32(joint_names.size());
        for (size_t i=0; i<joint_names.size(); i++) {
            connection.appendInt32(joint_names[i].length());
            connection.appendExternalBlock((char*)joint_names[i].c_str(), joint_names[i].length());
        }

        // *** transforms ***
        connection.appendInt32(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        // *** twist ***
        connection.appendInt32(twist.size());
        for (size_t i=0; i<twist.size(); i++) {
            if (!twist[i].write(connection)) {
                return false;
            }
        }

        // *** wrench ***
        connection.appendInt32(wrench.size());
        for (size_t i=0; i<wrench.size(); i++) {
            if (!wrench[i].write(connection)) {
                return false;
            }
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

        // *** joint_names ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);
        connection.appendInt32(joint_names.size());
        for (size_t i=0; i<joint_names.size(); i++) {
            connection.appendInt32(joint_names[i].length());
            connection.appendExternalBlock((char*)joint_names[i].c_str(), joint_names[i].length());
        }

        // *** transforms ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        // *** twist ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(twist.size());
        for (size_t i=0; i<twist.size(); i++) {
            if (!twist[i].write(connection)) {
                return false;
            }
        }

        // *** wrench ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(wrench.size());
        for (size_t i=0; i<wrench.size(); i++) {
            if (!wrench[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::MultiDOFJointState> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::MultiDOFJointState> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/MultiDOFJointState";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "690f272f0640d2631c305eeb8301e59d";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Representation of state for joints with multiple degrees of freedom, \n\
# following the structure of JointState.\n\
#\n\
# It is assumed that a joint in a system corresponds to a transform that gets applied \n\
# along the kinematic chain. For example, a planar joint (as in URDF) is 3DOF (x, y, yaw)\n\
# and those 3DOF can be expressed as a transformation matrix, and that transformation\n\
# matrix can be converted back to (x, y, yaw)\n\
#\n\
# Each joint is uniquely identified by its name\n\
# The header specifies the time at which the joint states were recorded. All the joint states\n\
# in one message have to be recorded at the same time.\n\
#\n\
# This message consists of a multiple arrays, one for each part of the joint state. \n\
# The goal is to make each of the fields optional. When e.g. your joints have no\n\
# wrench associated with them, you can leave the wrench array empty. \n\
#\n\
# All arrays in this message should have the same size, or be empty.\n\
# This is the only way to uniquely associate the joint name with the correct\n\
# states.\n\
\n\
Header header\n\
\n\
string[] joint_names\n\
geometry_msgs/Transform[] transforms\n\
geometry_msgs/Twist[] twist\n\
geometry_msgs/Wrench[] wrench\n\
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
\n\
================================================================================\n\
MSG: geometry_msgs/Transform\n\
# This represents the transform between two coordinate frames in free space.\n\
\n\
Vector3 translation\n\
Quaternion rotation\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
# This represents a vector in free space. \n\
# It is only meant to represent a direction. Therefore, it does not\n\
# make sense to apply a translation to it (e.g., when applying a \n\
# generic rigid transformation to a Vector3, tf2 will only apply the\n\
# rotation). If you want your data to be translatable too, use the\n\
# geometry_msgs/Point message instead.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Twist\n\
# This expresses velocity in free space broken into its linear and angular parts.\n\
Vector3  linear\n\
Vector3  angular\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Wrench\n\
# This represents force in free space, separated into\n\
# its linear and angular parts.\n\
Vector3  force\n\
Vector3  torque\n\
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

#endif // YARP_ROSMSG_sensor_msgs_MultiDOFJointState_h
