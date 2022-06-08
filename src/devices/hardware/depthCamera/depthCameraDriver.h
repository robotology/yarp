/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef DEPTHCAMERA_DRIVER_H
#define DEPTHCAMERA_DRIVER_H

#include <string>
#include <map>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/RGBDSensorParamParser.h>
#include <OpenNI.h>


#ifndef RAD2DEG
#define RAD2DEG (180/3.14159265359)
#endif

#ifndef DEG2RAD
#define DEG2RAD (3.14159265359/180.0)
#endif

class  streamFrameListener;

/**
 *  @ingroup dev_impl_media
 *
 * @brief `depthCamera`: YARP driver for OpenNI2 compatible devices.
 *
 * This device driver exposes the IRGBDSensor and IFrameGrabberControls
 * interfaces to read the images and operate on the available settings.
 *
 * See the documentation for more details about each interface.
 *
 * This device is paired with its server called RGBDSensorWrapper to stream the images and perform remote operations.
 *
 * The config file is subdivided into 5 major sections called "SETTINGS", "HW_DESCRIPTION", "RGB_INTRINSIC_PARAMETERS",
 * "DEPTH_INTRINSIC_PARAMETERS", "EXTRINSIC_PARAMETERS".
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
 * This group can also be used to by-pass OpenNI2 API in case some functionality is not correctly working with the current
 * device. For example the 'clipPlanes' property may return incorrect values or values using non-standard measurement unit.
 * In this case using the HW_DESCRIPTION, a user can override the value got from OpenNI2 API with a custom value.
 *
 * \note Parameters inside the HW_DESCRIPTION are read only, so the relative set functionality will be disabled.
 * \note For parameters which are neither in SETTINGS nor in HW_DESCRIPTION groups, read / write functionality is assumed
 *  but not initial setting will be performed. Device will start with manufacturer default values.
 * \warning A single parameter cannot be present into both SETTINGS and HW_DESCRIPTION groups.
 *
 * The [RGB/DEPTH_INTRINSIC_PARAMETERS] group describe the camera intrinsic parameters for rgb and depth respectively.
 * \note Right now only 'plumb_bob' (pinhole camera) distortion model is supported.
 *
 * The [EXTRINSIC_PARAMETERS] group describe the position of the depth camera in the rgb camera frame. It is composed of
 * a 4x4 rototranslation matrix.
 *
 * \warning: whenever more then one value is required by the setting, the values must be in parenthesys!
 *
 * | YARP device name |
 * |:-----------------:|
 * | `depthCamera` |
 *
 *   Parameters used by this device are:
 * | Parameter name               | SubParameter        | Type                |  Read / write   | Units          | Default Value | Required                         | Description                                                                            | Notes                                                                 |
 * |:----------------------------:|:-------------------:|:-------------------:|:---------------:|:--------------:|:-------------:|:--------------------------------:|:--------------------------------------------------------------------------------------:|:---------------------------------------------------------------------:|
 * |  SETTINGS                    |      -              | group               |  Read / write   | -              |   -           |   Yes                            | Initial setting of the device.                                                         |  Properties must be read/writable in order for setting to work        |
 * |                              |  rgbResolution      | int, int            |  Read / write   | pixels         |   -           |  Alternative to HW_DESCRIPTION   | Size of rgb image in pixels                                                            |  2 values expected as height, width                                   |
 * |                              |  depthResolution    | int, int            |  Read / write   | pixels         |   -           |  Alternative to HW_DESCRIPTION   | Size of depth image in pixels                                                          |  Values are height, width                                             |
 * |                              |  accuracy           | double              |  Read / write   | meters         |   -           |  Alternative to HW_DESCRIPTION   | Accuracy of the device, as the depth measurement error at 1 meter distance             |                                                                       |
 * |                              |  rgbFOV             | double, double      |  Read / write   | degrees        |   -           |  Alternative to HW_DESCRIPTION   | Horizontal and Vertical fields of view of the rgb camera                               | 2 values expected as horizontal and vertical FOVs                     |
 * |                              |  depthFOV           | double, double      |  Read / write   | degrees        |   -           |  Alternative to HW_DESCRIPTION   | Horizontal and Vertical fields of view of the depth camera                             | 2 values expected as horizontal and vertical FOVs                     |
 * |                              |  rgbMirroring       | bool                |  Read / write   | true/false     |  false        |  Alternative to HW_DESCRIPTION   | Set the mirroring to the acquired rgb image                                            |                                                                       |
 * |                              |  depthMirroring     | bool                |  Read / write   | true/false     |  false        |  Alternative to HW_DESCRIPTION   | Set the mirroring to the acquired depth image                                          |                                                                       |
 * |                              |  clipPlanes         | double, double      |  Read / write   | meters         |   -           |  Alternative to HW_DESCRIPTION   | Minimum and maximum distance at which an object is seen by the depth sensor            |  parameter introduced mainly for simulated sensors, it can be used to set the clip planes if Openni gives wrong values |
 * |  HW_DESCRIPTION              |      -              |  group              |                 | -              |   -           |   Yes                            | Hardware description of device property.                                               |  Read only property. Setting will be disabled                         |
 * |                              | same as 'SETTINGS' group | -              |    Read only    | -              |   -           |   Alternative to SETTING group   | Parameters here are alternative to the SETTING group                                   |                                                                       |
 * |  RGB_INTRINSIC_PARAMETERS    |      -              | group               |                 | -              |   -           |   Yes                            | Description of rgb camera visual parameters                                            |                                                                       |
 * |                              |  physFocalLength    | double              |                 | m              |   -           |   Yes                            | Physical focal length of the lens in meters                                            |                                                                       |
 * |                              |   focalLengthX      | double              |                 | pixel          |   -           |   Yes                            | Horizontal component of the focal length as a multiple of pixel width                  |                                                                       |
 * |                              |   focalLengthY      | double              |                 | pixel          |   -           |   Yes                            | Vertical component of the focal length as a multiple of pixel height                   |                                                                       |
 * |                              |  principalPointX    | double              |                 | pixel          |   -           |   Yes                            | X coordinate of the principal point                                                    |                                                                       |
 * |                              |  principalPointY    | double              |                 | pixel          |   -           |   Yes                            | Y coordinate of the principal point                                                    |                                                                       |
 * |                              |  distortionModel    | string              |                 | -              |   -           |   Yes                            |  Reference to group of parameters describing the distortion model of the camera, example 'rgbDistortionModelGroup'       | This is only another group's name to be searched for in the config file   |
 * |  rgbDistortionModelGroup     |                     |                     |                 |                |               |                                  |                                                                                        |                                                                       |
 * |                              |   name              | string              |                 | -              |   -           |   Yes                            |  Name of the distortion model, see notes                                               | right now only 'plumb_bob' is supported                               |
 * |                              |   k1                | double              |                 | -              |   -           |   Yes                            |                                                                                        |                                                                       |
 * |                              |   k2                | double              |                 | -              |   -           |   Yes                            |                                                                                        |                                                                       |
 * |                              |   k3                | double              |                 | -              |   -           |   Yes                            |                                                                                        |                                                                       |
 * |                              |   t1                | double              |                 | -              |   -           |   Yes                            |                                                                                        |                                                                       |
 * |                              |   t2                | double              |                 | -              |   -           |   Yes                            |                                                                                        |                                                                       |
 * |  DEPTH_INTRINSIC_PARAMETERS  |      -              | group               |                 | -              |   -           |   Yes                            | Description of depth camera visual parameters                                          |                                                                       |
 * |                              |  physFocalLength    | double              |                 | m              |   -           |   Yes                            | Physical focal length of the lens in meters                                            |                                                                       |
 * |                              |   focalLengthX      | double              |                 | pixel          |   -           |   Yes                            | Horizontal component of the focal length as a multiple of pixel width                  |                                                                       |
 * |                              |   focalLengthY      | double              |                 | pixel          |   -           |   Yes                            | Vertical component of the focal length as a multiple of pixel height                   |                                                                       |
 * |                              |  principalPointX    | double              |                 | pixel          |   -           |   Yes                            | X coordinate of the principal point                                                    |                                                                       |
 * |                              |  principalPointY    | double              |                 | pixel          |   -           |   Yes                            | Y coordinate of the principal point                                                    |                                                                       |
 * |                              |  distortionModel    | string              |                 | -              |   -           |   Yes                            |  Reference to group of parameters describing the distortion model of the camera, example 'depthDistortionModelGroup' | This is another group's name to be searched for in the config file    |
 * |  depthDistortionModelGroup   |                     |                     |                 |                |               |                                  |                                                                                        |                                                                       |
 * |                              |   name              | string              |                 | -              |   -           |   Yes                            |  Name of the distortion model, see notes                                               | right now only 'plumb_bob' is supported                               |
 * |                              |   k1                | double              |                 | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
 * |                              |   k2                | double              |                 | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
 * |                              |   k3                | double              |                 | -              |   -           |   Yes                            |  Radial distortion coefficient of the lens                                             |                                                                       |
 * |                              |   t1                | double              |                 | -              |   -           |   Yes                            |  Tangential distortion of the lens                                                     |                                                                       |
 * |                              |   t2                | double              |                 | -              |   -           |   Yes                            |  Tangential distortion of the lens                                                     |                                                                       |
 * |  EXTRINSIC_PARAMETERS        |      -              |                     |                 | -              |   -           |   Yes                            |                                                                                        |                                                                       |
 * |                              | transformation      | 4x4 double matrix   |                 | -              |   -           |   Yes                            | trasformation matrix between depth optical frame to the rgb one                        |                                                                       |
 *
 *
 * Configuration file using .ini format, using subdevice keyword.
 *
 * \code{.unparsed}

device       RGBDSensorWrapper
subdevice    depthCamera
name         /depthCamera

[SETTINGS]
accuracy     0.001
depthResolution (320 240)    #Note the parentesys
rgbResolution   (320 240)
rgbMirroring    false
depthMirroring  false

[HW_DESCRIPTION]
clipPlanes (0.4 4.5)

[RGB_INTRINSIC_PARAMETERS]
physFocalLength         0.5
focalLengthX            1.0
focalLengthY            2.0
principalPointX         256.0
principalPointY         128.0
distortionModel         rgb_distortion

[rgb_distortion]
name                    plumb_bob
k1                      1.0
k2                      2.0
t1                      3.0
t2                      4.0
k3                      5.0

[DEPTH_INTRINSIC_PARAMETERS]
physFocalLength         0.5
focalLengthX            1.0
focalLengthY            2.0
principalPointX         256.0
principalPointY         128.0
distortionModel         depth_distortion

[depth_distortion]
name                    plumb_bob
k1                      1.0
k2                      2.0
t1                      3.0
t2                      4.0
k3                      5.0

[EXTRINSIC_PARAMETERS]
transformation          (1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0  0.0 0.0 0.0 1.0)
 *
 * \endcode
 */

