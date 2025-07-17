/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeFrameGrabber.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/ImageDraw.h>

#include <cstdio>
#include <thread>
#include <random>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::draw;

//namespace {
//YARP_LOG_COMPONENT(FAKEFRAMEGRABBER, "yarp.device.fakeFrameGrabber")
//}

int FakeFrameGrabber::getRgbHeight()
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return m_height;
}

int FakeFrameGrabber::getRgbWidth()
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return m_width;
}

ReturnValue FakeFrameGrabber::getRgbSupportedConfigurations(std::vector<CameraConfig> &configurations)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    configurations=this->configurations;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getRgbResolution(int &width, int &height)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    width=m_width;
    height=m_height;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setRgbResolution(int width, int height)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_width =width;
    m_height =height;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    horizontalFov=this->m_horizontalFov;
    verticalFov=this->m_verticalFov;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setRgbFOV(double horizontalFov, double verticalFov)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    this->m_horizontalFov=horizontalFov;
    this->m_verticalFov=verticalFov;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getRgbIntrinsicParam(yarp::os::Property &intrinsic)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    intrinsic=this->m_intrinsic;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getRgbMirroring(bool &mirror)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    mirror=this->m_mirror;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setRgbMirroring(bool mirror)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    this->m_mirror =mirror;
    return ReturnValue_ok;
}
