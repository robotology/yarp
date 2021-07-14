/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeDepthCameraDriver.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include <algorithm>
#include <map>
#include <cmath>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace std;

namespace {
YARP_LOG_COMPONENT(FAKEDEPTHCAMERA, "yarp.device.fakeDepthCamera")
}

fakeDepthCameraDriver::fakeDepthCameraDriver() :
    rgb_h(480),
    rgb_w(640),
    dep_h(480),
    dep_w(640),
    rgb_Vfov(36),
    rgb_Hfov(50),
    dep_Vfov(36),
    dep_Hfov(50),
    dep_far(6),
    image(nullptr)
{}

fakeDepthCameraDriver::~fakeDepthCameraDriver() = default;

bool fakeDepthCameraDriver::open(Searchable& config)
{
    Property cfg;
    cfg.fromString(config.toString());
    cfg.unput("device");
    cfg.put("device", "fakeFrameGrabber");
    testgrabber.open(cfg);
    testgrabber.view(image);

    vector<tuple<double*, string, double> > param;
    param.emplace_back(&rgb_h,    "rgb_h",    480.0);
    param.emplace_back(&rgb_w,    "rgb_w",    640.0);
    param.emplace_back(&dep_h,    "rgb_h",    480.0);
    param.emplace_back(&dep_w,    "rgb_w",    640.0);
    param.emplace_back(&accuracy, "accuracy", 0.001);
    param.emplace_back(&rgb_Vfov, "rgb_Vfov", 50.0);
    param.emplace_back(&rgb_Hfov, "rgb_Hfov", 36.0);
    param.emplace_back(&dep_Vfov, "dep_Vfov", 50.0);
    param.emplace_back(&dep_Hfov, "dep_Hfov", 36.0);
    param.emplace_back(&dep_near, "dep_near", 0.2);
    param.emplace_back(&dep_far,  "dep_far",  6.0);
    for (auto p : param)
    {
        if (config.check(get<1>(p)))
        {
            *get<0>(p) = config.find(get<1>(p)).asFloat64();
        }
        else
        {
            *get<0>(p) = get<2>(p);
        }

    }

    return true;
}

bool fakeDepthCameraDriver::close()
{
    return true;
}

int fakeDepthCameraDriver::getRgbHeight()
{
    return image->height();
}

int fakeDepthCameraDriver::getRgbWidth()
{
    return image->width();
}

bool fakeDepthCameraDriver::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations)
{
    yCWarning(FAKEDEPTHCAMERA) << "getRgbSupportedConfigurations not implemented yet";
    return false;
}

bool fakeDepthCameraDriver::getRgbResolution(int &width, int &height)
{
    width  = image->width();
    height = image->height();
    return true;
}

bool fakeDepthCameraDriver::setRgbResolution(int width, int height)
{
    return false;
}

bool fakeDepthCameraDriver::setDepthResolution(int width, int height)
{
    return false;
}

bool fakeDepthCameraDriver::setRgbFOV(double horizontalFov, double verticalFov)
{
    rgb_Hfov = horizontalFov;
    rgb_Vfov = verticalFov;
    return true;
}

bool fakeDepthCameraDriver::setDepthFOV(double horizontalFov, double verticalFov)
{
    dep_Hfov = horizontalFov;
    dep_Vfov = verticalFov;
    return true;
}

bool fakeDepthCameraDriver::setDepthAccuracy(double in_accuracy)
{
    accuracy = in_accuracy;
    return true;
}

bool fakeDepthCameraDriver::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    horizontalFov = rgb_Hfov;
    verticalFov   = rgb_Vfov;
    return false;
}

bool fakeDepthCameraDriver::getRgbMirroring(bool& mirror)
{
    mirror = false;
    return true;
}

bool fakeDepthCameraDriver::setRgbMirroring(bool mirror)
{
    return false;
}

bool fakeDepthCameraDriver::getRgbIntrinsicParam(Property& intrinsic)
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

int  fakeDepthCameraDriver::getDepthHeight()
{
    return image->height();
}

int  fakeDepthCameraDriver::getDepthWidth()
{
    return image->width();
}

bool fakeDepthCameraDriver::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    horizontalFov = dep_Hfov;
    verticalFov   = dep_Vfov;
    return false;
}

bool fakeDepthCameraDriver::getDepthIntrinsicParam(Property& intrinsic)
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

double fakeDepthCameraDriver::getDepthAccuracy()
{
    return accuracy;
}

bool fakeDepthCameraDriver::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    nearPlane = dep_near;
    farPlane  = dep_far;
    return true;
}

bool fakeDepthCameraDriver::setDepthClipPlanes(double nearPlane, double farPlane)
{
    dep_near = nearPlane;
    dep_far  = farPlane;
    return true;
}

bool fakeDepthCameraDriver::getDepthMirroring(bool& mirror)
{
    mirror = false;
    return true;
}

bool fakeDepthCameraDriver::setDepthMirroring(bool mirror)
{
    return false;
}

bool fakeDepthCameraDriver::getExtrinsicParam(Matrix& extrinsic)
{
    extrinsic.resize(4, 4);
    extrinsic.zero();

    extrinsic[0][0] = 1;
    extrinsic[1][1] = 1;
    extrinsic[2][2] = 1;
    extrinsic[3][3] = 1;
    return true;
}

bool fakeDepthCameraDriver::getRgbImage(FlexImage& rgbImage, Stamp* timeStamp)
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

bool fakeDepthCameraDriver::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
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

bool fakeDepthCameraDriver::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    return getRgbImage(colorFrame, colorStamp) & getDepthImage(depthFrame, depthStamp);
}

IRGBDSensor::RGBDSensor_status fakeDepthCameraDriver::getSensorStatus()
{
    return RGBD_SENSOR_OK_IN_USE;
}

std::string fakeDepthCameraDriver::getLastErrorMsg(Stamp* timeStamp)
{
    return "no error";
}
