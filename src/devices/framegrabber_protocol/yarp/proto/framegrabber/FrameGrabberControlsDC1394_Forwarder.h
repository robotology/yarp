/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLSDC1394_FORWARDER_H
#define YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLSDC1394_FORWARDER_H

#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/os/Port.h>

namespace yarp {
namespace proto {
namespace framegrabber {

class FrameGrabberControlsDC1394_Forwarder :
        public yarp::dev::IFrameGrabberControlsDC1394
{
public:
    FrameGrabberControlsDC1394_Forwarder(yarp::os::Port& port);
    ~FrameGrabberControlsDC1394_Forwarder() override = default;

    unsigned int getVideoModeMaskDC1394() override;
    unsigned int getVideoModeDC1394() override;
    bool setVideoModeDC1394(int video_mode) override;
    unsigned int getFPSMaskDC1394() override;
    unsigned int getFPSDC1394() override;
    bool setFPSDC1394(int fps) override;
    unsigned int getISOSpeedDC1394() override;
    bool setISOSpeedDC1394(int speed) override;
    unsigned int getColorCodingMaskDC1394(unsigned int video_mode) override;
    unsigned int getColorCodingDC1394() override;
    bool setColorCodingDC1394(int coding) override;
    bool getFormat7MaxWindowDC1394(unsigned int& xdim,
                                   unsigned int& ydim,
                                   unsigned int& xstep,
                                   unsigned int& ystep,
                                   unsigned int& xoffstep,
                                   unsigned int& yoffstep) override;
    bool getFormat7WindowDC1394(unsigned int& xdim,
                                unsigned int& ydim,
                                int& x0,
                                int& y0) override;
    bool setFormat7WindowDC1394(unsigned int xdim,
                                unsigned int ydim,
                                int x0,
                                int y0) override;
    bool setOperationModeDC1394(bool b1394b) override;
    bool getOperationModeDC1394() override;
    bool setTransmissionDC1394(bool bTxON) override;
    bool getTransmissionDC1394() override;
    bool setBroadcastDC1394(bool onoff) override;
    bool setDefaultsDC1394() override;
    bool setResetDC1394() override;
    bool setPowerDC1394(bool onoff) override;
    bool setCaptureDC1394(bool bON) override;
    bool setBytesPerPacketDC1394(unsigned int bpp) override;
    unsigned int getBytesPerPacketDC1394() override;

private:
    yarp::os::Port& m_port;

    bool setCommand(int code, double v);
    bool setCommand(int code, double b, double r);
    double getCommand(int code) const;
    bool getCommand(int code, double& b, double& r) const;
};

} // namespace framegrabber
} // namespace proto
} // namespace yarp

#endif // YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLSDC1394_FORWARDER_H
