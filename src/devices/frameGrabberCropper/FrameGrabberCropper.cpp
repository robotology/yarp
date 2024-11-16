/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabberCropper.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

namespace {
YARP_LOG_COMPONENT(FRAMEGRABBERCROPPER, "yarp.device.frameGrabberCropper")
}

FrameGrabberCropper::~FrameGrabberCropper()
{
    close();
}


bool FrameGrabberCropper::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) { return false; }

    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::vertices = {{m_x1, m_y1}, {m_x2, m_y2}};
    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::vertices = {{m_x1, m_y1}, {m_x2, m_y2}};

    return true;
}


bool FrameGrabberCropper::close()
{
    return true;
}

bool FrameGrabberCropper::attach(yarp::dev::PolyDriver* poly)
{
    if (!poly->isValid()) {
        yCError(FRAMEGRABBERCROPPER) << "The device is not valid";
        return false;
    }

    poly->view(FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::iFrameGrabberOf);
    poly->view(FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::iFrameGrabberOf);
    poly->view(iFrameGrabberControls);
    poly->view(iFrameGrabberControlsDC1394);
    poly->view(iRgbVisualParams);
    poly->view(iPreciselyTimed);

    if (!FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::iFrameGrabberOf && !FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::iFrameGrabberOf) {
        yCError(FRAMEGRABBERCROPPER) << "The device does not implement the required interfaces";
        return false;
    }

    return true;
}

bool FrameGrabberCropper::detach()
{
    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::iFrameGrabberOf = nullptr;
    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::iFrameGrabberOf = nullptr;
    iFrameGrabberControls = nullptr;
    iFrameGrabberControlsDC1394 = nullptr;
    iRgbVisualParams = nullptr;
    iPreciselyTimed = nullptr;
    return true;
}

bool FrameGrabberCropper::attachAll(const yarp::dev::PolyDriverList& device2attach)
{
    if (device2attach.size() != 1) {
        yCError(FRAMEGRABBERCROPPER, "Expected only one device to be attached");
        return false;
    }

    return attach(device2attach[0]->poly);
}

bool FrameGrabberCropper::detachAll()
{
    return detach();
}
