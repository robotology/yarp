/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "WrenchStampedRosPublisher.h"

YARP_LOG_COMPONENT(GENERICSENSORROSPUBLISHER, "yarp.device.WrenchStampedRosPublisher")

bool WrenchStampedRosPublisher::viewInterfaces()
{
    // View all the interfaces
    bool ok = m_poly->view(m_iFTsens);
    m_iFTsens->getSixAxisForceTorqueSensorFrameName(m_sens_index, m_framename);
    return ok;
}

void WrenchStampedRosPublisher::run()
{
    if (m_publisher.asPort().isOpen())
    {
        yarp::sig::Vector vecwrench(6);
        yarp::rosmsg::geometry_msgs::WrenchStamped& wrench_ros_data = m_publisher.prepare();
        m_iFTsens->getSixAxisForceTorqueSensorMeasure(m_sens_index, vecwrench, m_timestamp);
        wrench_ros_data.clear();
        wrench_ros_data.header.frame_id = m_framename;
        wrench_ros_data.header.seq = m_msg_counter++;
        wrench_ros_data.header.stamp = m_timestamp;
        wrench_ros_data.wrench.force.x = vecwrench[0];
        wrench_ros_data.wrench.force.y = vecwrench[1];
        wrench_ros_data.wrench.force.z = vecwrench[2];
        wrench_ros_data.wrench.torque.x = vecwrench[4];
        wrench_ros_data.wrench.torque.y = vecwrench[5];
        wrench_ros_data.wrench.torque.z = vecwrench[6];
        m_publisher.write();
    }
}
