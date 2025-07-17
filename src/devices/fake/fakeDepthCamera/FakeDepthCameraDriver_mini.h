/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEDEPTHCAMERADRIVER_MINI_H
#define YARP_FAKEDEPTHCAMERADRIVER_MINI_H

#include <string>
#include <mutex>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberImage.h>

#include "FakeDepthCameraDriver_ParamsParser.h"

/**
 * @ingroup dev_impl_fake
 *
 * \brief `fakeDepthCamera_mini`: Documentation to be added
 *
 * Parameters required by this device are shown in class: FakeDepthCameraDriver_ParamsParser
 *
*/

class FakeDepthCameraDriver_mini :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IRGBDSensor,
        public FakeDepthCameraDriver_ParamsParser
{
protected:
    typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;
    typedef yarp::os::Stamp                           Stamp;
    typedef yarp::os::Property                        Property;
    typedef yarp::sig::FlexImage                      FlexImage;

public:
    FakeDepthCameraDriver_mini();
    ~FakeDepthCameraDriver_mini() override;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // IRGBDSensor
    int    getRgbHeight() override;
    int    getRgbWidth() override;
    yarp::dev::ReturnValue   getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig>& configurations) override;
    yarp::dev::ReturnValue   getRgbResolution(int& width, int& height) override;
    yarp::dev::ReturnValue   setRgbResolution(int width, int height) override;
    yarp::dev::ReturnValue   getRgbFOV(double& horizontalFov, double& verticalFov) override;
    yarp::dev::ReturnValue   setRgbFOV(double horizontalFov, double verticalFov) override;
    yarp::dev::ReturnValue   getRgbMirroring(bool& mirror) override;
    yarp::dev::ReturnValue   setRgbMirroring(bool mirror) override;

    yarp::dev::ReturnValue   getRgbIntrinsicParam(Property& intrinsic) override;
    int    getDepthHeight() override;
    int    getDepthWidth() override;
    yarp::dev::ReturnValue   getDepthResolution(int& width, int& height) override;
    yarp::dev::ReturnValue   setDepthResolution(int width, int height) override;
    yarp::dev::ReturnValue   getDepthFOV(double& horizontalFov, double& verticalFov) override;
    yarp::dev::ReturnValue   setDepthFOV(double horizontalFov, double verticalFov) override;
    yarp::dev::ReturnValue   getDepthIntrinsicParam(Property& intrinsic) override;
    yarp::dev::ReturnValue   getDepthAccuracy(double& accuracy) override;
    yarp::dev::ReturnValue   setDepthAccuracy(double accuracy) override;
    yarp::dev::ReturnValue   getDepthClipPlanes(double& nearPlane, double& farPlane) override;
    yarp::dev::ReturnValue   setDepthClipPlanes(double nearPlane, double farPlane) override;
    yarp::dev::ReturnValue   getDepthMirroring(bool& mirror) override;
    yarp::dev::ReturnValue   setDepthMirroring(bool mirror) override;

    yarp::dev::ReturnValue getExtrinsicParam(yarp::sig::Matrix& extrinsic) override;
    yarp::dev::ReturnValue getRgbImage(FlexImage& rgbImage, Stamp* timeStamp = nullptr) override;
    yarp::dev::ReturnValue getDepthImage(depthImage& depthImage, Stamp* timeStamp = nullptr) override;
    yarp::dev::ReturnValue getImages(FlexImage& colorFrame, depthImage& depthFrame, Stamp* colorStamp = nullptr, Stamp* depthStamp = nullptr) override;

    yarp::dev::ReturnValue getSensorStatus(RGBDSensor_status& status) override;
    yarp::dev::ReturnValue getLastErrorMsg(std::string& mesg, Stamp* timeStamp = nullptr) override;

protected:
    FlexImage                  m_rgbImage;
    depthImage                 m_depthImage;
    bool m_depth_mirror                            = false;
    bool m_rgb_mirror                              = false;
    void regenerate_rgb_image();
    void regenerate_depth_image();
    std::recursive_mutex                 m_mutex;

};

#endif // YARP_FAKEDEPTHCAMERADRIVER_MINI_H
