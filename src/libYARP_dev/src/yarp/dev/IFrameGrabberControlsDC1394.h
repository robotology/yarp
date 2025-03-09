/*
 * SPDX-FileCopyrightText: 2006-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMEGRABBERCONTROLSDC1394_H
#define YARP_DEV_IFRAMEGRABBERCONTROLSDC1394_H

#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {

/**
 * @ingroup dev_iface_media
 *
 * Control interface for frame grabber devices that conform to the 1394-based Digital Camera Specifications.
 */
class YARP_dev_API IFrameGrabberControlsDC1394
{
public:
    virtual ~IFrameGrabberControlsDC1394();

    // 12 13 14
    virtual yarp::dev::ReturnValue getVideoModeMaskDC1394(unsigned int& val)=0;
    virtual yarp::dev::ReturnValue getVideoModeDC1394(unsigned int& val)=0;
    virtual yarp::dev::ReturnValue setVideoModeDC1394(int video_mode)=0;

    // 15 16 17
    virtual yarp::dev::ReturnValue getFPSMaskDC1394(unsigned int& val)=0;
    virtual yarp::dev::ReturnValue getFPSDC1394(unsigned int& val)=0;
    virtual yarp::dev::ReturnValue setFPSDC1394(int fps)=0;

    // 18 19
    virtual yarp::dev::ReturnValue getISOSpeedDC1394(unsigned int& val)=0;
    virtual yarp::dev::ReturnValue setISOSpeedDC1394(int speed)=0;

    // 20 21 22
    virtual yarp::dev::ReturnValue getColorCodingMaskDC1394(unsigned int video_mode,unsigned int& val)=0;
    virtual yarp::dev::ReturnValue getColorCodingDC1394(unsigned int& val)=0;
    virtual yarp::dev::ReturnValue setColorCodingDC1394(int coding)=0;

    // 25 26 27
    virtual yarp::dev::ReturnValue getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep)=0;
    virtual yarp::dev::ReturnValue getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0)=0;
    virtual yarp::dev::ReturnValue setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0)=0;

    // 28 29 30 31
    virtual yarp::dev::ReturnValue setOperationModeDC1394(bool b1394b)=0;
    virtual yarp::dev::ReturnValue getOperationModeDC1394(bool& b1394)=0;
    virtual yarp::dev::ReturnValue setTransmissionDC1394(bool bTxON)=0;
    virtual yarp::dev::ReturnValue getTransmissionDC1394(bool& bTxON)=0;
    // 32
    //virtual yarp::dev::ReturnValue setBayerDC1394(bool bON)=0;
    // 33
    //virtual yarp::dev::ReturnValue getBayerDC1394()=0;

    // 34 35 36 37
    virtual yarp::dev::ReturnValue setBroadcastDC1394(bool onoff)=0;
    virtual yarp::dev::ReturnValue setDefaultsDC1394()=0;
    virtual yarp::dev::ReturnValue setResetDC1394()=0;
    virtual yarp::dev::ReturnValue setPowerDC1394(bool onoff)=0;

    // 38
    virtual yarp::dev::ReturnValue setCaptureDC1394(bool bON)=0;

    // 39
    virtual yarp::dev::ReturnValue getBytesPerPacketDC1394(unsigned int& bpp)=0;

    // 40
    virtual yarp::dev::ReturnValue setBytesPerPacketDC1394(unsigned int bpp)=0;
};

} // namespace yarp::dev

#endif // YARP_DEV_IFRAMEGRABBERCONTROLSDC1394_H
