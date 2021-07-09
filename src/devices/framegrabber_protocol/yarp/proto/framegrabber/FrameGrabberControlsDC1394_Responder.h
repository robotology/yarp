/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLSDC1394_RESPONDER_H
#define YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLSDC1394_RESPONDER_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>

namespace yarp {
namespace proto {
namespace framegrabber {

class FrameGrabberControlsDC1394_Responder :
        public yarp::dev::DeviceResponder
{
private:
    yarp::dev::IFrameGrabberControlsDC1394  *fgCtrl_DC1394{nullptr};

public:
    FrameGrabberControlsDC1394_Responder() = default;
    ~FrameGrabberControlsDC1394_Responder() override = default;
    bool configure(yarp::dev::IFrameGrabberControlsDC1394 *interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

} // namespace framegrabber
} // namespace proto
} // namespace yarp

#endif // YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERCONTROLSDC1394_RESPONDER_H