class depthCameraDriver :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRGBDSensor,
        public yarp::dev::IFrameGrabberControls
{
private:
    typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;
    typedef yarp::os::Stamp                           Stamp;
    typedef yarp::os::Property                        Property;
    typedef yarp::sig::FlexImage                      FlexImage;

public:
    depthCameraDriver();
    ~depthCameraDriver();
    static int pixFormatToCode(openni::PixelFormat p);

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // IRGBDSensor
    int    getRgbHeight() override;
    int    getRgbWidth() override;
    bool   getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig>& configurations) override;
    bool   getRgbResolution(int& width, int& height) override;
    bool   setRgbResolution(int width, int height) override;
    bool   getRgbFOV(double& horizontalFov, double& verticalFov) override;
    bool   setRgbFOV(double horizontalFov, double verticalFov) override;
    bool   getRgbMirroring(bool& mirror) override;
    bool   setRgbMirroring(bool mirror) override;

    bool   getRgbIntrinsicParam(Property& intrinsic) override;
    int    getDepthHeight() override;
    int    getDepthWidth() override;
    bool   setDepthResolution(int width, int height) override;
    bool   getDepthFOV(double& horizontalFov, double& verticalFov) override;
    bool   setDepthFOV(double horizontalFov, double verticalFov) override;
    bool   getDepthIntrinsicParam(Property& intrinsic) override;
    double getDepthAccuracy() override;
    bool   setDepthAccuracy(double accuracy) override;
    bool   getDepthClipPlanes(double& nearPlane, double& farPlane) override;
    bool   setDepthClipPlanes(double nearPlane, double farPlane) override;
    bool   getDepthMirroring(bool& mirror) override;
    bool   setDepthMirroring(bool mirror) override;


    bool   getExtrinsicParam(yarp::sig::Matrix& extrinsic) override;
    bool   getRgbImage(FlexImage& rgbImage, Stamp* timeStamp = NULL) override;
    bool   getDepthImage(depthImage& depthImage, Stamp* timeStamp = NULL) override;
    bool   getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp=NULL, Stamp* depthStamp=NULL) override;

    RGBDSensor_status     getSensorStatus() override;
    std::string getLastErrorMsg(Stamp* timeStamp = NULL) override;

    //IFrameGrabberControls
    bool   getCameraDescription(CameraDescriptor *camera) override;
    bool   hasFeature(int feature, bool*   hasFeature) override;
    bool   setFeature(int feature, double  value) override;
    bool   getFeature(int feature, double* value) override;
    bool   setFeature(int feature, double  value1,  double  value2) override;
    bool   getFeature(int feature, double* value1,  double* value2) override;
    bool   hasOnOff(  int feature, bool*   HasOnOff) override;
    bool   setActive( int feature, bool    onoff) override;
    bool   getActive( int feature, bool*   isActive) override;
    bool   hasAuto(   int feature, bool*   hasAuto) override;
    bool   hasManual( int feature, bool*   hasManual) override;
    bool   hasOnePush(int feature, bool*   hasOnePush) override;
    bool   setMode(   int feature, FeatureMode mode) override;
    bool   getMode(   int feature, FeatureMode *mode) override;
    bool   setOnePush(int feature) override;

private:
    //method
    inline bool initializeOpeNIDevice();
    inline bool setParams();
    bool        getImage(FlexImage& Frame, Stamp* Stamp, streamFrameListener *sourceFrame);
    bool        getImage(depthImage& Frame, Stamp* Stamp, streamFrameListener *sourceFrame);
    bool        setResolution(int w, int h, openni::VideoStream& stream);
    bool        setFOV(double horizontalFov, double verticalFov, openni::VideoStream& stream);
    bool        setIntrinsic(yarp::os::Property& intrinsic, const yarp::sig::IntrinsicParams& values);
    void        settingErrorMsg(const std::string& error, bool& ret);

    //properties
    openni::VideoStream             m_depthStream;
    openni::VideoStream             m_imageStream;
    openni::Device                  m_device;
    streamFrameListener*      m_depthFrame;
    streamFrameListener*      m_imageFrame;
    std::string           m_lastError;
    yarp::dev::RGBDSensorParamParser* m_paramParser;
    bool                            m_depthRegistration;
    std::vector<cameraFeature_id_t> m_supportedFeatures;
};
#endif
