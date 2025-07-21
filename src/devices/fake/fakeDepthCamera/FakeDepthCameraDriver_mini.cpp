/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeDepthCameraDriver_mini.h"

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
YARP_LOG_COMPONENT(FAKEDEPTHCAMERA, "yarp.device.fakeDepthCamera_mini")
}

FakeDepthCameraDriver_mini::FakeDepthCameraDriver_mini()
{
}

FakeDepthCameraDriver_mini::~FakeDepthCameraDriver_mini() = default;

bool FakeDepthCameraDriver_mini::open(Searchable& config)
{
    yCDebug(FAKEDEPTHCAMERA) << "Opening Device";
    if (!this->parseParams(config)) {return false;}

    regenerate_rgb_image();
    regenerate_depth_image();
    return true;
}

bool FakeDepthCameraDriver_mini::close()
{
    yCDebug(FAKEDEPTHCAMERA) << "Closing Device";
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    return true;
}

int FakeDepthCameraDriver_mini::getRgbHeight()
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    return m_rgb_h;
}

int FakeDepthCameraDriver_mini::getRgbWidth()
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    return m_rgb_w;
}

ReturnValue FakeDepthCameraDriver_mini::getRgbSupportedConfigurations(std::vector<CameraConfig>& configurations)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    CameraConfig cfg;
    configurations.push_back(cfg);
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getRgbResolution(int& width, int& height)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    width = m_rgb_w;
    height = m_rgb_h;
    return ReturnValue_ok;
}

void FakeDepthCameraDriver_mini::regenerate_rgb_image()
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_rgbImage.setPixelCode(VOCAB_PIXEL_RGB);
    m_rgbImage.setQuantum(1);
    m_rgbImage.resize(m_rgb_w, m_rgb_h);
    ImageOf<PixelRgb>& rgbImg = (ImageOf<PixelRgb>&)m_rgbImage;
    for (size_t y = 0; y < m_rgb_h; y++)
        for (size_t x = 0; x < m_rgb_w; x++)
        {
            rgbImg.pixel(x, y).r = x * 255.0 / m_rgb_w;
            rgbImg.pixel(x, y).g = 0;
            rgbImg.pixel(x, y).b = y * 255.0 / m_rgb_h;
        }
}

void FakeDepthCameraDriver_mini::regenerate_depth_image()
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_depthImage.resize(m_dep_w, m_dep_h);
    size_t totalPixels = m_dep_w * m_dep_h;
    size_t index = 0;
    for (size_t y = 0; y < m_dep_h; y++)
        for (size_t x = 0; x < m_dep_w; x++) {
            auto& p = m_depthImage.pixel(x, y);
            p = static_cast<double>(index) / (totalPixels - 1);
            ++index;
        }
}

ReturnValue FakeDepthCameraDriver_mini::setRgbResolution(int width, int height)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_rgb_w = width;
    m_rgb_h = height;
    regenerate_rgb_image();
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getDepthResolution(int& width, int& height)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    width = m_dep_w;
    height = m_dep_h;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::setDepthResolution(int width, int height)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_dep_w = width;
    m_dep_h = height;
    regenerate_depth_image();
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::setRgbFOV(double horizontalFov, double verticalFov)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_rgb_Hfov = horizontalFov;
    m_rgb_Vfov = verticalFov;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::setDepthFOV(double horizontalFov, double verticalFov)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_dep_Hfov = horizontalFov;
    m_dep_Vfov = verticalFov;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::setDepthAccuracy(double in_accuracy)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_accuracy = in_accuracy;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getRgbFOV(double& horizontalFov, double& verticalFov)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    horizontalFov = m_rgb_Hfov;
    verticalFov   = m_rgb_Vfov;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getRgbMirroring(bool& mirror)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    mirror = m_rgb_mirror;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::setRgbMirroring(bool mirror)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_rgb_mirror = mirror;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getRgbIntrinsicParam(Property& intrinsic)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
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
    return ReturnValue_ok;
}

int FakeDepthCameraDriver_mini::getDepthHeight()
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    return m_dep_h;
}

int FakeDepthCameraDriver_mini::getDepthWidth()
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    return m_dep_w;
}

ReturnValue FakeDepthCameraDriver_mini::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    horizontalFov = m_dep_Hfov;
    verticalFov   = m_dep_Vfov;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getDepthIntrinsicParam(Property& intrinsic)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
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
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getDepthAccuracy(double& accuracy)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    accuracy = m_accuracy;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    nearPlane = m_dep_near;
    farPlane  = m_dep_far;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::setDepthClipPlanes(double nearPlane, double farPlane)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_dep_near = nearPlane;
    m_dep_far  = farPlane;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getDepthMirroring(bool& mirror)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    mirror = m_depth_mirror;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::setDepthMirroring(bool _mirror)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    m_depth_mirror = _mirror;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getExtrinsicParam(Matrix& extrinsic)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    extrinsic.resize(4, 4);
    extrinsic.zero();

    extrinsic[0][0] = 1;
    extrinsic[1][1] = 1;
    extrinsic[2][2] = 1;
    extrinsic[3][3] = 1;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getRgbImage(FlexImage& rgbImage, Stamp* timeStamp)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    rgbImage = m_rgbImage;
    if (timeStamp) {
        timeStamp->update(yarp::os::Time::now());
    }
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    depthImage = m_depthImage;
    if (timeStamp) {
        timeStamp->update(yarp::os::Time::now());
    }
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    auto r1 = getRgbImage(colorFrame, colorStamp);
    auto r2 = getDepthImage(depthFrame, depthStamp);
    ReturnValue rr = r1 && r2;
    return rr;
}

ReturnValue FakeDepthCameraDriver_mini::getSensorStatus(IRGBDSensor::RGBDSensor_status& status)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    status = RGBD_SENSOR_OK_IN_USE;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver_mini::getLastErrorMsg(std::string& msg, Stamp* timeStamp)
{
    std::lock_guard <std::recursive_mutex> lg(m_mutex);
    msg = std::string("no error");
    return ReturnValue_ok;
}
