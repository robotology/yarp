/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeDepthCameraDriver.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include <algorithm>
#include <map>
#include <cmath>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(FAKEDEPTHCAMERA, "yarp.device.fakeDepthCamera")
}

FakeDepthCameraDriver::FakeDepthCameraDriver() :
    image(nullptr)
{}

FakeDepthCameraDriver::~FakeDepthCameraDriver() = default;

bool FakeDepthCameraDriver::open(Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    Property cfg;
    cfg.fromString(config.toString());
    cfg.unput("device");
    cfg.put("device", "fakeFrameGrabber");
    testgrabber.open(cfg);
    testgrabber.view(image);

    return true;
}

bool FakeDepthCameraDriver::close()
{
    return true;
}

int FakeDepthCameraDriver::getRgbHeight()
{
    return image->height();
}

int FakeDepthCameraDriver::getRgbWidth()
{
    return image->width();
}

bool FakeDepthCameraDriver::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations)
{
    yCWarning(FAKEDEPTHCAMERA) << "getRgbSupportedConfigurations not implemented yet";
    return false;
}

bool FakeDepthCameraDriver::getRgbResolution(int &width, int &height)
{
    width  = image->width();
    height = image->height();
    return true;
}

bool FakeDepthCameraDriver::setRgbResolution(int width, int height)
{
    return false;
}

bool FakeDepthCameraDriver::setDepthResolution(int width, int height)
{
    return false;
}

bool FakeDepthCameraDriver::setRgbFOV(double horizontalFov, double verticalFov)
{
    m_rgb_Hfov = horizontalFov;
    m_rgb_Vfov = verticalFov;
    return true;
}

bool FakeDepthCameraDriver::setDepthFOV(double horizontalFov, double verticalFov)
{
    m_dep_Hfov = horizontalFov;
    m_dep_Vfov = verticalFov;
    return true;
}

bool FakeDepthCameraDriver::setDepthAccuracy(double in_accuracy)
{
    m_accuracy = in_accuracy;
    return true;
}

bool FakeDepthCameraDriver::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    horizontalFov = m_rgb_Hfov;
    verticalFov   = m_rgb_Vfov;
    return false;
}

bool FakeDepthCameraDriver::getRgbMirroring(bool& mirror)
{
    mirror = false;
    return true;
}

bool FakeDepthCameraDriver::setRgbMirroring(bool mirror)
{
    return false;
}

bool FakeDepthCameraDriver::getRgbIntrinsicParam(Property& intrinsic)
{
    intrinsic.put("physFocalLength", 0.5);
    intrinsic.put("focalLengthX",    512);
    intrinsic.put("focalLengthY",    512);
    intrinsic.put("principalPointX", 235);
    intrinsic.put("principalPointY", 231);
    intrinsic.put("distortionModel", "plumb_bob");
    intrinsic.put("k1", 0);
    intrinsic.put("k2", 0);
    intrinsic.put("t1", 0);
    intrinsic.put("t2", 0);
    intrinsic.put("k3", 0);

    intrinsic.put("stamp", yarp::os::Time::now());
    return true;
}

int  FakeDepthCameraDriver::getDepthHeight()
{
    return image->height();
}

int  FakeDepthCameraDriver::getDepthWidth()
{
    return image->width();
}

bool FakeDepthCameraDriver::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    horizontalFov = m_dep_Hfov;
    verticalFov   = m_dep_Vfov;
    return false;
}

bool FakeDepthCameraDriver::getDepthIntrinsicParam(Property& intrinsic)
{
    intrinsic.put("physFocalLength", 0.5);
    intrinsic.put("focalLengthX",    512);
    intrinsic.put("focalLengthY",    512);
    intrinsic.put("principalPointX", 235);
    intrinsic.put("principalPointY", 231);
    intrinsic.put("distortionModel", "plumb_bob");
    intrinsic.put("k1", 0);
    intrinsic.put("k2", 0);
    intrinsic.put("t1", 0);
    intrinsic.put("t2", 0);
    intrinsic.put("k3", 0);

    intrinsic.put("stamp", yarp::os::Time::now());
    return true;
}

double FakeDepthCameraDriver::getDepthAccuracy()
{
    return m_accuracy;
}

bool FakeDepthCameraDriver::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    nearPlane = m_dep_near;
    farPlane  = m_dep_far;
    return true;
}

bool FakeDepthCameraDriver::setDepthClipPlanes(double nearPlane, double farPlane)
{
    m_dep_near = nearPlane;
    m_dep_far  = farPlane;
    return true;
}

bool FakeDepthCameraDriver::getDepthMirroring(bool& mirror)
{
    mirror = false;
    return true;
}

bool FakeDepthCameraDriver::setDepthMirroring(bool mirror)
{
    return false;
}

bool FakeDepthCameraDriver::getExtrinsicParam(Matrix& extrinsic)
{
    extrinsic.resize(4, 4);
    extrinsic.zero();

    extrinsic[0][0] = 1;
    extrinsic[1][1] = 1;
    extrinsic[2][2] = 1;
    extrinsic[3][3] = 1;
    return true;
}

bool FakeDepthCameraDriver::getRgbImage(FlexImage& rgbImage, Stamp* timeStamp)
{
    if (!image->getImage(imageof)) {return false;}
    rgbImage.setPixelCode(VOCAB_PIXEL_RGB);
    rgbImage.resize(imageof);
    memcpy((void*)rgbImage.getRawImage(), (void*)imageof.getRawImage(), imageof.getRawImageSize());
    if (timeStamp) {
        timeStamp->update(yarp::os::Time::now());
    }
    return true;
}

bool FakeDepthCameraDriver::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
{
    if (!image->getImage(imageof)) {return false;}
    depthImage.resize(imageof);
    for (size_t i = 0; i < imageof.width(); i++)
    {
        for (size_t j = 0; j < imageof.height(); j++)
        {
            PixelRgb pix = (*(PixelRgb*)imageof.getPixelAddress(i, j));
            *(PixelFloat*)depthImage.getPixelAddress(i, j) = (float(pix.b) / 255.0)/3.0 + (float(pix.g) / 255.0) / 3.0 + (float(pix.r) / 255.0) / 3.0;
        }
    }
    if (timeStamp) {
        timeStamp->update(yarp::os::Time::now());
    }
    return true;
}

bool FakeDepthCameraDriver::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    return getRgbImage(colorFrame, colorStamp) & getDepthImage(depthFrame, depthStamp);
}

IRGBDSensor::RGBDSensor_status FakeDepthCameraDriver::getSensorStatus()
{
    return RGBD_SENSOR_OK_IN_USE;
}

std::string FakeDepthCameraDriver::getLastErrorMsg(Stamp* timeStamp)
{
    return "no error";
}
