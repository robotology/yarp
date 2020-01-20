/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "realsense2Tracking.h"

#include <yarp/os/Value.h>

#include <yarp/sig/ImageUtils.h>

#include <yarp/math/quaternion.h>
#include <yarp/math/math.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <librealsense2/rsutil.h>
#include <librealsense2/rs.hpp>
#include <mutex>

 /**********************************************************************************************************/
 // This software module is experimental.
 // It is provided with uncomplete documentation and it may be modified/renamed/removed without any notice.
 /**********************************************************************************************************/

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct float3 {
    float x, y, z;
    float3 operator*(float t)
    {
        return { x * t, y * t, z * t };
    }

    float3 operator-(float t)
    {
        return { x - t, y - t, z - t };
    }

    void operator*=(float t)
    {
        x = x * t;
        y = y * t;
        z = z * t;
    }

    void operator=(float3 other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    void add(float t1, float t2, float t3)
    {
        x += t1;
        y += t2;
        z += t3;
    }
};

//---------------------------------------------------------------

static std::string get_device_information(const rs2::device& dev)
{

    std::stringstream ss;
    ss << "Device information: " << std::endl;

    for (int i = 0; i < static_cast<int>(RS2_CAMERA_INFO_COUNT); i++) {
        auto info_type = static_cast<rs2_camera_info>(i);
        ss << "  " << std::left << std::setw(20) << info_type << " : ";

        if (dev.supports(info_type))
            ss << dev.get_info(info_type) << std::endl;
        else
            ss << "N/A" << std::endl;
    }
    return ss.str();
}

static bool setOption(rs2_option option, const rs2::sensor* sensor, float value)
{

    if (!sensor) {
        return false;
    }

    // First, verify that the sensor actually supports this option
    if (!sensor->supports(option)) {
        yError() << "realsense2Driver: The option" << rs2_option_to_string(option) << "is not supported by this sensor";
        return false;
    }

    // To set an option to a different value, we can call set_option with a new value
    try {
        sensor->set_option(option, value);
    } catch (const rs2::error& e) {
        // Some options can only be set while the camera is streaming,
        // and generally the hardware might fail so it is good practice to catch exceptions from set_option
        yError() << "realsense2Driver: Failed to set option " << rs2_option_to_string(option) << ". (" << e.what() << ")";
        return false;
    }
    return true;
}

static bool getOption(rs2_option option, const rs2::sensor* sensor, float& value)
{
    if (!sensor) {
        return false;
    }

    // First, verify that the sensor actually supports this option
    if (!sensor->supports(option)) {
        yError() << "realsense2Driver: The option" << rs2_option_to_string(option) << "is not supported by this sensor";
        return false;
    }

    // To set an option to a different value, we can call set_option with a new value
    try {
        value = sensor->get_option(option);
    } catch (const rs2::error& e) {
        // Some options can only be set while the camera is streaming,
        // and generally the hardware might fail so it is good practice to catch exceptions from set_option
        yError() << "realsense2Driver: Failed to get option " << rs2_option_to_string(option) << ". (" << e.what() << ")";
        return false;
    }
    return true;
}

static void settingErrorMsg(const string& error, bool& ret)
{
    yError() << "realsense2Driver:" << error.c_str();
    ret = false;
}

realsense2Tracking::realsense2Tracking()
{
}

bool realsense2Tracking::pipelineStartup()
{
    try {
        m_profile = m_pipeline.start(m_cfg);
    } catch (const rs2::error& e) {
        yError() << "realsense2Driver: failed to start the pipeline:"
                 << "(" << e.what() << ")";
        m_lastError = e.what();
        return false;
    }
    return true;
}

bool realsense2Tracking::pipelineShutdown()
{
    try {
        m_pipeline.stop();
    } catch (const rs2::error& e) {
        yError() << "realsense2Driver: failed to stop the pipeline:"
                 << "(" << e.what() << ")";
        m_lastError = e.what();
        return false;
    }
    return true;
}

bool realsense2Tracking::pipelineRestart()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (!pipelineShutdown())
        return false;

    return pipelineStartup();
}

