/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/Imu" msg definition:
//   # This is a message to hold data from an IMU (Inertial Measurement Unit)
//   #
//   # Accelerations should be in m/s^2 (not in g's), and rotational velocity should be in rad/sec
//   #
//   # If the covariance of the measurement is known, it should be filled in (if all you know is the 
//   # variance of each measurement, e.g. from the datasheet, just put those along the diagonal)
//   # A covariance matrix of all zeros will be interpreted as "covariance unknown", and to use the
//   # data a covariance will have to be assumed or gotten from some other source
//   #
//   # If you have no estimate for one of the data elements (e.g. your IMU doesn't produce an orientation 
//   # estimate), please set element 0 of the associated covariance matrix to -1
//   # If you are interpreting this message, please check for a value of -1 in the first element of each 
//   # covariance matrix, and disregard the associated estimate.
//   
//   Header header
//   
//   geometry_msgs/Quaternion orientation
//   float64[9] orientation_covariance # Row major about x, y, z axes
//   
//   geometry_msgs/Vector3 angular_velocity
//   float64[9] angular_velocity_covariance # Row major about x, y, z axes
//   
//   geometry_msgs/Vector3 linear_acceleration
//   float64[9] linear_acceleration_covariance # Row major x, y z 
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_Imu_h
#define YARP_ROSMSG_sensor_msgs_Imu_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Quaternion.h>
#include <yarp/rosmsg/geometry_msgs/Vector3.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class Imu : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::geometry_msgs::Quaternion orientation;
    std::vector<yarp::os::NetFloat64> orientation_covariance;
    yarp::rosmsg::geometry_msgs::Vector3 angular_velocity;
    std::vector<yarp::os::NetFloat64> angular_velocity_covariance;
    yarp::rosmsg::geometry_msgs::Vector3 linear_acceleration;
    std::vector<yarp::os::NetFloat64> linear_acceleration_covariance;

    Imu() :
            header(),
            orientation(),
            orientation_covariance(),
            angular_velocity(),
            angular_velocity_covariance(),
            linear_acceleration(),
            linear_acceleration_covariance()
    {
        orientation_covariance.resize(9, 0.0);
        angular_velocity_covariance.resize(9, 0.0);
        linear_acceleration_covariance.resize(9, 0.0);
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** orientation ***
        orientation.clear();

        // *** orientation_covariance ***
        orientation_covariance.clear();
        orientation_covariance.resize(9, 0.0);

        // *** angular_velocity ***
        angular_velocity.clear();

        // *** angular_velocity_covariance ***
        angular_velocity_covariance.clear();
        angular_velocity_covariance.resize(9, 0.0);

        // *** linear_acceleration ***
        linear_acceleration.clear();

        // *** linear_acceleration_covariance ***
        linear_acceleration_covariance.clear();
        linear_acceleration_covariance.resize(9, 0.0);
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.read(connection)) {
            return false;
        }

        // *** orientation_covariance ***
        int len = 9;
        orientation_covariance.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&orientation_covariance[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** angular_velocity ***
        if (!angular_velocity.read(connection)) {
            return false;
        }

        // *** angular_velocity_covariance ***
        len = 9;
        angular_velocity_covariance.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&angular_velocity_covariance[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        // *** linear_acceleration ***
        if (!linear_acceleration.read(connection)) {
            return false;
        }

        // *** linear_acceleration_covariance ***
        len = 9;
        linear_acceleration_covariance.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&linear_acceleration_covariance[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(7)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.read(connection)) {
            return false;
        }

        // *** orientation_covariance ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        orientation_covariance.resize(len);
        for (int i=0; i<len; i++) {
            orientation_covariance[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** angular_velocity ***
        if (!angular_velocity.read(connection)) {
            return false;
        }

        // *** angular_velocity_covariance ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        len = connection.expectInt();
        angular_velocity_covariance.resize(len);
        for (int i=0; i<len; i++) {
            angular_velocity_covariance[i] = (yarp::os::NetFloat64)connection.expectDouble();
        }

        // *** linear_acceleration ***
        if (!linear_acceleration.read(connection)) {
            return false;
        }

        // *** linear_acceleration_covariance ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        len = connection.expectInt();
        linear_acceleration_covariance.resize(len);
        for (int i=0; i<len; i++) {
            linear_acceleration_covariance[i] = (yarp::os::NetFloat64)connection.expectDouble();
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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.write(connection)) {
            return false;
        }

        // *** orientation_covariance ***
        if (orientation_covariance.size()>0) {
            connection.appendExternalBlock((char*)&orientation_covariance[0], sizeof(yarp::os::NetFloat64)*orientation_covariance.size());
        }

        // *** angular_velocity ***
        if (!angular_velocity.write(connection)) {
            return false;
        }

        // *** angular_velocity_covariance ***
        if (angular_velocity_covariance.size()>0) {
            connection.appendExternalBlock((char*)&angular_velocity_covariance[0], sizeof(yarp::os::NetFloat64)*angular_velocity_covariance.size());
        }

        // *** linear_acceleration ***
        if (!linear_acceleration.write(connection)) {
            return false;
        }

        // *** linear_acceleration_covariance ***
        if (linear_acceleration_covariance.size()>0) {
            connection.appendExternalBlock((char*)&linear_acceleration_covariance[0], sizeof(yarp::os::NetFloat64)*linear_acceleration_covariance.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(7);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.write(connection)) {
            return false;
        }

        // *** orientation_covariance ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(orientation_covariance.size());
        for (size_t i=0; i<orientation_covariance.size(); i++) {
            connection.appendDouble((double)orientation_covariance[i]);
        }

        // *** angular_velocity ***
        if (!angular_velocity.write(connection)) {
            return false;
        }

        // *** angular_velocity_covariance ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(angular_velocity_covariance.size());
        for (size_t i=0; i<angular_velocity_covariance.size(); i++) {
            connection.appendDouble((double)angular_velocity_covariance[i]);
        }

        // *** linear_acceleration ***
        if (!linear_acceleration.write(connection)) {
            return false;
        }

        // *** linear_acceleration_covariance ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(linear_acceleration_covariance.size());
        for (size_t i=0; i<linear_acceleration_covariance.size(); i++) {
            connection.appendDouble((double)linear_acceleration_covariance[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::Imu> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::Imu> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
# This is a message to hold data from an IMU (Inertial Measurement Unit)\n\
#\n\
# Accelerations should be in m/s^2 (not in g's), and rotational velocity should be in rad/sec\n\
#\n\
# If the covariance of the measurement is known, it should be filled in (if all you know is the \n\
# variance of each measurement, e.g. from the datasheet, just put those along the diagonal)\n\
# A covariance matrix of all zeros will be interpreted as \"covariance unknown\", and to use the\n\
# data a covariance will have to be assumed or gotten from some other source\n\
#\n\
# If you have no estimate for one of the data elements (e.g. your IMU doesn't produce an orientation \n\
# estimate), please set element 0 of the associated covariance matrix to -1\n\
# If you are interpreting this message, please check for a value of -1 in the first element of each \n\
# covariance matrix, and disregard the associated estimate.\n\
\n\
Header header\n\
\n\
geometry_msgs/Quaternion orientation\n\
float64[9] orientation_covariance # Row major about x, y, z axes\n\
\n\
geometry_msgs/Vector3 angular_velocity\n\
float64[9] angular_velocity_covariance # Row major about x, y, z axes\n\
\n\
geometry_msgs/Vector3 linear_acceleration\n\
float64[9] linear_acceleration_covariance # Row major x, y z \n\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
") + yarp::rosmsg::geometry_msgs::Quaternion::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
") + yarp::rosmsg::geometry_msgs::Vector3::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::Imu::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/Imu", "sensor_msgs/Imu");
        typ.addProperty("md5sum", yarp::os::Value("6a62c6daae103f4ff57a132d6f95cec2"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_Imu_h
