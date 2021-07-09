/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabberControlsDC1394_Responder.h"
#include "CameraVocabs.h"

#include <yarp/os/LogStream.h>

using yarp::proto::framegrabber::FrameGrabberControlsDC1394_Responder;

bool FrameGrabberControlsDC1394_Responder::configure(yarp::dev::IFrameGrabberControlsDC1394* interface)
{
    fgCtrl_DC1394 = interface;
    return true;
}

bool FrameGrabberControlsDC1394_Responder::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    if (!fgCtrl_DC1394) {
        yWarning() << "FrameGrabberControlsDC1394_Responder: firewire interface not implemented in subdevice, some features could not be available";
        return yarp::dev::DeviceResponder::respond(cmd, response);
    }

    int code = cmd.get(1).asVocab32();
    switch (code) {
    case VOCAB_DRGETMSK:
        response.addInt32(int(fgCtrl_DC1394->getVideoModeMaskDC1394()));
        return true;

    case VOCAB_DRGETVMD:
        response.addInt32(int(fgCtrl_DC1394->getVideoModeDC1394()));
        return true;

    case VOCAB_DRSETVMD:
        response.addInt32(int(fgCtrl_DC1394->setVideoModeDC1394(cmd.get(1).asInt32())));
        return true;

    case VOCAB_DRGETFPM:
        response.addInt32(int(fgCtrl_DC1394->getFPSMaskDC1394()));
        return true;

    case VOCAB_DRGETFPS:
        response.addInt32(int(fgCtrl_DC1394->getFPSDC1394()));
        return true;

    case VOCAB_DRSETFPS:
        response.addInt32(int(fgCtrl_DC1394->setFPSDC1394(cmd.get(1).asInt32())));
        return true;

    case VOCAB_DRGETISO:
        response.addInt32(int(fgCtrl_DC1394->getISOSpeedDC1394()));
        return true;

    case VOCAB_DRSETISO:
        response.addInt32(int(fgCtrl_DC1394->setISOSpeedDC1394(cmd.get(1).asInt32())));
        return true;

    case VOCAB_DRGETCCM:
        response.addInt32(int(fgCtrl_DC1394->getColorCodingMaskDC1394(cmd.get(1).asInt32())));
        return true;

    case VOCAB_DRGETCOD:
        response.addInt32(int(fgCtrl_DC1394->getColorCodingDC1394()));
        return true;

    case VOCAB_DRSETCOD:
        response.addInt32(int(fgCtrl_DC1394->setColorCodingDC1394(cmd.get(1).asInt32())));
        return true;

    case VOCAB_DRGETF7M:
    {
        unsigned int xstep;
        unsigned int ystep;
        unsigned int xdim;
        unsigned int ydim;
        unsigned int xoffstep;
        unsigned int yoffstep;
        fgCtrl_DC1394->getFormat7MaxWindowDC1394(xdim, ydim, xstep, ystep, xoffstep, yoffstep);
        response.addInt32(xdim);
        response.addInt32(ydim);
        response.addInt32(xstep);
        response.addInt32(ystep);
        response.addInt32(xoffstep);
        response.addInt32(yoffstep);
    }
        return true;

    case VOCAB_DRGETWF7:
    {
        unsigned int xdim;
        unsigned int ydim;
        int x0;
        int y0;
        fgCtrl_DC1394->getFormat7WindowDC1394(xdim, ydim, x0, y0);
        response.addInt32(xdim);
        response.addInt32(ydim);
        response.addInt32(x0);
        response.addInt32(y0);
    }
        return true;

    case VOCAB_DRSETWF7:
        response.addInt32(int(fgCtrl_DC1394->setFormat7WindowDC1394(cmd.get(1).asInt32(), cmd.get(2).asInt32(), cmd.get(3).asInt32(), cmd.get(4).asInt32())));
        return true;

    case VOCAB_DRSETOPM:
        response.addInt32(int(fgCtrl_DC1394->setOperationModeDC1394(cmd.get(1).asInt32() != 0)));
        return true;

    case VOCAB_DRGETOPM:
        response.addInt32(fgCtrl_DC1394->getOperationModeDC1394());
        return true;

    case VOCAB_DRSETTXM:
        response.addInt32(int(fgCtrl_DC1394->setTransmissionDC1394(cmd.get(1).asInt32() != 0)));
        return true;

    case VOCAB_DRGETTXM:
        response.addInt32(fgCtrl_DC1394->getTransmissionDC1394());
        return true;

    case VOCAB_DRSETBCS:
        response.addInt32(int(fgCtrl_DC1394->setBroadcastDC1394(cmd.get(1).asInt32() != 0)));
        return true;

    case VOCAB_DRSETDEF:
        response.addInt32(int(fgCtrl_DC1394->setDefaultsDC1394()));
        return true;

    case VOCAB_DRSETRST:
        response.addInt32(int(fgCtrl_DC1394->setResetDC1394()));
        return true;

    case VOCAB_DRSETPWR:
        response.addInt32(int(fgCtrl_DC1394->setPowerDC1394(cmd.get(1).asInt32() != 0)));
        return true;

    case VOCAB_DRSETCAP:
        response.addInt32(int(fgCtrl_DC1394->setCaptureDC1394(cmd.get(1).asInt32() != 0)));
        return true;

    case VOCAB_DRSETBPP:
        response.addInt32(int(fgCtrl_DC1394->setBytesPerPacketDC1394(cmd.get(1).asInt32())));
        return true;

    case VOCAB_DRGETBPP:
        response.addInt32(fgCtrl_DC1394->getBytesPerPacketDC1394());
        return true;
    }

    return true;
}
