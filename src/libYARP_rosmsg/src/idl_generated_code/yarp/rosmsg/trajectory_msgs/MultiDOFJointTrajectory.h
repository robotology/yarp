/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "trajectory_msgs/MultiDOFJointTrajectory" msg definition:
//   # The header is used to specify the coordinate frame and the reference time for the trajectory durations
//   Header header
//   
//   # A representation of a multi-dof joint trajectory (each point is a transformation)
//   # Each point along the trajectory will include an array of positions/velocities/accelerations
//   # that has the same length as the array of joint names, and has the same order of joints as 
//   # the joint names array.
//   
//   string[] joint_names
//   MultiDOFJointTrajectoryPoint[] points
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectory_h
#define YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectory_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/trajectory_msgs/MultiDOFJointTrajectoryPoint.h>

namespace yarp {
namespace rosmsg {
namespace trajectory_msgs {

class MultiDOFJointTrajectory : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<std::string> joint_names;
    std::vector<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectoryPoint> points;

    MultiDOFJointTrajectory() :
            header(),
            joint_names(),
            points()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** joint_names ***
        joint_names.clear();

        // *** points ***
        points.clear();
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

        // *** points ***
        len = connection.expectInt32();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
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

        // *** points ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
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

        // *** points ***
        connection.appendInt32(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

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

        // *** joint_names ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);
        connection.appendInt32(joint_names.size());
        for (size_t i=0; i<joint_names.size(); i++) {
            connection.appendInt32(joint_names[i].length());
            connection.appendExternalBlock((char*)joint_names[i].c_str(), joint_names[i].length());
        }

        // *** points ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectory> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectory> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "trajectory_msgs/MultiDOFJointTrajectory";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "ef145a45a5f47b77b7f5cdde4b16c942";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# The header is used to specify the coordinate frame and the reference time for the trajectory durations\n\
Header header\n\
\n\
# A representation of a multi-dof joint trajectory (each point is a transformation)\n\
# Each point along the trajectory will include an array of positions/velocities/accelerations\n\
# that has the same length as the array of joint names, and has the same order of joints as \n\
# the joint names array.\n\
\n\
string[] joint_names\n\
MultiDOFJointTrajectoryPoint[] points\n\
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
MSG: trajectory_msgs/MultiDOFJointTrajectoryPoint\n\
# Each multi-dof joint can specify a transform (up to 6 DOF)\n\
geometry_msgs/Transform[] transforms\n\
\n\
# There can be a velocity specified for the origin of the joint \n\
geometry_msgs/Twist[] velocities\n\
\n\
# There can be an acceleration specified for the origin of the joint \n\
geometry_msgs/Twist[] accelerations\n\
\n\
duration time_from_start\n\
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
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace trajectory_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectory_h
