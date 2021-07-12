/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TemperatureRosPublisher.h"

YARP_LOG_COMPONENT(GENERICSENSORROSPUBLISHER, "yarp.device.PoseStampedRosPublisher")

bool TemperatureRosPublisher::viewInterfaces()
{
    // View all the interfaces
    bool ok = true;
    ok &= m_poly->view(m_ITemperature);
    m_ITemperature->getTemperatureSensorFrameName(0,m_framename);
    return ok;
}

void TemperatureRosPublisher::run()
{
    if (m_publisher.asPort().isOpen())
    {
        size_t index = 0;
        double temperature;
        yarp::rosmsg::sensor_msgs::Temperature& temp_ros_data = m_publisher.prepare();
        m_ITemperature->getTemperatureSensorMeasure(index, temperature, m_timestamp);
        temp_ros_data.clear();
        temp_ros_data.header.frame_id = m_framename;
        temp_ros_data.header.seq = m_msg_counter++;
        temp_ros_data.header.stamp = m_timestamp;
        temp_ros_data.temperature = temperature;
        temp_ros_data.variance = 0;
        m_publisher.write();
    }
 }
