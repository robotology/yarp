/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "PoseStampedRosPublisher.h"
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

YARP_LOG_COMPONENT(GENERICSENSORROSPUBLISHER, "yarp.device.PoseStampedRosPublisher")

bool PoseStampedRosPublisher::viewInterfaces()
{
    // View all the interfaces
    bool ok = true;
    ok &= m_poly->view(m_iOrientationSensors);
    ok &= m_poly->view(m_iPositionSensors);
    m_iPositionSensors->getPositionSensorFrameName(m_sens_index, m_framename);
    return ok;
}

void PoseStampedRosPublisher::run()
{
    if (m_publisher.asPort().isOpen())
    {
        yarp::sig::Vector vecpos(3);
        yarp::sig::Vector vecrpy(3);
        yarp::rosmsg::geometry_msgs::PoseStamped& pose_data = m_publisher.prepare();
        m_iPositionSensors->getPositionSensorMeasure(m_sens_index, vecpos, m_timestamp);
        m_iOrientationSensors->getOrientationSensorMeasureAsRollPitchYaw(m_sens_index, vecrpy, m_timestamp);
        pose_data.clear();
        pose_data.header.frame_id = m_framename;
        pose_data.header.seq = m_msg_counter++;
        pose_data.header.stamp = m_timestamp;
        pose_data.pose.position.x = vecpos[0];
        pose_data.pose.position.y = vecpos[1];
        pose_data.pose.position.z = vecpos[2];
        vecrpy[0] = vecrpy[0] * M_PI / 180.0;
        vecrpy[1] = vecrpy[1] * M_PI / 180.0;
        vecrpy[2] = vecrpy[2] * M_PI / 180.0;
        yarp::sig::Matrix matrix = yarp::math::rpy2dcm(vecrpy);
        yarp::math::Quaternion q; q.fromRotationMatrix(matrix);
        pose_data.pose.orientation.x = q.x();
        pose_data.pose.orientation.y = q.y();
        pose_data.pose.orientation.z = q.z();
        pose_data.pose.orientation.w = q.w();
        //imu_ros_data.orientation_covariance = 0;
        m_publisher.write();
    }
 }
