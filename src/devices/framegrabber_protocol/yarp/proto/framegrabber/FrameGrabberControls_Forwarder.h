/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLS_FORWARDER_H
#define YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLS_FORWARDER_H

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/Port.h>

namespace yarp {
namespace proto {
namespace framegrabber {

/**
 * This classes implement a sender / parser for IFrameGrabberControls
 * interface messages
 */
class FrameGrabberControls_Forwarder :
        public yarp::dev::IFrameGrabberControls
{
private:
    yarp::os::Port& m_port;

public:
    FrameGrabberControls_Forwarder(yarp::os::Port& port);
    ~FrameGrabberControls_Forwarder() override = default;

    bool getCameraDescription(CameraDescriptor* camera) override;
    bool hasFeature(int feature, bool* hasFeature) override;
    bool setFeature(int feature, double value) override;
    bool getFeature(int feature, double* value) override;
    bool setFeature(int feature, double value1, double value2) override;
    bool getFeature(int feature, double* value1, double* value2) override;
    bool hasOnOff(int feature, bool* HasOnOff) override;
    bool setActive(int feature, bool onoff) override;
    bool getActive(int feature, bool* isActive) override;
    bool hasAuto(int feature, bool* hasAuto) override;
    bool hasManual(int feature, bool* hasManual) override;
    bool hasOnePush(int feature, bool* hasOnePush) override;
    bool setMode(int feature, FeatureMode mode) override;
    bool getMode(int feature, FeatureMode* mode) override;
    bool setOnePush(int feature) override;
};

} // namespace framegrabber
} // namespace proto
} // namespace yarp

#endif // YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLS_FORWARDER_H
