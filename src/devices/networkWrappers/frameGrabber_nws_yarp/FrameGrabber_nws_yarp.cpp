/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabber_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/DummyConnector.h>
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
    std::lock_guard lock (m_mutex);

    if (!poly->isValid())
    {
        yCError(FRAMEGRABBER_NWS_YARP) << "Device " << poly << " to attach to is not valid ... cannot proceed";
        return false;
    }

    //Device Interfaces
    poly->view(m_iRgbVisualParams);
    poly->view(m_iFrameGrabberControls);
    poly->view(m_iFrameGrabberControlsDC1394);
    poly->view(m_iFrameGrabberImage); //optional: may be not implemented by device
    poly->view(m_iFrameGrabberImageRaw); // optional: may be not implemented by device
    poly->view(m_iPreciselyTimed);  //optional: may be not implemented by device

    switch (m_cap) {
    case COLOR: {
        if (m_iFrameGrabberImage == nullptr) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Capability \"COLOR\" required not supported";
            return false;
        }
    } break;
    case RAW: {
        if (m_iFrameGrabberImageRaw == nullptr) {
            yCError(FRAMEGRABBER_NWS_YARP) << "Capability \"RAW\" required not supported";
            return false;
        }
    }
    }

    if (m_iRgbVisualParams == nullptr) {
        yCWarning(FRAMEGRABBER_NWS_YARP) << "Targets has not IVisualParamInterface, some features cannot be available";
    }

    m_RPC_FrameGrabber = std::make_unique<FrameGrabberMsgsImpl>(m_iRgbVisualParams, m_iFrameGrabberControls, m_iFrameGrabberControlsDC1394, m_iFrameGrabberImage, m_iFrameGrabberImageRaw);

    return PeriodicThread::start();
}


bool FrameGrabber_nws_yarp::detach()
{
    std::lock_guard lock (m_mutex);

    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    m_iFrameGrabberImage = nullptr;
    m_iFrameGrabberImageRaw = nullptr;
    m_iPreciselyTimed = nullptr;
    m_iRgbVisualParams = nullptr;
    m_iFrameGrabberControls = nullptr;
    m_iFrameGrabberControlsDC1394 = nullptr;

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

    if (m_cap == COLOR)
    {
        if (m_iFrameGrabberImage != nullptr) {
            if (m_iFrameGrabberImage->getImage(*img))
                {flex_i.swap(*img);}
            else
                {yCError(FRAMEGRABBER_NWS_YARP) << "Image not captured (getImage failed). Check hardware configuration.";}
        } else {
            yCError(FRAMEGRABBER_NWS_YARP) << "Image not captured (invalid interface). Check hw/sw configuration.";
        }
    }

    if (m_cap == RAW)
    {
        if (m_iFrameGrabberImageRaw != nullptr) {
            if (m_iFrameGrabberImageRaw->getImage(*img_Raw))
                {flex_i.swap(*img_Raw);}
            else
                {yCError(FRAMEGRABBER_NWS_YARP) << "Image not captured (getImage failed). Check hardware configuration.";}
        } else {
            yCError(FRAMEGRABBER_NWS_YARP) << "Image not captured (invalid interface). Check hw/sw configuration.";
        }
    }

    if (m_iPreciselyTimed)
    {
        m_stamp = m_iPreciselyTimed->getLastInputStamp();
    } else {
        m_stamp.update(yarp::os::Time::now());
    }
    pImg.setEnvelope(m_stamp);

    pImg.write();
}

// Respond to the RPC calls
bool FrameGrabber_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}
    if (!m_RPC_FrameGrabber) { return false;}

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_RPC_FrameGrabber->read(connection))
    {
        return true;
    }

    yCError(FRAMEGRABBER_NWS_YARP) << "read() Command failed";
    return false;
}
