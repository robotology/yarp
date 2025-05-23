/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameWriter_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/PolyDriver.h>

namespace {
YARP_LOG_COMPONENT(FRAMEWRITER_NWS_YARP, "yarp.device.frameWriter_nws_yarp")
} // namespace

using namespace yarp::sig;

static constexpr double DEFAULT_THREAD_PERIOD = 0.033; // seconds

FrameWriter_nws_yarp::FrameWriter_nws_yarp() :
        PeriodicThread(DEFAULT_THREAD_PERIOD)
{
}


FrameWriter_nws_yarp::~FrameWriter_nws_yarp()
{
    close();
}


bool FrameWriter_nws_yarp::close()
{
    detach();

    m_portProc.close();
    rpcPort.close();

    return true;
}


bool FrameWriter_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) { return false; }

    PeriodicThread::setPeriod(0.010);

    // Check "name" option and open ports
    std::string pImg_Name = m_name;
    std::string rpcPort_Name = pImg_Name + "/rpc";

    if (!rpcPort.open(rpcPort_Name)) {
        yCError(FRAMEWRITER_NWS_YARP) << "Unable to open rpc Port" << rpcPort_Name.c_str();
        return false;
    }

    if (!m_portProc.open(pImg_Name)) {
        yCError(FRAMEWRITER_NWS_YARP) << "Unable to open image input Port" << pImg_Name.c_str();
        return false;
    }
    m_portProc.useCallback();

    yCInfo(FRAMEWRITER_NWS_YARP) << "Running, waiting for attach...";

    return true;
}

bool FrameWriter_nws_yarp::attach(yarp::dev::PolyDriver* poly)
{
    if (!poly->isValid()) {
        yCError(FRAMEWRITER_NWS_YARP) << "Device " << poly << " to attach to is not valid ... cannot proceed";
        return false;
    }

    poly->view(iFrameWriterImage);

    return PeriodicThread::start();
}


bool FrameWriter_nws_yarp::detach()
{
    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    iFrameWriterImage = nullptr;

    return true;
}

bool FrameWriter_nws_yarp::threadInit()
{
    return true;
}

// Processes the received images
void FrameWriter_nws_yarp::run()
{
    std::vector<yarp::sig::FlexImage> vec;
    m_portProc.getLast(vec);

    for (auto it = vec.begin(); it != vec.end(); it++)
    {
        if (it->getPixelCode() == VOCAB_PIXEL_RGB) {
            ImageOf<yarp::sig::PixelRgb> img = (ImageOf<PixelRgb>&)(*it);
            iFrameWriterImage->putImage(img);
        }
        else
        {
            yCError(FRAMEWRITER_NWS_YARP) << "Unsupported image type received";
        }
    }
}
