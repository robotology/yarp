/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeFrameGrabber.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <cstdio>
#include <thread>
#include <random>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKEFRAMEGRABBER, "yarp.device.fakeFrameGrabber")
}

ReturnValue FakeFrameGrabber::getCameraDescription(CameraDescriptor& camera)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::hasFeature(int feature, bool& hasFeature)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setFeature(int feature, double value)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getFeature(int feature, double& value)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setFeature(int feature, double  value1, double  value2)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getFeature(int feature, double& value1, double& value2)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::hasOnOff(int feature, bool& HasOnOff)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setActive(int feature, bool onoff)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getActive(int feature, bool& isActive)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::hasAuto(int feature, bool& hasAuto)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::hasManual(int feature, bool& hasManual)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::hasOnePush(int feature, bool& hasOnePush)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setMode(int feature, FeatureMode mode)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getMode(int feature, FeatureMode& mode)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setOnePush(int feature)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }
