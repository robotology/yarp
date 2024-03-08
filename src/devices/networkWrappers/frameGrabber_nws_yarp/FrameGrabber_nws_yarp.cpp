/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabber_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/PolyDriver.h>

#include <yarp/proto/framegrabber/CameraVocabs.h>

namespace {
YARP_LOG_COMPONENT(FRAMEGRABBER_NWS_YARP, "yarp.device.frameGrabber_nws_yarp")
} // namespace


static constexpr double DEFAULT_THREAD_PERIOD = 0.033; // seconds

FrameGrabber_nws_yarp::FrameGrabber_nws_yarp() :
        PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}


FrameGrabber_nws_yarp::~FrameGrabber_nws_yarp()
{
    close();
}


bool FrameGrabber_nws_yarp::close()
{
    detach();

    pImg.interrupt();
    pImg.close();

    rpcPort.interrupt();
    rpcPort.close();

    delete img;
    img = nullptr;

    delete img_Raw;
    img_Raw = nullptr;

    return true;
}


bool FrameGrabber_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) { return false; }

    PeriodicThread::setPeriod(m_period);

    // Check "capabilities" option
    if      (m_capabilities == "COLOR")    { m_cap = COLOR; }
    else if (m_capabilities == "RAW")      { m_cap = RAW; }
    else    { yCError(FRAMEGRABBER_NWS_YARP) << "'capabilities' parameter unsupported value"; return false;}

    // Check "name" option and open ports
    std::string pImg_Name = m_name;
    std::string rpcPort_Name = pImg_Name + "/rpc";

    if (!rpcPort.open(rpcPort_Name)) {
        yCError(FRAMEGRABBER_NWS_YARP) << "Unable to open rpc Port" << rpcPort_Name.c_str();
        return false;
    }
    rpcPort.setReader(*this);

    pImg.promiseType(yarp::os::Type::byName("yarp/image"));
    pImg.setWriteOnly();
    pImg.setStrict(m_no_drop);
    if (!pImg.open(pImg_Name)) {
        yCError(FRAMEGRABBER_NWS_YARP) << "Unable to open image streaming Port" << pImg_Name.c_str();
        return false;
    }
    pImg.setReader(*this);

    yCInfo(FRAMEGRABBER_NWS_YARP) << "Running, waiting for attach...";

    return true;
}

bool FrameGrabber_nws_yarp::attach(yarp::dev::PolyDriver* poly)
{
    if (!poly->isValid()) {
        yCError(FRAMEGRABBER_NWS_YARP) << "Device " << poly << " to attach to is not valid ... cannot proceed";
        return false;
    }

    poly->view(iRgbVisualParams);
    poly->view(iFrameGrabberImage);
    poly->view(iFrameGrabberImageRaw);
    poly->view(iFrameGrabberControls);
    poly->view(iFrameGrabberControlsDC1394);
    poly->view(iPreciselyTimed);

    switch (m_cap) {
    case COLOR: {
        if (iFrameGrabberImage == nullptr) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Capability \"COLOR\" required not supported";
            return false;
        }
    } break;
    case RAW: {
        if (iFrameGrabberImageRaw == nullptr) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Capability \"RAW\" required not supported";
            return false;
        }
    }
    }

    if (iRgbVisualParams == nullptr) {
        yCWarning(FRAMEGRABBER_NWS_YARP) << "Targets has not IVisualParamInterface, some features cannot be available";
    }

    // Configuring parsers
    if (iFrameGrabberImage != nullptr) {
        if (!(frameGrabberImage_Responder.configure(iFrameGrabberImage))) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Error configuring interfaces for parsers";
            return false;
        }
    }

    if (iFrameGrabberImageRaw != nullptr) {
        if (!(frameGrabberImageRaw_Responder.configure(iFrameGrabberImageRaw))) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Error configuring interfaces for parsers";
            return false;
        }
    }

    if (iRgbVisualParams != nullptr) {
        if (!(rgbVisualParams_Responder.configure(iRgbVisualParams))) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Error configuring interfaces for parsers";
            return false;
        }
    }
    if (iFrameGrabberControls != nullptr) {
        if (!(frameGrabberControls_Responder.configure(iFrameGrabberControls))) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Error configuring interfaces for parsers";
            return false;
        }
    }

    if (iFrameGrabberControlsDC1394 != nullptr) {
        if (!(frameGrabberControlsDC1394_Responder.configure(iFrameGrabberControlsDC1394))) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Error configuring interfaces for parsers";
            return false;
        }
    }

    return PeriodicThread::start();
}


bool FrameGrabber_nws_yarp::detach()
{
    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    iRgbVisualParams = nullptr;
    iFrameGrabberImage = nullptr;
    iFrameGrabberImageRaw = nullptr;
    iFrameGrabberControls = nullptr;
    iFrameGrabberControlsDC1394 = nullptr;
    iPreciselyTimed = nullptr;

    return true;
}

bool FrameGrabber_nws_yarp::threadInit()
{
    if (m_cap == COLOR) {
        img = new yarp::sig::ImageOf<yarp::sig::PixelRgb>;
    } else {
        img_Raw = new yarp::sig::ImageOf<yarp::sig::PixelMono>;
    }

    return true;
}

// Publish the images on the buffered port
void FrameGrabber_nws_yarp::run()
{
    if (pImg.getOutputCount() == 0) {
        // If no ports are connected, do not call getImage on the interface.
        return;
    }

    yarp::sig::FlexImage& flex_i = pImg.prepare();

    if (m_cap == COLOR) {
        if (iFrameGrabberImage != nullptr) {
            if (iFrameGrabberImage->getImage(*img))
                {flex_i.swap(*img);}
            else
                {yCError(FRAMEGRABBER_NWS_YARP) << "Image not captured (getImage failed). Check hardware configuration.";}
        } else {
            yCError(FRAMEGRABBER_NWS_YARP) << "Image not captured (invalid interface). Check hw/sw configuration.";
        }
    }

    if (m_cap == RAW) {
        if (iFrameGrabberImageRaw != nullptr) {
            if (iFrameGrabberImageRaw->getImage(*img_Raw))
                {flex_i.swap(*img_Raw);}
            else
                {yCError(FRAMEGRABBER_NWS_YARP) << "Image not captured (getImage failed). Check hardware configuration.";}
        } else {
            yCError(FRAMEGRABBER_NWS_YARP) << "Image not captured (invalid interface). Check hw/sw configuration.";
        }
    }

    if (iPreciselyTimed) {
        m_stamp = iPreciselyTimed->getLastInputStamp();
    } else {
        m_stamp.update(yarp::os::Time::now());
    }
    pImg.setEnvelope(m_stamp);

    pImg.write();
}

// Respond to the RPC calls
bool FrameGrabber_nws_yarp::respond(const yarp::os::Bottle& command,
                                    yarp::os::Bottle& reply)
{
    yarp::conf::vocab32_t code = command.get(0).asVocab32();
    switch (code) {
    case VOCAB_FRAMEGRABBER_IMAGE:
        return frameGrabberImage_Responder.respond(command, reply);
    case VOCAB_FRAMEGRABBER_IMAGERAW:
        return frameGrabberImageRaw_Responder.respond(command, reply);
    case VOCAB_FRAMEGRABBER_CONTROL:
        return frameGrabberControls_Responder.respond(command, reply);
    case VOCAB_RGB_VISUAL_PARAMS:
        return rgbVisualParams_Responder.respond(command, reply);
    case VOCAB_FRAMEGRABBER_CONTROL_DC1394:
        return frameGrabberControlsDC1394_Responder.respond(command, reply);
    default:
        yCError(FRAMEGRABBER_NWS_YARP) << "Command not recognized" << command.toString();
        return false;
    }

    return false;
}
