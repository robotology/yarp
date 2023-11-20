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

    int getRgbHeight() override;
    int getRgbWidth() override;
    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig>& configurations) override;
    bool getRgbResolution(int& width, int& height) override;
    bool setRgbResolution(int width, int height) override;
    bool getRgbFOV(double& horizontalFov, double& verticalFov) override;
    bool setRgbFOV(double horizontalFov, double verticalFov) override;
    bool getRgbIntrinsicParam(yarp::os::Property& intrinsic) override;
    bool getRgbMirroring(bool& mirror) override;
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

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;
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

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;
    int height() const override;
    int width() const override;
};

#endif // YARP_DEVICE_USBCAMERA_COMMON_USBCAMERA_H
