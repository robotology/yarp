/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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


/**
 *  @ingroup dev_impl_media
 *
 * @brief `realsense2` : driver for realsense2 compatible devices.
 *
 * This device driver exposes the IRGBDSensor and IFrameGrabberControls
 * interfaces to read the images and operate on the available settings.
 * See the documentation for more details about each interface.
 *
 * This device is paired with its server called RGBDSensorWrapper to stream the images and perform remote operations.
 *
 * The configuration file is subdivided into 2 major sections called "SETTINGS" and "HW_DESCRIPTION".
 *
 * The "SETTINGS" section is meant for read/write parameters, meaning parameters which can be get and set by the device.
 * A common case of setting is the image resolution in pixel. This setting will be read by the device and it'll be applied
 * in the startup phase. If the setting fails, the device will terminate the execution with a error message.
 *
 * The "HW_DESCRIPTION" section is meant for read only parameters which describe the hardware property of the device and
 * cannot be provided by the device through software API.
 * A common case is the 'Field Of View' property, which may or may not be supported by the physical device.
 * When a property is present in the HW_DESCRIPTION group, the YARP RGBDSensorClient will report this value when asked for
 * and setting will be disabled.
 * This group can also be used to by-pass realsense2 API in case some functionality is not correctly working with the current
 * device. For example the 'clipPlanes' property may return incorrect values or values using non-standard measurement unit.
 * In this case using the HW_DESCRIPTION, a user can override the value got from OpenNI2 API with a custom value.
 *
 * \note Parameters inside the HW_DESCRIPTION are read only, so the relative set functionality will be disabled.
 * \note For parameters which are neither in SETTINGS nor in HW_DESCRIPTION groups, read / write functionality is assumed
 *  but not initial setting will be performed. Device will start with manufacturer default values.
 * \warning A single parameter cannot be present into both SETTINGS and HW_DESCRIPTION groups.
 * \warning whenever more then one value is required by the setting, the values must be in parentheses!
 *
 * | YARP device name |
 * |:-----------------:|
 * | `realsense2` |
 *
 *   Parameters used by this device are:
 * | Parameter name               | SubParameter        | Type                |  Read / write   | Units          | Default Value | Required                         | Description                                                                            | Notes                                                                 |
 * |:----------------------------:|:-------------------:|:-------------------:|:---------------:|:--------------:|:-------------:|:--------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------:|
 * |  stereoMode                  |      -              | bool                |  Read / write   |                |   false       |  No(see notes)                   | Flag for using the realsense as stereo camera                                          |  This option is to use it with yarp::dev::ServerGrabber as network wrapper. The stereo images provided are raw images(yarp::sig::PixelMono) and note that not all the realsense devices have the stereo streams. |
 * |  verbose                     |      -              | bool                |  Read / write   |                |   false       |  No                              | Flag for enabling debug prints                                                         |                                                                       |
 * |  SETTINGS                    |      -              | group               |  Read / write   | -              |   -           |  Yes                             | Initial setting of the device.                                                         |  Properties must be read/writable in order for setting to work        |
 * |                              |  rgbResolution      | int, int            |  Read / write   | pixels         |   -           |  Yes                             | Size of rgb image in pixels                                                            |  2 values expected as height, width                                   |
 * |                              |  depthResolution    | int, int            |  Read / write   | pixels         |   -           |  Yes                             | Size of depth image in pixels                                                          |  Values are height, width                                             |
 * |                              |  accuracy           | double              |  Read / write   | meters         |   -           |  No                              | Accuracy of the device, as the depth measurement error at 1 meter distance             |  Note that only few realsense devices allows to set it                |
 * |                              |  framerate          | int                 |  Read / Write   | fps            |   30          |  No                              | Framerate of the sensor                                                                |                                                                       |
 * |                              |  enableEmitter      | bool                |  Read / Write   | -              |   true        |  No                              | Flag for enabling the IR emitter(if supported by the sensor)                           |                                                                       |
 * |                              |  needAlignment      | bool                |  Read / Write   | -              |   true        |  No                              | Flag for enabling the alignment of the depth frame over the rgb frame                  |  This option is deprecated, please use alignmentFrame instead.        |
 * |                              |  alignmentFrame     | string              |  Read / Write   | -              |   RGB         |  No                              | This parameter specifies the frame to which the frames RGB and Depth will be aligned.  |  The accepted values are RGB, Depth, None. This operation could be heavy, set it to None to increase the fps.|
 * |  HW_DESCRIPTION              |      -              |  group              |                 | -              |   -           |  Yes                             | Hardware description of device property.                                               |  Read only property. Setting will be disabled                         |
 * |                              |  clipPlanes         | double, double      |  Read / write   | meters         |   -           |  No                              | Minimum and maximum distance at which an object is seen by the depth sensor            |  parameter introduced mainly for simulated sensors, it can be used to set the clip planes if Openni gives wrong values |
 *
 * Configuration file using .ini format, for using as RGBD device:
 *
 * \code{.unparsed}

device       RGBDSensorWrapper
subdevice    realsense2
name         /depthCamerac

[SETTINGS]
depthResolution (640 480)    #Note the parentheses
rgbResolution   (640 480)
framerate       30
enableEmitter   true
alignmentFrame  RGB

[HW_DESCRIPTION]
clipPlanes (0.2 10.0)
 *
 * \endcode
 *
 * Configuration file using .ini format, for using as stereo camera:
 * \code{.unparsed}
device       grabberDual
subdevice    realsense2
name         /stereoCamera
capabilities RAW
stereoMode   true

[SETTINGS]
rgbResolution   (640 480)
depthResolution   (640 480)
framerate       30
enableEmitter   false

[HW_DESCRIPTION]
clipPlanes (0.2 10.0)
 * \endcode
 */

