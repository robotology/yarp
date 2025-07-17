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

struct
{
  unsigned int  VideoModeMaskDC1394 = 10;
  unsigned int VideoModeDC1394 = 11;
  unsigned int fpsmask = 12;
  unsigned int fps = 13;
  unsigned int iso = 14;
  unsigned int colorcoding = 15;
  unsigned int colorcodingmask = 16;
  bool operation = false;
  bool transmission = false;
  unsigned int bpp = 100;

} m_controlsDC1394;

//namespace {
//YARP_LOG_COMPONENT(FAKEFRAMEGRABBER, "yarp.device.fakeFrameGrabber")
//}

ReturnValue FakeFrameGrabber::getVideoModeMaskDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    val = m_controlsDC1394.VideoModeMaskDC1394;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getVideoModeDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    val = m_controlsDC1394.VideoModeDC1394;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setVideoModeDC1394(int video_mode)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controlsDC1394.VideoModeDC1394 = video_mode;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getFPSMaskDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    val = m_controlsDC1394.fpsmask;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getFPSDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    val = m_controlsDC1394.fps;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setFPSDC1394(int fps)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controlsDC1394.fps = fps;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getISOSpeedDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    val = m_controlsDC1394.iso;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setISOSpeedDC1394(int speed)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controlsDC1394.iso = speed;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getColorCodingMaskDC1394(unsigned int video_mode,unsigned int& val)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    val = m_controlsDC1394.colorcodingmask;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getColorCodingDC1394(unsigned int& val)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    val = m_controlsDC1394.colorcoding;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setColorCodingDC1394(int coding)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controlsDC1394.colorcoding = coding;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setOperationModeDC1394(bool b1394b)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controlsDC1394.operation = b1394b;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getOperationModeDC1394(bool& b1394)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    b1394 = m_controlsDC1394.operation;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setTransmissionDC1394(bool bTxON)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controlsDC1394.transmission = bTxON;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getTransmissionDC1394(bool& bTxON)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    bTxON = m_controlsDC1394.transmission;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setBroadcastDC1394(bool onoff)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setDefaultsDC1394()
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setResetDC1394()
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setPowerDC1394(bool onoff)
{
    return ReturnValue_ok;
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
}

ReturnValue FakeFrameGrabber::setCaptureDC1394(bool bON)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getBytesPerPacketDC1394(unsigned int& bpp)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    bpp = m_controlsDC1394.bpp;
    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::setBytesPerPacketDC1394(unsigned int bpp)
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    m_controlsDC1394.bpp = bpp;
    return ReturnValue_ok;
}
