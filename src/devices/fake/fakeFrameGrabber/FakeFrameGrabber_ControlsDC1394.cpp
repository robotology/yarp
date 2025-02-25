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

ReturnValue FakeFrameGrabber::getVideoModeMaskDC1394(unsigned int& val)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getVideoModeDC1394(unsigned int& val)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setVideoModeDC1394(int video_mode)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getFPSMaskDC1394(unsigned int& val)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getFPSDC1394(unsigned int& val)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setFPSDC1394(int fps)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getISOSpeedDC1394(unsigned int& val)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setISOSpeedDC1394(int speed)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getColorCodingMaskDC1394(unsigned int video_mode,unsigned int& val)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getColorCodingDC1394(unsigned int& val)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setColorCodingDC1394(int coding) 
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setOperationModeDC1394(bool b1394b)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getOperationModeDC1394(bool& b1394)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setTransmissionDC1394(bool bTxON)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getTransmissionDC1394(bool& bTxON)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setBroadcastDC1394(bool onoff)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setDefaultsDC1394()
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setResetDC1394()
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setPowerDC1394(bool onoff)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setCaptureDC1394(bool bON)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::getBytesPerPacketDC1394(unsigned int& bpp)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }

ReturnValue FakeFrameGrabber::setBytesPerPacketDC1394(unsigned int bpp)
{ return ReturnValue::return_code::return_value_error_not_implemented_by_device; }
