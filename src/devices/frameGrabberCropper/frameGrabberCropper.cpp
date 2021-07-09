/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "frameGrabberCropper.h"

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
    int x1;
    if (config.check("x1", "x1") && config.find("x1").isInt32()) {
        x1 = config.find("x1").asInt32();
    } else {
        yCError(FRAMEGRABBERCROPPER) << "x1 parameter not found";
        return false;
    }

    int y1;
    if (config.check("y1", "y1") && config.find("y1").isInt32()) {
        y1 = config.find("y1").asInt32();
    } else {
        yCError(FRAMEGRABBERCROPPER) << "y1 parameter not found";
        return false;
    }

    int x2;
    if (config.check("x2", "x2") && config.find("x2").isInt32()) {
        x2 = config.find("x2").asInt32();
    } else {
        yCError(FRAMEGRABBERCROPPER) << "x2 parameter not found";
        return false;
    }

    int y2;
    if (config.check("y2", "y2") && config.find("y2").isInt32()) {
        y2 = config.find("y2").asInt32();
    } else {
        yCError(FRAMEGRABBERCROPPER) << "y2 parameter not found";
        return false;
    }

    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::vertices = {{x1, y1}, {x2, y2}};
    FrameGrabberCropperOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::vertices = {{x1, y1}, {x2, y2}};

    if (config.check("forwardRgbVisualParams", "Forward the the IRgbVisualParams calls to the attached device")) {
        forwardRgbVisualParams = true;
    }

    if (config.check("subdevice")) {
        yarp::os::Property p;
        subdevice = new yarp::dev::PolyDriver;
        p.fromString(config.toString());
        p.setMonitor(config.getMonitor(), "subdevice"); // pass on any monitoring
        p.unput("device");
        p.put("device", config.find("subdevice").asString()); // subdevice was already checked before

        // if errors occurred during open, quit here.
        if (!subdevice->open(p) || !(subdevice->isValid())) {
            yCError(FRAMEGRABBERCROPPER, "Could not open subdevice");
            return false;
        }

        if (!attach(subdevice)) {
            yCError(FRAMEGRABBERCROPPER, "Could not attach subdevice");
            subdevice->close();
            return false;
        }
        subdeviceOwned = true;
    }

    return true;
}


bool FrameGrabberCropper::close()
{
    if (subdeviceOwned) {
        subdevice->close();
        delete subdevice;
        subdevice = nullptr;
    }
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
