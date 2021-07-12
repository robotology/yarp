/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_FORWARDER_H
#define YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_FORWARDER_H

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/os/Port.h>

#include "CameraVocabs.h"

namespace yarp {
namespace proto {
namespace framegrabber {

/**
 * This classes implement a sender / parser for IFrameGrabberOf
 * interface messages
 */
template <typename ImageType,
          yarp::conf::vocab32_t IfVocab = VOCAB_FRAMEGRABBER_IMAGE,
          yarp::conf::vocab32_t ImgVocab = VOCAB_RGB_IMAGE>
class FrameGrabberOf_Forwarder :
        public yarp::dev::IFrameGrabberOf<ImageType>
{
private:
    yarp::os::Port& m_port;

public:
    FrameGrabberOf_Forwarder(yarp::os::Port& port);
    ~FrameGrabberOf_Forwarder() override = default;

    int height() const override;
    int width() const override;
    bool getImage(ImageType& image) override;
    bool getImageCrop(cropType_id_t cropType,
                      yarp::sig::VectorOf<std::pair<int, int>> vertices,
                      ImageType& image) override;
};

} // namespace framegrabber
} // namespace proto
} // namespace yarp


#include <yarp/proto/framegrabber/FrameGrabberOf_Forwarder-inl.h>

#endif // YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_FORWARDER_H
