/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


#ifndef YARP_DEVICE_USBCAMERA_COMMON_USBCAMERA_H
#define YARP_DEVICE_USBCAMERA_COMMON_USBCAMERA_H


#include <yarp/os/Bottle.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IRgbVisualParams.h>
#include <yarp/dev/IPreciselyTimed.h>

/**
 * @ingroup dev_impl_media
 *
 * \brief `usbCamera`: YARP device driver implementation for acquiring images
 * from USB cameras.
 *
 * Documentation to be added
 */
class USBCameraDriver :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IRgbVisualParams
{
    USBCameraDriver(const USBCameraDriver&) = delete;
    void operator=(const USBCameraDriver&) = delete;

protected:
    yarp::dev::IPreciselyTimed* deviceTimed;
    yarp::dev::IFrameGrabberImage* frameGrabberImage;
    yarp::dev::IFrameGrabberImageRaw* frameGrabberImageRaw;
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

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    int height() const;
    int width() const;

    yarp::os::Stamp getLastInputStamp() override;

    /**
     * Implementation of IFrameGrabberControls2 interface
     *
     * Actual function will be implemented by OS specific devices
     */
    yarp::dev::ReturnValue getCameraDescription(yarp::dev::CameraDescriptor& camera) override;
    yarp::dev::ReturnValue hasFeature(yarp::dev::cameraFeature_id_t feature, bool& hasFeature) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double value) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double&value) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double  value1, double  value2) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double& value1, double& value2) override;
    yarp::dev::ReturnValue setActive(yarp::dev::cameraFeature_id_t feature, bool onoff) override;
    yarp::dev::ReturnValue getActive(yarp::dev::cameraFeature_id_t feature, bool& isActive) override;
    yarp::dev::ReturnValue hasOnOff(yarp::dev::cameraFeature_id_t feature, bool& HasOnOff) override;
    yarp::dev::ReturnValue hasAuto(yarp::dev::cameraFeature_id_t feature, bool& hasAuto) override;
    yarp::dev::ReturnValue hasManual(yarp::dev::cameraFeature_id_t feature, bool& hasManual) override;
    yarp::dev::ReturnValue setMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode mode) override;
    yarp::dev::ReturnValue getMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode& mode) override;
    yarp::dev::ReturnValue hasOnePush(yarp::dev::cameraFeature_id_t feature, bool& hasOnePush) override;
    yarp::dev::ReturnValue setOnePush(yarp::dev::cameraFeature_id_t feature) override;

    int getRgbHeight() override;
    int getRgbWidth() override;
    yarp::dev::ReturnValue getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig>& configurations) override;
    yarp::dev::ReturnValue getRgbResolution(int& width, int& height) override;
    yarp::dev::ReturnValue setRgbResolution(int width, int height) override;
    yarp::dev::ReturnValue getRgbFOV(double& horizontalFov, double& verticalFov) override;
    yarp::dev::ReturnValue setRgbFOV(double horizontalFov, double verticalFov) override;
    yarp::dev::ReturnValue getRgbIntrinsicParam(yarp::os::Property& intrinsic) override;
    yarp::dev::ReturnValue getRgbMirroring(bool& mirror) override;
    yarp::dev::ReturnValue setRgbMirroring(bool mirror) override;
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

    yarp::dev::ReturnValue getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;
    yarp::dev::ReturnValue getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;
    int height() const override;
    int width() const override;
};

/**
 * @ingroup dev_impl_media
 *
 * \brief `usbCameraRaw`: Documentation to be added
 */
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

    yarp::dev::ReturnValue getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;
    int height() const override;
    int width() const override;
};

#endif // YARP_DEVICE_USBCAMERA_COMMON_USBCAMERA_H