bool realsense2Tracking::open(Searchable& config)
{
    yWarning() << "This software module is experimental.";
    yWarning() << "It is provided with uncomplete documentation and it may be modified/renamed/removed without any notice.";

    string sensor_is = "t265";
    bool b= true;

    m_cfg.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
    m_cfg.enable_stream(RS2_STREAM_GYRO, RS2_FORMAT_MOTION_XYZ32F);
    m_cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);
    b &= pipelineStartup();
    if (b==false)
    {
        yError() << "Pipeline initialization failed";
        return false;
    }

    return true;
}

bool realsense2Tracking::close()
{
    pipelineShutdown();
    return true;
}

//---------------------------------------------------------------------------------------------------------------
/* IThreeAxisGyroscopes methods */
size_t realsense2Tracking::getNrOfThreeAxisGyroscopes() const
{
    return 1;
}

yarp::dev::MAS_status realsense2Tracking::getThreeAxisGyroscopeStatus(size_t sens_index) const
{
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2Tracking::getThreeAxisGyroscopeName(size_t sens_index, std::string& name) const
{
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_gyro_sensor_tag;
    return true;
}

bool realsense2Tracking::getThreeAxisGyroscopeFrameName(size_t sens_index, std::string& frameName) const
{
    if (sens_index != 0) { return false; }
    frameName = m_gyroFrameName;
    return true;
}

bool realsense2Tracking::getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if (sens_index != 0) {
        return false;
    }

    std::lock_guard<std::mutex> guard(m_mutex);
    rs2::frameset dataframe = m_pipeline.wait_for_frames();
    auto fg = dataframe.first(RS2_STREAM_GYRO, RS2_FORMAT_MOTION_XYZ32F);
    rs2::motion_frame gyro = fg.as<rs2::motion_frame>();
    m_last_gyro = gyro.get_motion_data();
    out.resize(3);
    out[0] = m_last_gyro.x;
    out[1] = m_last_gyro.y;
    out[2] = m_last_gyro.z;
    return true;
}

//-------------------------------------------------------------------------------------------------------

/* IThreeAxisLinearAccelerometers methods */
size_t realsense2Tracking::getNrOfThreeAxisLinearAccelerometers() const
{
    return 1;
}

yarp::dev::MAS_status realsense2Tracking::getThreeAxisLinearAccelerometerStatus(size_t sens_index) const
{
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2Tracking::getThreeAxisLinearAccelerometerName(size_t sens_index, std::string& name) const
{
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_accel_sensor_tag;
    return true;
}

bool realsense2Tracking::getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string& frameName) const
{
    if (sens_index != 0) { return false; }
    frameName = m_accelFrameName;
    return true;
}

bool realsense2Tracking::getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    std::lock_guard<std::mutex> guard(m_mutex);
    rs2::frameset dataframe = m_pipeline.wait_for_frames();
    auto fa = dataframe.first(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
    rs2::motion_frame accel = fa.as<rs2::motion_frame>();
    m_last_accel = accel.get_motion_data();
    out.resize(3);
    out[0] = m_last_accel.x;
    out[1] = m_last_accel.y;
    out[2] = m_last_accel.z;
    return true;
}

//-------------------------------------------------------------------------------------------------------

/* IOrientationSensors methods */
size_t realsense2Tracking::getNrOfOrientationSensors() const
{
    return 1;
}

