/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef REALSENSE2IMU_DRIVER_H
#define REALSENSE2IMU_DRIVER_H

#include <yarp/os/PeriodicThread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

#include "realsense2Driver.h"
#include <cstring>
#include <iostream>
#include <librealsense2/rs.hpp>
#include <map>
#include <mutex>

 /**********************************************************************************************************/
 // This software module is experimental.
 // It is provided with uncomplete documentation and it may be modified/renamed/removed without any notice.
 /**********************************************************************************************************/
 
class rotation_estimator;

class realsense2withIMUDriver :
        public realsense2Driver,
        public yarp::dev::IThreeAxisGyroscopes,
        public yarp::dev::IThreeAxisLinearAccelerometers,
        public yarp::dev::IOrientationSensors
{
private:
    typedef yarp::os::Stamp Stamp;
    typedef yarp::os::Property Property;

public:
    realsense2withIMUDriver();
    ~realsense2withIMUDriver() override = default;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

private:
    //method
    inline bool initializeRealsenseDevice();
    inline bool setParams();

#if 0
    bool pipelineStartup(); //inherited
    bool pipelineShutdown(); //inherited
    bool pipelineRestart(); //inherited
    void fallback(); //inherited
#endif

public:
    /* IThreeAxisGyroscopes methods */
    size_t getNrOfThreeAxisGyroscopes() const override;
    yarp::dev::MAS_status getThreeAxisGyroscopeStatus(size_t sens_index) const override;
    bool getThreeAxisGyroscopeName(size_t sens_index, std::string& name) const override;
    bool getThreeAxisGyroscopeFrameName(size_t sens_index, std::string& frameName) const override;
    bool getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisLinearAccelerometers methods */
    size_t getNrOfThreeAxisLinearAccelerometers() const override;
    yarp::dev::MAS_status getThreeAxisLinearAccelerometerStatus(size_t sens_index) const override;
    bool getThreeAxisLinearAccelerometerName(size_t sens_index, std::string& name) const override;
    bool getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string& frameName) const override;
    bool getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IOrientationSensors methods */
    size_t getNrOfOrientationSensors() const override;
    yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    bool getOrientationSensorName(size_t sens_index, std::string& name) const override;
    bool getOrientationSensorFrameName(size_t sens_index, std::string& frameName) const override;
    bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;

protected:
    // realsense classes
    mutable rs2_vector m_last_gyro;
    mutable rs2_vector m_last_accel;
    mutable rs2_pose   m_last_pose;
    mutable rotation_estimator* m_rotation_estimator;

    bool m_sensor_has_orientation_estimator;

    //strings
    std::string       m_inertial_sensor_name_prefix;
    const std::string m_accel_sensor_tag       = "accelerations_sensor";
    const std::string m_gyro_sensor_tag        = "gyro_sensor";
    const std::string m_orientation_sensor_tag = "orientation_sensor";
    const std::string m_position_sensor_tag    = "position_sensor";
    std::string m_gyroFrameName;
    std::string m_accelFrameName;
    std::string m_orientationFrameName;
    std::string m_positionFrameName;

    /*std::mutex   m_mutex;
    rs2::context m_ctx;
    rs2::config m_cfg;
    rs2::pipeline m_pipeline;
    rs2::pipeline_profile m_profile;
    rs2::device  m_device;
    std::vector<rs2::sensor> m_sensors;
    rs2::sensor* m_depth_sensor;
    rs2::sensor* m_color_sensor;
    rs2_intrinsics m_depth_intrin{}, m_color_intrin{}, m_infrared_intrin{};
    rs2_extrinsics m_depth_to_color{}, m_color_to_depth{};
    rs2_stream  m_alignment_stream{RS2_STREAM_COLOR};


    yarp::os::Stamp m_rgb_stamp;
    yarp::os::Stamp m_depth_stamp;
    std::string m_lastError;
    yarp::dev::RGBDSensorParamParser m_paramParser;
    bool m_verbose;
    bool m_initialized;
    bool m_stereoMode;
    bool m_needAlignment;
    int m_fps;
    float m_scale;
    std::vector<cameraFeature_id_t> m_supportedFeatures;*/
};
#endif

