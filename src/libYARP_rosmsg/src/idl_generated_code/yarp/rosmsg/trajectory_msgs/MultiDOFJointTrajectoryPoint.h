/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "trajectory_msgs/MultiDOFJointTrajectoryPoint" msg definition:
//   # Each multi-dof joint can specify a transform (up to 6 DOF)
//   geometry_msgs/Transform[] transforms
//   
//   # There can be a velocity specified for the origin of the joint 
//   geometry_msgs/Twist[] velocities
//   
//   # There can be an acceleration specified for the origin of the joint 
//   geometry_msgs/Twist[] accelerations
//   
//   duration time_from_start
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectoryPoint_h
#define YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectoryPoint_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Transform.h>
#include <yarp/rosmsg/geometry_msgs/Twist.h>
#include <yarp/rosmsg/TickDuration.h>

namespace yarp {
namespace rosmsg {
namespace trajectory_msgs {

class MultiDOFJointTrajectoryPoint : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::rosmsg::geometry_msgs::Transform> transforms;
    std::vector<yarp::rosmsg::geometry_msgs::Twist> velocities;
    std::vector<yarp::rosmsg::geometry_msgs::Twist> accelerations;
    yarp::rosmsg::TickDuration time_from_start;

    MultiDOFJointTrajectoryPoint() :
            transforms(),
            velocities(),
            accelerations(),
            time_from_start()
    {
    }

    void clear()
    {
        // *** transforms ***
        transforms.clear();

        // *** velocities ***
        velocities.clear();

        // *** accelerations ***
        accelerations.clear();

        // *** time_from_start ***
        time_from_start.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** transforms ***
        int len = connection.expectInt32();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
                return false;
            }
        }

        // *** velocities ***
        len = connection.expectInt32();
        velocities.resize(len);
        for (int i=0; i<len; i++) {
            if (!velocities[i].read(connection)) {
                return false;
            }
        }

        // *** accelerations ***
        len = connection.expectInt32();
        accelerations.resize(len);
        for (int i=0; i<len; i++) {
            if (!accelerations[i].read(connection)) {
                return false;
            }
        }

        // *** time_from_start ***
        if (!time_from_start.read(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(4)) {
            return false;
        }

        // *** transforms ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
                return false;
            }
        }

        // *** velocities ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        velocities.resize(len);
        for (int i=0; i<len; i++) {
            if (!velocities[i].read(connection)) {
                return false;
            }
        }

        // *** accelerations ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        accelerations.resize(len);
        for (int i=0; i<len; i++) {
            if (!accelerations[i].read(connection)) {
                return false;
            }
        }

        // *** time_from_start ***
        if (!time_from_start.read(connection)) {
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
        // *** transforms ***
        connection.appendInt32(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        // *** velocities ***
        connection.appendInt32(velocities.size());
        for (size_t i=0; i<velocities.size(); i++) {
            if (!velocities[i].write(connection)) {
                return false;
            }
        }

        // *** accelerations ***
        connection.appendInt32(accelerations.size());
        for (size_t i=0; i<accelerations.size(); i++) {
            if (!accelerations[i].write(connection)) {
                return false;
            }
        }

        // *** time_from_start ***
        if (!time_from_start.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(4);

        // *** transforms ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        // *** velocities ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(velocities.size());
        for (size_t i=0; i<velocities.size(); i++) {
            if (!velocities[i].write(connection)) {
                return false;
            }
        }

        // *** accelerations ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(accelerations.size());
        for (size_t i=0; i<accelerations.size(); i++) {
            if (!accelerations[i].write(connection)) {
                return false;
            }
        }

        // *** time_from_start ***
        if (!time_from_start.write(connection)) {
            return false;
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectoryPoint> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectoryPoint> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "trajectory_msgs/MultiDOFJointTrajectoryPoint";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "3ebe08d1abd5b65862d50e09430db776";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
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

#endif // YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectoryPoint_h
