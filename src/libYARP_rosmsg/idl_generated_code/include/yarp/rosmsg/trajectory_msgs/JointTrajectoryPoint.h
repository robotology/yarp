/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    std::vector<yarp::os::NetFloat64> positions;
    std::vector<yarp::os::NetFloat64> velocities;
    std::vector<yarp::os::NetFloat64> accelerations;
    std::vector<yarp::os::NetFloat64> effort;
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
        int len = connection.expectInt();
        positions.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&positions[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** velocities ***
        len = connection.expectInt();
        velocities.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&velocities[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** accelerations ***
        len = connection.expectInt();
        accelerations.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&accelerations[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** effort ***
        len = connection.expectInt();
        effort.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&effort[0], sizeof(yarp::os::NetFloat64)*len)) {
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
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        positions.resize(len);
        for (int i=0; i<len; i++) {
            positions[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** velocities ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        len = connection.expectInt();
        velocities.resize(len);
        for (int i=0; i<len; i++) {
            velocities[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** accelerations ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        len = connection.expectInt();
        accelerations.resize(len);
        for (int i=0; i<len; i++) {
            accelerations[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** effort ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        len = connection.expectInt();
        effort.resize(len);
        for (int i=0; i<len; i++) {
            effort[i] = (yarp::os::NetFloat64)connection.expectDouble();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** positions ***
        connection.appendInt(positions.size());
        if (positions.size()>0) {
            connection.appendExternalBlock((char*)&positions[0], sizeof(yarp::os::NetFloat64)*positions.size());
        }

        // *** velocities ***
        connection.appendInt(velocities.size());
        if (velocities.size()>0) {
            connection.appendExternalBlock((char*)&velocities[0], sizeof(yarp::os::NetFloat64)*velocities.size());
        }

        // *** accelerations ***
        connection.appendInt(accelerations.size());
        if (accelerations.size()>0) {
            connection.appendExternalBlock((char*)&accelerations[0], sizeof(yarp::os::NetFloat64)*accelerations.size());
        }

        // *** effort ***
        connection.appendInt(effort.size());
        if (effort.size()>0) {
            connection.appendExternalBlock((char*)&effort[0], sizeof(yarp::os::NetFloat64)*effort.size());
        }

        // *** time_from_start ***
        if (!time_from_start.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(5);

        // *** positions ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(positions.size());
        for (size_t i=0; i<positions.size(); i++) {
            connection.appendDouble((double)positions[i]);
        }

        // *** velocities ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(velocities.size());
        for (size_t i=0; i<velocities.size(); i++) {
            connection.appendDouble((double)velocities[i]);
        }

        // *** accelerations ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(accelerations.size());
        for (size_t i=0; i<accelerations.size(); i++) {
            connection.appendDouble((double)accelerations[i]);
        }

        // *** effort ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(effort.size());
        for (size_t i=0; i<effort.size(); i++) {
            connection.appendDouble((double)effort[i]);
        }

        // *** time_from_start ***
        if (!time_from_start.write(connection)) {
            return false;
        }

        connection.convertTextMode();
        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::write;
    bool write(yarp::os::ConnectionWriter& connection) override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::trajectory_msgs::JointTrajectoryPoint> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::trajectory_msgs::JointTrajectoryPoint> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
# Each trajectory point specifies either positions[, velocities[, accelerations]]\n\
# or positions[, effort] for the trajectory to be executed.\n\
# All specified values are in the same order as the joint names in JointTrajectory.msg\n\
\n\
float64[] positions\n\
float64[] velocities\n\
float64[] accelerations\n\
float64[] effort\n\
duration time_from_start\n\
");
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::trajectory_msgs::JointTrajectoryPoint::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("trajectory_msgs/JointTrajectoryPoint", "trajectory_msgs/JointTrajectoryPoint");
        typ.addProperty("md5sum", yarp::os::Value("32aa46c1b23c0fd4d41578b2d62f37c5"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace trajectory_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_trajectory_msgs_JointTrajectoryPoint_h
