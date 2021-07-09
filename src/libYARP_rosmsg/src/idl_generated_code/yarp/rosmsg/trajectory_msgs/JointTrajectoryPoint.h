/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "trajectory_msgs/JointTrajectoryPoint" msg definition:
//   # Each trajectory point specifies either positions[, velocities[, accelerations]]
//   # or positions[, effort] for the trajectory to be executed.
//   # All specified values are in the same order as the joint names in JointTrajectory.msg
//   
//   float64[] positions
//   float64[] velocities
//   float64[] accelerations
//   float64[] effort
//   duration time_from_start
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_trajectory_msgs_JointTrajectoryPoint_h
#define YARP_ROSMSG_trajectory_msgs_JointTrajectoryPoint_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/TickDuration.h>

namespace yarp {
namespace rosmsg {
namespace trajectory_msgs {

class JointTrajectoryPoint : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::conf::float64_t> positions;
    std::vector<yarp::conf::float64_t> velocities;
    std::vector<yarp::conf::float64_t> accelerations;
    std::vector<yarp::conf::float64_t> effort;
    yarp::rosmsg::TickDuration time_from_start;

    JointTrajectoryPoint() :
            positions(),
            velocities(),
            accelerations(),
            effort(),
            time_from_start()
    {
    }

    void clear()
    {
        // *** positions ***
        positions.clear();

        // *** velocities ***
        velocities.clear();

        // *** accelerations ***
        accelerations.clear();

        // *** effort ***
        effort.clear();

        // *** time_from_start ***
        time_from_start.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** positions ***
        int len = connection.expectInt32();
        positions.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&positions[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        // *** velocities ***
        len = connection.expectInt32();
        velocities.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&velocities[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        // *** accelerations ***
        len = connection.expectInt32();
        accelerations.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&accelerations[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        // *** effort ***
        len = connection.expectInt32();
        effort.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&effort[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
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
        if (!reader.readListHeader(5)) {
            return false;
        }

        // *** positions ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        int len = connection.expectInt32();
        positions.resize(len);
        for (int i=0; i<len; i++) {
            positions[i] = (yarp::conf::float64_t)connection.expectFloat64();
        }

        // *** velocities ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        len = connection.expectInt32();
        velocities.resize(len);
        for (int i=0; i<len; i++) {
            velocities[i] = (yarp::conf::float64_t)connection.expectFloat64();
        }

        // *** accelerations ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        len = connection.expectInt32();
        accelerations.resize(len);
        for (int i=0; i<len; i++) {
            accelerations[i] = (yarp::conf::float64_t)connection.expectFloat64();
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
        // *** positions ***
        connection.appendInt32(positions.size());
        if (positions.size()>0) {
            connection.appendExternalBlock((char*)&positions[0], sizeof(yarp::conf::float64_t)*positions.size());
        }

        // *** velocities ***
        connection.appendInt32(velocities.size());
        if (velocities.size()>0) {
            connection.appendExternalBlock((char*)&velocities[0], sizeof(yarp::conf::float64_t)*velocities.size());
        }

        // *** accelerations ***
        connection.appendInt32(accelerations.size());
        if (accelerations.size()>0) {
            connection.appendExternalBlock((char*)&accelerations[0], sizeof(yarp::conf::float64_t)*accelerations.size());
        }

        // *** effort ***
        connection.appendInt32(effort.size());
        if (effort.size()>0) {
            connection.appendExternalBlock((char*)&effort[0], sizeof(yarp::conf::float64_t)*effort.size());
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
        connection.appendInt32(5);

        // *** positions ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(positions.size());
        for (size_t i=0; i<positions.size(); i++) {
            connection.appendFloat64(positions[i]);
        }

        // *** velocities ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(velocities.size());
        for (size_t i=0; i<velocities.size(); i++) {
            connection.appendFloat64(velocities[i]);
        }

        // *** accelerations ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(accelerations.size());
        for (size_t i=0; i<accelerations.size(); i++) {
            connection.appendFloat64(accelerations[i]);
        }

        // *** effort ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(effort.size());
        for (size_t i=0; i<effort.size(); i++) {
            connection.appendFloat64(effort[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::trajectory_msgs::JointTrajectoryPoint> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::trajectory_msgs::JointTrajectoryPoint> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "trajectory_msgs/JointTrajectoryPoint";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "f3cd1e1c4d320c79d6985c904ae5dcd3";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Each trajectory point specifies either positions[, velocities[, accelerations]]\n\
# or positions[, effort] for the trajectory to be executed.\n\
# All specified values are in the same order as the joint names in JointTrajectory.msg\n\
\n\
float64[] positions\n\
float64[] velocities\n\
float64[] accelerations\n\
float64[] effort\n\
duration time_from_start\n\
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

#endif // YARP_ROSMSG_trajectory_msgs_JointTrajectoryPoint_h
