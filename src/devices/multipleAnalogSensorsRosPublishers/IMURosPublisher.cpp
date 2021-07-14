/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "IMURosPublisher.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

YARP_LOG_COMPONENT(GENERICSENSORROSPUBLISHER, "yarp.device.IMURosPublisher")

bool IMURosPublisher::viewInterfaces()
{
    // View all the interfaces
    bool ok = true;
    ok &= m_poly->view(m_iThreeAxisGyroscopes);
    ok &= m_poly->view(m_iThreeAxisLinearAccelerometers);
    ok &= m_poly->view(m_iThreeAxisMagnetometers);
    ok &= m_poly->view(m_iOrientationSensors);
    if (m_iThreeAxisGyroscopes) {
        m_iThreeAxisGyroscopes->getThreeAxisGyroscopeFrameName(m_sens_index, m_framename);
    }
    return ok;
}

void IMURosPublisher::run()
{
    if (m_publisher.asPort().isOpen())
    {
        yarp::sig::Vector vecgyr(3);
        yarp::sig::Vector vecacc(3);
        yarp::sig::Vector vecrpy(3);
        yarp::rosmsg::sensor_msgs::Imu& imu_ros_data = m_publisher.prepare();
        m_iThreeAxisGyroscopes->getThreeAxisGyroscopeMeasure(m_sens_index, vecgyr, m_timestamp);
        m_iThreeAxisLinearAccelerometers->getThreeAxisLinearAccelerometerMeasure(m_sens_index, vecacc, m_timestamp);
        m_iOrientationSensors->getOrientationSensorMeasureAsRollPitchYaw(m_sens_index, vecrpy, m_timestamp);
        imu_ros_data.clear();
        imu_ros_data.header.frame_id = m_framename;
        imu_ros_data.header.seq = m_msg_counter++;
        imu_ros_data.header.stamp = m_timestamp;
        imu_ros_data.angular_velocity.x = vecgyr[0] * M_PI / 180.0;
        imu_ros_data.angular_velocity.y = vecgyr[1] * M_PI / 180.0;
        imu_ros_data.angular_velocity.z = vecgyr[2] * M_PI / 180.0;
        imu_ros_data.linear_acceleration.x = vecacc[0];
        imu_ros_data.linear_acceleration.y = vecacc[1];
        imu_ros_data.linear_acceleration.z = vecacc[2];
        imu_ros_data.orientation.x = vecrpy[0] * M_PI / 180.0;
        imu_ros_data.orientation.y = vecrpy[1] * M_PI / 180.0;
        imu_ros_data.orientation.z = vecrpy[2] * M_PI / 180.0;
        //imu_ros_data.orientation_covariance = 0;
        m_publisher.write();
    }
 }