yarp::dev::MAS_status realsense2Tracking::getOrientationSensorStatus(size_t sens_index) const
{
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2Tracking::getOrientationSensorName(size_t sens_index, std::string& name) const
{
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_orientation_sensor_tag;
    return true;
}

bool realsense2Tracking::getOrientationSensorFrameName(size_t sens_index, std::string& frameName) const
{
    if (sens_index != 0) { return false; }
    frameName = m_orientationFrameName;
    return true;
}


bool realsense2Tracking::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const
{
    if (sens_index != 0) { return false; }
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        rs2::frameset dataframe = m_pipeline.wait_for_frames();
        auto fa = dataframe.first(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
        rs2::pose_frame pose = fa.as<rs2::pose_frame>();
        m_last_pose = pose.get_pose_data();
        yarp::math::Quaternion q(m_last_pose.rotation.x, m_last_pose.rotation.y, m_last_pose.rotation.z, m_last_pose.rotation.w);
        yarp::sig::Matrix mat = q.toRotationMatrix3x3();
        yarp::sig::Vector rpy_temp = yarp::math::dcm2rpy(mat);
        rpy[0] = 0 + rpy_temp[0] * 180 / M_PI; //here we can eventually adjust the sign and/or sum an offset
        rpy[1] = 0 + rpy_temp[1] * 180 / M_PI;
        rpy[2] = 0 + rpy_temp[2] * 180 / M_PI;
       return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------------------

/* IPositionSensors methods */
size_t realsense2Tracking::getNrOfPositionSensors() const
{
    return 1;
}

yarp::dev::MAS_status realsense2Tracking::getPositionSensorStatus(size_t sens_index) const
{
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2Tracking::getPositionSensorName(size_t sens_index, std::string& name) const
{
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_pose_sensor_tag;
    return true;
}

bool realsense2Tracking::getPositionSensorFrameName(size_t sens_index, std::string& frameName) const
{
    if (sens_index != 0) { return false; }
    frameName = m_poseFrameName;
    return true;
}


bool realsense2Tracking::getPositionSensorMeasure(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const
{
    std::lock_guard<std::mutex> guard(m_mutex);
    rs2::frameset dataframe = m_pipeline.wait_for_frames();
    auto fa = dataframe.first(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
    rs2::pose_frame pose = fa.as<rs2::pose_frame>();
    m_last_pose = pose.get_pose_data();
    xyz.resize(3);
    xyz[0] = m_last_pose.translation.x;
    xyz[1] = m_last_pose.translation.y;
    xyz[2] = m_last_pose.translation.z;
    return true;
}

//-------------------------------------------------------------------------------------------------------
#if 0
/* IPoseSensors methods */
size_t realsense2Tracking::getNrOfPoseSensors() const
{
    return 1;
}

yarp::dev::MAS_status realsense2Tracking::getPoseSensorStatus(size_t sens_index) const
{
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2Tracking::getPoseSensorName(size_t sens_index, std::string& name) const
{
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_pose_sensor_tag;
    return true;
}

bool realsense2Tracking::getPoseSensorFrameName(size_t sens_index, std::string& frameName) const
{
    if (sens_index != 0) { return false; }
    frameName = m_poseFrameName;
    return true;
}

bool realsense2Tracking::getPoseSensorMeasureAsXYZRPY(size_t sens_index, yarp::sig::Vector& xyzrpy, double& timestamp) const
{
    std::lock_guard<std::mutex> guard(m_mutex);
    rs2::frameset dataframe = m_pipeline.wait_for_frames();
    auto fa = dataframe.first(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
    rs2::pose_frame pose = fa.as<rs2::pose_frame>();
    m_last_pose = pose.get_pose_data();
    xyzrpy.resize(6);
    xyzrpy[0] = m_last_pose.translation.x;
    xyzrpy[1] = m_last_pose.translation.y;
    xyzrpy[2] = m_last_pose.translation.z;
    yarp::math::Quaternion q(m_last_pose.rotation.x, m_last_pose.rotation.y, m_last_pose.rotation.z, m_last_pose.rotation.w);
    yarp::sig::Matrix mat = q.toRotationMatrix3x3();
    yarp::sig::Vector rpy = yarp::math::dcm2rpy(mat);
    xyzrpy[3] = 0 + rpy[0] * 180 / M_PI;
    xyzrpy[4] = 0 + rpy[1] * 180 / M_PI;
    xyzrpy[5] = 0 + rpy[2] * 180 / M_PI;
    return true;
}
#endif
//-----------------------------------------------------------------------------
