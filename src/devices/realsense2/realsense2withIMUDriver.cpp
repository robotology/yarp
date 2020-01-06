/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "realsense2withIMUdriver.h"

#include <yarp/os/Value.h>

#include <yarp/sig/ImageUtils.h>

#include <yarp/math/quaternion.h>
#include <yarp/math/math.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <librealsense2/rsutil.h>
#include <mutex>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

using namespace std;

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

realsense2withIMUDriver::realsense2withIMUDriver() :
        realsense2Driver()
{
}

#if 0
bool realsense2withIMUDriver::pipelineStartup()
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

bool realsense2withIMUDriver::pipelineShutdown()
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

bool realsense2withIMUDriver::pipelineRestart()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (!pipelineShutdown())
        return false;

    return pipelineStartup();
}


void realsense2Driver::fallback()
{
    m_cfg.enable_stream(RS2_STREAM_COLOR, m_color_intrin.width, m_color_intrin.height, RS2_FORMAT_RGB8, m_fps);
    m_cfg.enable_stream(RS2_STREAM_DEPTH, m_depth_intrin.width, m_depth_intrin.height, RS2_FORMAT_Z16, m_fps);
    yWarning() << "realsense2Driver: format not supported, use --verbose for more details. Setting the fallback format";
    std::cout << "COLOR: " << m_color_intrin.width << "x" << m_color_intrin.height << " fps: " << m_fps << std::endl;
    std::cout << "DEPTH: " << m_depth_intrin.width << "x" << m_depth_intrin.height << " fps: " << m_fps << std::endl;
}
#endif

bool realsense2withIMUDriver::open(Searchable& config)
{
    realsense2Driver::open(config);
    m_cfg.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
    m_cfg.enable_stream(RS2_STREAM_GYRO, RS2_FORMAT_MOTION_XYZ32F);
    m_sensor_has_pose_capabilities = false;
    if (m_sensor_has_pose_capabilities)
    {
        m_cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);
    }
    bool b = pipelineRestart();
    return b;
}

bool realsense2withIMUDriver::close()
{
    pipelineShutdown();
    return true;
}

//---------------------------------------------------------------------------------------------------------------
/* IThreeAxisGyroscopes methods */
size_t realsense2withIMUDriver::getNrOfThreeAxisGyroscopes() const
{
    return 1;
}

yarp::dev::MAS_status realsense2withIMUDriver::getThreeAxisGyroscopeStatus(size_t sens_index) const
{
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2withIMUDriver::getThreeAxisGyroscopeName(size_t sens_index, std::string& name) const
{
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_gyro_sensor_tag;
    return true;
}

bool realsense2withIMUDriver::getThreeAxisGyroscopeFrameName(size_t sens_index, std::string& frameName) const
{
    if (sens_index != 0) { return false; }
    frameName = m_gyroFrameName;
    return true;
}

bool realsense2withIMUDriver::getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if (sens_index != 0) {
        return false;
    }

    std::lock_guard<std::mutex> guard(realsense2Driver::m_mutex);
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
size_t realsense2withIMUDriver::getNrOfThreeAxisLinearAccelerometers() const
{
    return 1;
}

yarp::dev::MAS_status realsense2withIMUDriver::getThreeAxisLinearAccelerometerStatus(size_t sens_index) const
{
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2withIMUDriver::getThreeAxisLinearAccelerometerName(size_t sens_index, std::string& name) const
{
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_accel_sensor_tag;
    return true;
}

bool realsense2withIMUDriver::getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string& frameName) const
{
    if (sens_index != 0) { return false; }
    frameName = m_accelFrameName;
    return true;
}

bool realsense2withIMUDriver::getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
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
size_t realsense2withIMUDriver::getNrOfOrientationSensors() const
{
    if (m_sensor_has_pose_capabilities) { return 1; }
    if (m_sensor_has_orientation_estimator) { return 1; }
    return 0;
}

yarp::dev::MAS_status realsense2withIMUDriver::getOrientationSensorStatus(size_t sens_index) const
{
    if (m_sensor_has_pose_capabilities == false || m_sensor_has_orientation_estimator == false) {return yarp::dev::MAS_status::MAS_UNKNOWN; }
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2withIMUDriver::getOrientationSensorName(size_t sens_index, std::string& name) const
{
    if (m_sensor_has_pose_capabilities ==false || m_sensor_has_orientation_estimator==false)
    {
        return false;
    }
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_orientation_sensor_tag;
    return true;
}

bool realsense2withIMUDriver::getOrientationSensorFrameName(size_t sens_index, std::string& frameName) const
{
    if (m_sensor_has_pose_capabilities == false || m_sensor_has_orientation_estimator == false)
    {
        return false;
    }
    if (sens_index != 0) { return false; }
    frameName = m_orientationFrameName;
    return true;
}


bool realsense2withIMUDriver::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const
{
    if (m_sensor_has_pose_capabilities == false || m_sensor_has_orientation_estimator == false)
    {
        return false;
    }
    if (sens_index != 0) { return false; }
    return true;
}

//-------------------------------------------------------------------------------------------------------

/* IPoseSensors methods */
size_t realsense2withIMUDriver::getNrOfPoseSensors() const
{
    if (m_sensor_has_pose_capabilities == false) { return 0; }
    return 1;
}

yarp::dev::MAS_status realsense2withIMUDriver::getPoseSensorStatus(size_t sens_index) const
{
    if (m_sensor_has_pose_capabilities == false) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2withIMUDriver::getPoseSensorName(size_t sens_index, std::string& name) const
{
    if (m_sensor_has_pose_capabilities == false) { return false; }
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_pose_sensor_tag;
    return true;
}

bool realsense2withIMUDriver::getPoseSensorFrameName(size_t sens_index, std::string& frameName) const
{
    if (m_sensor_has_pose_capabilities == false) { return false; }
    if (sens_index != 0) { return false; }
    frameName = m_poseFrameName;
    return true;
}


bool realsense2withIMUDriver::getPoseSensorMeasureAsXYZRPY(size_t sens_index, yarp::sig::Vector& xyzrpy, double& timestamp) const
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
    yarp::math::Quaternion q(m_last_pose.rotation.x, m_last_pose.rotation.y,m_last_pose.rotation.z,m_last_pose.rotation.w);
    yarp::sig::Matrix mat = q.toRotationMatrix3x3();
    yarp::sig::Vector rpy = yarp::math::dcm2rpy(mat);
    xyzrpy[3] = rpy[0];
    xyzrpy[4] = rpy[1];
    xyzrpy[5] = rpy[2];
    return true;
}

//-----------------------------------------------------------------------------

#ifdef FORCE_MISSING_MAGNETOMETERS_ON
/* IThreeAxisMagnetometers methods */
size_t realsense2withIMUDriver::getNrOfThreeAxisMagnetometers() const
{
    return 0;
}

yarp::dev::MAS_status realsense2withIMUDriver::getThreeAxisMagnetometerStatus(size_t sens_index) const
{
    return yarp::dev::MAS_status::MAS_UNKNOWN;
}

bool realsense2withIMUDriver::getThreeAxisMagnetometerName(size_t sens_index, std::string& name) const
{
    name = "invalid";
    return false;
}

bool realsense2withIMUDriver::getThreeAxisMagnetometerFrameName(size_t sens_index, std::string& frameName) const
{
    frameName = "invalid";
    return false;
}

bool realsense2withIMUDriver::getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    out.resize(3);
    out[0] = out[1] = out[2] = std::nan("");
    timestamp = yarp::os::Time::now();
    return false;
}
#endif
