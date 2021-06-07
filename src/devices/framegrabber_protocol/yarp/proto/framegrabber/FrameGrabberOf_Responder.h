/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERIMAGE_RESPONDER_H
#define YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERIMAGE_RESPONDER_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>

#include "CameraVocabs.h"

namespace yarp {
namespace proto {
namespace framegrabber {

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab = VOCAB_FRAMEGRABBER_IMAGE,
          yarp::conf::vocab32_t ImgVocab = VOCAB_RGB_IMAGE>
class FrameGrabberOf_Responder :
        public yarp::dev::DeviceResponder
{
private:
    yarp::dev::IFrameGrabberOf<ImageType>* iFrameGrabberOf {nullptr};

public:
    FrameGrabberOf_Responder() = default;
    ~FrameGrabberOf_Responder() override = default;

    bool configure(yarp::dev::IFrameGrabberOf<ImageType>* interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

} // namespace framegrabber
} // namespace proto
} // namespace yarp

#include <yarp/proto/framegrabber/FrameGrabberOf_Responder-inl.h>

#endif // YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBERIMAGE_RESPONDER_H
