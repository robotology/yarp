/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_DEPTHVISUALPARAMS_RESPONDER_H
#define YARP_FRAMEGRABBER_PROTOCOL_DEPTHVISUALPARAMS_RESPONDER_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IDepthVisualParams.h>

namespace yarp {
namespace proto {
namespace framegrabber {

class DepthVisualParams_Responder :
        public yarp::dev::DeviceResponder
{
private:
    yarp::dev::IDepthVisualParams* iDepthVisual;

public:
    DepthVisualParams_Responder();
    ~DepthVisualParams_Responder() override = default;

    bool configure(yarp::dev::IDepthVisualParams* interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

} // namespace framegrabber
} // namespace proto
} // namespace yarp

#endif // YARP_FRAMEGRABBER_PROTOCOL_DEPTHVISUALPARAMS_RESPONDER_H
