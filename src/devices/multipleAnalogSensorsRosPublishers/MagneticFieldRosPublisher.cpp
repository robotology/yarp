/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MagneticFieldRosPublisher.h"

YARP_LOG_COMPONENT(GENERICSENSORROSPUBLISHER, "yarp.device.MagneticFieldRosPublisher")

bool MagneticFieldRosPublisher::viewInterfaces()
{
    // View all the interfaces
    bool ok = true;
    ok &= m_poly->view(m_iThreeAxisMagnetometers);
    m_iThreeAxisMagnetometers->getThreeAxisMagnetometerFrameName(0, m_framename);
    return ok;
}

void MagneticFieldRosPublisher::run()
{
    if (m_publisher.asPort().isOpen())
    {
        yarp::sig::Vector vecmagn(3);
        yarp::rosmsg::sensor_msgs::MagneticField& magfield_ros_data = m_publisher.prepare();
        m_iThreeAxisMagnetometers->getThreeAxisMagnetometerMeasure(m_sens_index, vecmagn, m_timestamp);
        magfield_ros_data.clear();
        magfield_ros_data.header.frame_id = m_framename;;
        magfield_ros_data.header.seq = m_msg_counter++;
        magfield_ros_data.header.stamp = m_timestamp;
        magfield_ros_data.magnetic_field.x = vecmagn[0];
        magfield_ros_data.magnetic_field.y = vecmagn[1];
        magfield_ros_data.magnetic_field.z = vecmagn[2];
        //magfield_ros_data.magnetic_field_covariance = 0;
        m_publisher.write();
    }
 }
