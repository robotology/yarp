/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef YARP_DEVICE_USBCAMERA_COMMON_USBCAMERA_H
#define YARP_DEVICE_USBCAMERA_COMMON_USBCAMERA_H


#include <yarp/os/Bottle.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/IVisualParams.h>
#include <yarp/dev/IPreciselyTimed.h>

/**
 *  Yarp device driver implementation for acquiring images from USB cameras
 */
class USBCameraDriver :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IFrameGrabber,
        public yarp::dev::IFrameGrabberRgb,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IRgbVisualParams
{
    USBCameraDriver(const USBCameraDriver&) = delete;
    void operator=(const USBCameraDriver&) = delete;

protected:
    yarp::dev::IFrameGrabberRgb* deviceRgb;
    yarp::dev::IPreciselyTimed* deviceTimed;
    yarp::dev::IFrameGrabber* deviceRaw;
    yarp::dev::DeviceDriver* os_device;
    yarp::dev::IFrameGrabberControls* deviceControls;
    yarp::dev::IRgbVisualParams* deviceRgbVisualParam;

    size_t _width;
    size_t _height;
    int pixelType;

public:
    /**
     * Constructor.
     */
    USBCameraDriver();

    /**
     * Destructor.
     */
    ~USBCameraDriver() override;

    /**
     * Open the device driver.
     * @param config configuration for the device driver
     * @return returns true on success, false on failure.
     */
    bool open(yarp::os::Searchable& config) override;

    /**
     * Closes the device driver.
     * @return returns true/false on success/failure.
     */
    bool close(void) override;

    /**
     * Implements FrameGrabber basic interface.
     */
    int height() const override;

    /**
     * Implements FrameGrabber basic interface.
     */
    int width() const override;

    /**
     * Implements FrameGrabber basic interface.
     * @param buffer the pointer to the array to store the last frame.
     * @return returns true/false on success/failure.
     */
    bool getRawBuffer(unsigned char* buffer) override;

    /**
     * Implements the Frame grabber basic interface.
     * @return the size of the raw buffer (for the Dragonfly
     * camera this is 1x640x480).
     */
    int getRawBufferSize() override;

    /**
     * FrameGrabber bgr interface, returns the last acquired frame as
     * a buffer of bgr triplets. A demosaicking method is applied to
     * reconstuct the color from the Bayer pattern of the sensor.
     * @param buffer pointer to the array that will contain the last frame.
     * @return true/false upon success/failure
     */
    bool getRgbBuffer(unsigned char* buffer) override;

    /**
     * Implements the IPreciselyTimed interface.
     * @return the yarp::os::Stamp of the last image acquired
     */
    yarp::os::Stamp getLastInputStamp() override;

    /**
     * Implementation of IFrameGrabberControls2 interface
     *
     * Actual function will be implemented by OS specific devices
     */
    bool getCameraDescription(CameraDescriptor* camera) override;
    bool hasFeature(int feature, bool* hasFeature) override;
    bool setFeature(int feature, double value) override;
    bool getFeature(int feature, double* value) override;
    bool setFeature(int feature, double value1, double value2) override;
    bool getFeature(int feature, double* value1, double* value2) override;
    bool hasOnOff(int feature, bool* HasOnOff) override;
    bool setActive(int feature, bool onoff) override;
    bool getActive(int feature, bool* isActive) override;
    bool hasAuto(int feature, bool* hasAuto) override;
    bool hasManual(int feature, bool* hasManual) override;
    bool hasOnePush(int feature, bool* hasOnePush) override;
    bool setMode(int feature, FeatureMode mode) override;
    bool getMode(int feature, FeatureMode* mode) override;
    bool setOnePush(int feature) override;
    /**
     * Return the height of each frame.
     * @return rgb image height
     */
    int getRgbHeight() override;

    /**
     * Return the width of each frame.
     * @return rgb image width
     */
    int getRgbWidth() override;

    /**
     * Get the possible configurations of the camera
     * @param configurations  list of camera supported configurations as CameraConfig type
     * @return true on success
     */

    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig>& configurations) override;
    /**
     * Get the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */

    bool getRgbResolution(int& width, int& height) override;

    /**
     * Set the resolution of the rgb image from the camera
     * @param width  image width
     * @param height image height
     * @return true on success
     */

    bool setRgbResolution(int width, int height) override;

    /**
     * Get the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will return the value of the horizontal fov in degrees
     * @param  verticalFov   will return the value of the vertical fov in degrees
     * @return true on success
     */
    bool getRgbFOV(double& horizontalFov, double& verticalFov) override;

    /**
     * Set the field of view (FOV) of the rgb camera.
     *
     * @param  horizontalFov will set the value of the horizontal fov in degrees
     * @param  verticalFov   will set the value of the vertical fov in degrees
     * @return true on success
     */
    bool setRgbFOV(double horizontalFov, double verticalFov) override;

    /**
     * Get the intrinsic parameters of the rgb camera
     * @param  intrinsic  return a Property containing intrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
     *
     * Look at IVisualParams.h for more details
     */
    bool getRgbIntrinsicParam(yarp::os::Property& intrinsic) override;

    /**
     * Get the mirroring setting of the sensor
     *
     * @param mirror: true if image is mirrored, false otherwise
     * @return true if success
     */
    bool getRgbMirroring(bool& mirror) override;

    /**
     * Set the mirroring setting of the sensor
     *
     * @param mirror: true if image should be mirrored, false otherwise
     * @return true if success
     */
    bool setRgbMirroring(bool mirror) override;
};


class USBCameraDriverRgb :
        public USBCameraDriver,
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::IFrameGrabberImageRaw
{
private:
    USBCameraDriverRgb(const USBCameraDriverRgb&) = delete;
    void operator=(const USBCameraDriverRgb&) = delete;

public:
    USBCameraDriverRgb();
    ~USBCameraDriverRgb() override;

    /**
     * FrameGrabber image interface, returns the last acquired frame as
     * an rgb image. A demosaicking method is applied to
     * reconstuct the color from the Bayer pattern of the sensor.
     * @param image that will store the last frame.
     * @return true/false upon success/failure
     */
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

    /**
     * FrameGrabber image interface, returns the last acquired frame as
     * a raw image.
     * @param image that will store the last frame.
     * @return true/false upon success/failure
     */
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;

    /**
     * Return the height of each frame.
     * @return image height
     */
    int height() const override;

    /**
     * Return the width of each frame.
     * @return image width
     */
    int width() const override;
};


class USBCameraDriverRaw :
        public USBCameraDriver,
        public yarp::dev::IFrameGrabberImageRaw
{
private:
    USBCameraDriverRaw(const USBCameraDriverRaw&) = delete;
    void operator=(const USBCameraDriverRaw&) = delete;

public:
    USBCameraDriverRaw();
    ~USBCameraDriverRaw() override;

    /**
     * FrameGrabber image interface, returns the last acquired frame as
     * an rgb image. A demosaicking method is applied to
     * reconstuct the color from the Bayer pattern of the sensor.
     * @param image that will store the last frame.
     * @return true/false upon success/failure
     */
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;

    /**
     * Return the height of each frame.
     * @return image height
     */
    int height() const override;

    /**
     * Return the width of each frame.
     * @return image width
     */
    int width() const override;
};

#endif // YARP_DEVICE_USBCAMERA_COMMON_USBCAMERA_H