class realsense2withIMUDriver :
        public realsense2Driver,
        public yarp::dev::IThreeAxisGyroscopes,
        public yarp::dev::IThreeAxisLinearAccelerometers,
#ifdef FORCE_MISSING_MAGNETOMETERS_ON
        public yarp::dev::IThreeAxisMagnetometers,
#endif
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

    void updateTransformations();
#if 0
    bool pipelineStartup(); //inherited
    bool pipelineShutdown(); //inherited
    bool pipelineRestart(); //inherited
    void fallback(); //inherited
#endif
    bool setFramerate(const int _fps);


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

#ifdef FORCE_MISSING_MAGNETOMETERS_ON
    /* IThreeAxisMagnetometers methods */
    size_t getNrOfThreeAxisMagnetometers() const override;
    yarp::dev::MAS_status getThreeAxisMagnetometerStatus(size_t sens_index) const override;
    bool getThreeAxisMagnetometerName(size_t sens_index, std::string& name) const override;
    bool getThreeAxisMagnetometerFrameName(size_t sens_index, std::string& frameName) const override;
    bool getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
#endif

    /* IOrientationSensors methods */
    size_t getNrOfOrientationSensors() const override;
    yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    bool getOrientationSensorName(size_t sens_index, std::string& name) const override;
    bool getOrientationSensorFrameName(size_t sens_index, std::string& frameName) const override;
    bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;

        /* IPoseSensors methods */
    size_t getNrOfPoseSensors() const ;
    yarp::dev::MAS_status getPoseSensorStatus(size_t sens_index) const;
    bool getPoseSensorName(size_t sens_index, std::string& name) const;
    bool getPoseSensorFrameName(size_t sens_index, std::string& frameName) const;
    bool getPoseSensorMeasureAsXYZRPY(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const;

protected:
    // realsense classes
    mutable rs2_vector m_last_gyro;
    mutable rs2_vector m_last_accel;
    mutable rs2_pose   m_last_pose;

    bool m_sensor_has_pose_capabilities;
    bool m_sensor_has_orientation_estimator;

    //strings
    std::string m_inertial_sensor_name_prefix;
    const std::string m_accel_sensor_tag = "accelerations_sensor";
    const std::string m_gyro_sensor_tag = "gyro_sensor";
    const std::string m_orientation_sensor_tag = "orientation_sensor";
    const std::string m_pose_sensor_tag = "pose_sensor";
    const std::string m_magnetic_sensor_tag = "magnetic_field_sensor";
    std::string m_gyroFrameName;
    std::string m_accelFrameName;
    std::string m_orientationFrameName;
    std::string m_poseFrameName;
    std::string m_magneticFrameName;

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
