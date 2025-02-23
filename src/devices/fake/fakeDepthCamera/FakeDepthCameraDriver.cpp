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

FakeDepthCameraDriver::FakeDepthCameraDriver()
{
}

FakeDepthCameraDriver::~FakeDepthCameraDriver() = default;

bool FakeDepthCameraDriver::open(Searchable& config)
{
    yCDebug(FAKEDEPTHCAMERA) << "Opening Device";
    if (!this->parseParams(config)) {return false;}

    regenerate_rgb_image();
    regenerate_depth_image();
    return true;
}

bool FakeDepthCameraDriver::close()
{
    yCDebug(FAKEDEPTHCAMERA) << "Closing Device";
    return true;
}

ReturnValue FakeDepthCameraDriver::getCameraDescription(yarp::dev::CameraDescriptor& camera)
{
    camera.busType = BusType::BUS_USB;
    camera.deviceDescription = "fakeCameraDevice";
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::hasFeature(cameraFeature_id_t feature, bool& hasFeature)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::setFeature(cameraFeature_id_t feature, double value)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::getFeature(cameraFeature_id_t feature, double& value)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::setFeature(cameraFeature_id_t feature, double value1, double value2)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::getFeature(cameraFeature_id_t feature, double& value1, double& value2)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::hasOnOff(cameraFeature_id_t feature, bool& HasOnOff)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::setActive(cameraFeature_id_t feature, bool onoff)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::getActive(cameraFeature_id_t feature, bool& isActive)
{
    //not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::hasAuto(cameraFeature_id_t feature, bool& hasAuto)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::hasManual(cameraFeature_id_t feature, bool& hasManual)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::hasOnePush(cameraFeature_id_t feature, bool& hasOnePush)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::setMode(cameraFeature_id_t feature, yarp::dev::FeatureMode mode)
{
    // not yet implemented
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::getMode(cameraFeature_id_t feature, yarp::dev::FeatureMode& mode)
{
    mode = yarp::dev::FeatureMode::MODE_AUTO;
    return ReturnValue_ok;
}

ReturnValue FakeDepthCameraDriver::setOnePush(cameraFeature_id_t feature)
{
    // not yet implemented
    return ReturnValue_ok;
}
