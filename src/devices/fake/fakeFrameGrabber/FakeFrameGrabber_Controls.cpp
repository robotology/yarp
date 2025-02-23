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

//namespace {
//YARP_LOG_COMPONENT(FAKEFRAMEGRABBER, "yarp.device.fakeFrameGrabber")
//}

struct
{
  bool hasFeature = false;
  double feature  = 0.0;
  double feature1 = 0.0;
  double feature2 = 0.0;
  bool hasOnOff = false;
  bool hasAuto = false;
  bool hasManual = false;
  bool hasOnePush = false;
  bool isActive = false;
  FeatureMode mode;
  CameraDescriptor camera;
} m_controls;

ReturnValue FakeFrameGrabber::getCameraDescription(CameraDescriptor& camera)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    camera = m_controls.camera;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::hasFeature(cameraFeature_id_t feature, bool& hasFeature)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    hasFeature = m_controls.hasFeature;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setFeature(cameraFeature_id_t feature, double value)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controls.feature = value;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getFeature(cameraFeature_id_t feature, double& value)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    value = m_controls.feature;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setFeature(cameraFeature_id_t feature, double value1, double value2)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controls.feature1 = value1;
    m_controls.feature2 = value2;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getFeature(cameraFeature_id_t feature, double& value1, double& value2)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    value1 = m_controls.feature2;
    value2 = m_controls.feature2;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::hasOnOff(cameraFeature_id_t feature, bool& HasOnOff)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    HasOnOff = m_controls.hasOnOff;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setActive(cameraFeature_id_t feature, bool onoff)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controls.isActive = onoff;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getActive(cameraFeature_id_t feature, bool& isActive)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    isActive = m_controls.isActive;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::hasAuto(cameraFeature_id_t feature, bool& hasAuto)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    hasAuto = m_controls.hasAuto;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::hasManual(cameraFeature_id_t feature, bool& hasManual)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    hasManual = m_controls.hasManual;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::hasOnePush(cameraFeature_id_t feature, bool& hasOnePush)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    hasOnePush = m_controls.hasOnePush;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setMode(cameraFeature_id_t feature, FeatureMode mode)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controls.mode = mode;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getMode(cameraFeature_id_t feature, FeatureMode& mode)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    mode = m_controls.mode;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setOnePush(cameraFeature_id_t feature)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controls.hasOnePush = true;
    return ReturnValue_ok;
}
