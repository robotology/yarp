/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabberControlsDC1394_Forwarder.h"
#include "CameraVocabs.h"

#include <yarp/os/Bottle.h>

using yarp::proto::framegrabber::FrameGrabberControlsDC1394_Forwarder;

FrameGrabberControlsDC1394_Forwarder::FrameGrabberControlsDC1394_Forwarder(yarp::os::Port& port) :
    m_port(port)
{
}


bool FrameGrabberControlsDC1394_Forwarder::setCommand(int code, double v)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(code);
    cmd.addFloat64(v);
    m_port.write(cmd, response);
    return true;
}


bool FrameGrabberControlsDC1394_Forwarder::setCommand(int code, double b, double r)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(code);
    cmd.addFloat64(b);
    cmd.addFloat64(r);
    m_port.write(cmd, response);
    return true;
}


double FrameGrabberControlsDC1394_Forwarder::getCommand(int code) const
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(code);
    m_port.write(cmd, response);
    // response should be [cmd] [name] value
    return response.get(2).asFloat64();
}


bool FrameGrabberControlsDC1394_Forwarder::getCommand(int code, double& b, double& r) const
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(code);
    m_port.write(cmd, response);
    // response should be [cmd] [name] value
    b = response.get(2).asFloat64();
    r = response.get(3).asFloat64();
    return true;
}


unsigned int FrameGrabberControlsDC1394_Forwarder::getVideoModeMaskDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETMSK);
    m_port.write(cmd, response);
    return static_cast<unsigned int>(response.get(0).asInt32());
}


unsigned int FrameGrabberControlsDC1394_Forwarder::getVideoModeDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETVMD);
    m_port.write(cmd, response);
    return static_cast<unsigned int>(response.get(0).asInt32());
}


bool FrameGrabberControlsDC1394_Forwarder::setVideoModeDC1394(int video_mode)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETVMD);
    cmd.addInt32(video_mode);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


unsigned int FrameGrabberControlsDC1394_Forwarder::getFPSMaskDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETFPM);
    m_port.write(cmd, response);
    return static_cast<unsigned int>(response.get(0).asInt32());
}


unsigned int FrameGrabberControlsDC1394_Forwarder::getFPSDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETFPS);
    m_port.write(cmd, response);
    return static_cast<unsigned int>(response.get(0).asInt32());
}


bool FrameGrabberControlsDC1394_Forwarder::setFPSDC1394(int fps)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETFPS);
    cmd.addInt32(fps);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


unsigned int FrameGrabberControlsDC1394_Forwarder::getISOSpeedDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETISO);
    m_port.write(cmd, response);
    return static_cast<unsigned int>(response.get(0).asInt32());
}


bool FrameGrabberControlsDC1394_Forwarder::setISOSpeedDC1394(int speed)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETISO);
    cmd.addInt32(speed);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


unsigned int FrameGrabberControlsDC1394_Forwarder::getColorCodingMaskDC1394(unsigned int video_mode)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETCCM);
    cmd.addInt32(video_mode);
    m_port.write(cmd, response);
    return static_cast<unsigned int>(response.get(0).asInt32());
}


unsigned int FrameGrabberControlsDC1394_Forwarder::getColorCodingDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETCOD);
    m_port.write(cmd, response);
    return static_cast<unsigned int>(response.get(0).asInt32());
}


bool FrameGrabberControlsDC1394_Forwarder::setColorCodingDC1394(int coding)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETCOD);
    cmd.addInt32(coding);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::getFormat7MaxWindowDC1394(unsigned int& xdim,
                                                unsigned int& ydim,
                                                unsigned int& xstep,
                                                unsigned int& ystep,
                                                unsigned int& xoffstep,
                                                unsigned int& yoffstep)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETF7M);
    m_port.write(cmd, response);

    xdim = response.get(0).asInt32();
    ydim = response.get(1).asInt32();
    xstep = response.get(2).asInt32();
    ystep = response.get(3).asInt32();
    xoffstep = response.get(4).asInt32();
    yoffstep = response.get(5).asInt32();
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::getFormat7WindowDC1394(unsigned int& xdim, unsigned int& ydim, int& x0, int& y0)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETWF7);
    m_port.write(cmd, response);
    xdim = response.get(0).asInt32();
    ydim = response.get(1).asInt32();
    x0 = response.get(2).asInt32();
    y0 = response.get(3).asInt32();
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setFormat7WindowDC1394(unsigned int xdim, unsigned int ydim, int x0, int y0)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETWF7);
    cmd.addInt32(xdim);
    cmd.addInt32(ydim);
    cmd.addInt32(x0);
    cmd.addInt32(y0);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setOperationModeDC1394(bool b1394b)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETOPM);
    cmd.addInt32(int(b1394b));
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::getOperationModeDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETOPM);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setTransmissionDC1394(bool bTxON)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETTXM);
    cmd.addInt32(int(bTxON));
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::getTransmissionDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETTXM);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setBroadcastDC1394(bool onoff)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETBCS);
    cmd.addInt32(static_cast<int>(onoff));
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setDefaultsDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETDEF);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setResetDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETRST);
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setPowerDC1394(bool onoff)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETPWR);
    cmd.addInt32(static_cast<int>(onoff));
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setCaptureDC1394(bool bON)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETCAP);
    cmd.addInt32(int(bON));
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


bool FrameGrabberControlsDC1394_Forwarder::setBytesPerPacketDC1394(unsigned int bpp)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRSETBPP);
    cmd.addInt32(int(bpp));
    m_port.write(cmd, response);
    return response.get(0).asBool();
}


unsigned int FrameGrabberControlsDC1394_Forwarder::getBytesPerPacketDC1394()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL_DC1394);
    cmd.addVocab32(VOCAB_DRGETBPP);
    m_port.write(cmd, response);
    return static_cast<unsigned int>(response.get(0).asInt32());
}
