/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

class rotation_estimator
{
    // theta is the angle of camera rotation in x, y and z components
    float3 theta;
    std::mutex theta_mtx;
    /* alpha indicates the part that gyro and accelerometer take in computation of theta; higher alpha gives more weight to gyro, but too high
    values cause drift; lower alpha gives more weight to accelerometer, which is more sensitive to disturbances */
    float alpha = 0.98;
    bool first = true;
    // Keeps the arrival time of previous gyro frame
    double last_ts_gyro = 0;
public:
    // Function to calculate the change in angle of motion based on data from gyro
    void process_gyro(rs2_vector gyro_data, double ts)
    {
        if (first) // On the first iteration, use only data from accelerometer to set the camera's initial position
        {
            last_ts_gyro = ts;
            return;
        }
        // Holds the change in angle, as calculated from gyro
        float3 gyro_angle;

        // Initialize gyro_angle with data from gyro
        gyro_angle.x = gyro_data.x; // Pitch
        gyro_angle.y = gyro_data.y; // Yaw
        gyro_angle.z = gyro_data.z; // Roll

        // Compute the difference between arrival times of previous and current gyro frames
        double dt_gyro = (ts - last_ts_gyro) / 1000.0;
        last_ts_gyro = ts;

        // Change in angle equals gyro measures * time passed since last measurement
        gyro_angle = gyro_angle * dt_gyro;

        // Apply the calculated change of angle to the current angle (theta)
        std::lock_guard<std::mutex> lock(theta_mtx);
        theta.add(-gyro_angle.z, -gyro_angle.y, gyro_angle.x);
    }

    void process_accel(rs2_vector accel_data)
    {
        // Holds the angle as calculated from accelerometer data
        float3 accel_angle;

        // Calculate rotation angle from accelerometer data
        accel_angle.z = atan2(accel_data.y, accel_data.z);
        accel_angle.x = atan2(accel_data.x, sqrt(accel_data.y * accel_data.y + accel_data.z * accel_data.z));
        accel_angle.y = 0; //ADDED by randaz81

        // If it is the first iteration, set initial pose of camera according to accelerometer data (note the different handling for Y axis)
        std::lock_guard<std::mutex> lock(theta_mtx);
        if (first)
        {
            first = false;
            theta = accel_angle;
            // Since we can't infer the angle around Y axis using accelerometer data, we'll use PI as a convetion for the initial pose
            theta.y = M_PI;
        }
        else
        {
            /*
            Apply Complementary Filter:
                - high-pass filter = theta * alpha:  allows short-duration signals to pass through while filtering out signals
                  that are steady over time, is used to cancel out drift.
                - low-pass filter = accel * (1- alpha): lets through long term changes, filtering out short term fluctuations
            */
            theta.x = theta.x * alpha + accel_angle.x * (1 - alpha);
            theta.z = theta.z * alpha + accel_angle.z * (1 - alpha);
        }
    }

    // Returns the current rotation angle
    float3 get_theta()
    {
        std::lock_guard<std::mutex> lock(theta_mtx);
        return theta;
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

realsense2withIMUDriver::realsense2withIMUDriver() :
        realsense2Driver()
{
    m_rotation_estimator=nullptr;
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
    yWarning() << "This software module is experimental.";
    yWarning() << "It is provided with uncomplete documentation and it may be modified/renamed/removed without any notice.";

    string sensor_is = "d435i";
    bool b = true;

    if (sensor_is == "d435")
    {
        b &= realsense2Driver::open(config);
        //m_sensor_has_pose_capabilities = false;
        m_sensor_has_orientation_estimator = false;
    }
    else if (sensor_is == "d435i")
    {
        b &= realsense2Driver::open(config);
        //m_sensor_has_pose_capabilities = false;
        m_sensor_has_orientation_estimator = true;
        m_cfg.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
        m_cfg.enable_stream(RS2_STREAM_GYRO, RS2_FORMAT_MOTION_XYZ32F);
        b &= pipelineRestart();
    }
    /*
    //T265 is very diffcukt to implement without major refactoring. 
    //here some infos, just for reference
    else if(sensor_is == "t265")
    {
        b &= realsense2Driver::open(config);
        m_sensor_has_pose_capabilities = true;
        m_sensor_has_orientation_estimator = false;
        m_cfg.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
        m_cfg.enable_stream(RS2_STREAM_GYRO, RS2_FORMAT_MOTION_XYZ32F);
        m_cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);
        b &= pipelineRestart();
    }*/
    else
    {
        yError() << "Unkwnon device";
        return false;
    }

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
    if (m_sensor_has_orientation_estimator) { return 1; }
    return 0;
}

yarp::dev::MAS_status realsense2withIMUDriver::getOrientationSensorStatus(size_t sens_index) const
{
    if (m_sensor_has_orientation_estimator == false) {return yarp::dev::MAS_status::MAS_UNKNOWN; }
    if (sens_index != 0) { return yarp::dev::MAS_status::MAS_UNKNOWN; }
    return yarp::dev::MAS_status::MAS_OK;
}

bool realsense2withIMUDriver::getOrientationSensorName(size_t sens_index, std::string& name) const
{
    if (m_sensor_has_orientation_estimator==false)
    {
        return false;
    }
    if (sens_index != 0) { return false; }
    name = m_inertial_sensor_name_prefix + "/" + m_orientation_sensor_tag;
    return true;
}

bool realsense2withIMUDriver::getOrientationSensorFrameName(size_t sens_index, std::string& frameName) const
{
    if (m_sensor_has_orientation_estimator == false)
    {
        return false;
    }
    if (sens_index != 0) { return false; }
    frameName = m_orientationFrameName;
    return true;
}


bool realsense2withIMUDriver::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const
{
    if (sens_index != 0) { return false; }
    if (m_sensor_has_orientation_estimator)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        rs2::frameset dataframe = m_pipeline.wait_for_frames();
        auto motion = dataframe.as<rs2::motion_frame>();
        if (motion && motion.get_profile().stream_type() == RS2_STREAM_GYRO && 
                      motion.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F)
        {
            // Get the timestamp of the current frame
            double ts = motion.get_timestamp();
            // Get gyro measures
            rs2_vector gyro_data = motion.get_motion_data();
            // Call function that computes the angle of motion based on the retrieved measures
            m_rotation_estimator->process_gyro(gyro_data, ts);
        }
        // If casting succeeded and the arrived frame is from accelerometer stream
        if (motion && motion.get_profile().stream_type() == RS2_STREAM_ACCEL &&
                      motion.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F)
        {
            // Get accelerometer measures
            rs2_vector accel_data = motion.get_motion_data();
            // Call function that computes the angle of motion based on the retrieved measures
            m_rotation_estimator->process_accel(accel_data);
        }
        float3 theta = m_rotation_estimator->get_theta();
        rpy.resize(3);
        rpy[0] = 0 + theta.x * 180.0 / M_PI; //here we can eventually adjust the sign and/or sum an offset
        rpy[1] = 0 + theta.y * 180.0 / M_PI;
        rpy[2] = 0 + theta.z * 180.0 / M_PI;
        return true;
    }
    return false;
}
