/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_RESPONDER_INL_H
#define YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_RESPONDER_INL_H

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/sig/ImageNetworkHeader.h>
#include <yarp/sig/ImageUtils.h>

namespace {
YARP_LOG_COMPONENT(FRAMEGRABBEROF_RESPONDER, "yarp.proto.framegrabber.FrameGrabberOf_Responder")
}

namespace yarp {
namespace proto {
namespace framegrabber {


template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
bool FrameGrabberOf_Responder<ImageType, IfVocab, ImgVocab>::configure(yarp::dev::IFrameGrabberOf<ImageType>* interface)
{
    if (!interface) {
        yCError(FRAMEGRABBEROF_RESPONDER) << "Invalid IFrameGrabberOf interface received";
        return false;
    }

    iFrameGrabberOf = interface;
    return true;
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
bool FrameGrabberOf_Responder<ImageType, IfVocab, ImgVocab>::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& reply)
{
    if (!iFrameGrabberOf) {
        reply.addVocab32(VOCAB_FAILED);
        // FIXME C++17
        if /* constexpr */ (std::is_same<ImageType, yarp::sig::ImageOf<yarp::sig::PixelRgb>>::value) {
            reply.addString("Selected camera device has no IFrameGrabberImage interface");
            yCError(FRAMEGRABBEROF_RESPONDER) << "Selected camera device has no IFrameGrabberImage interface";
        } else if /* constexpr */ (std::is_same<ImageType, yarp::sig::ImageOf<yarp::sig::PixelMono>>::value) {
            reply.addString("Selected camera device has no IFrameGrabberImageRaw interface");
            yCError(FRAMEGRABBEROF_RESPONDER) << "Selected camera device has no IFrameGrabberImageRaw interface";
        } else {
            reply.addString("Selected camera device has no IFrameGrabberOf<ImageType> interface");
            yCError(FRAMEGRABBEROF_RESPONDER) << "Selected camera device has no IFrameGrabberOf<ImageType> interface";
        }
        return false;
    }

    if (cmd.get(0).asVocab32() != IfVocab) {
        reply.addVocab32(VOCAB_FAILED);
        yCError(FRAMEGRABBEROF_RESPONDER) << "Received a command not belonging to this interface.";
        return false;
    }

    switch (cmd.get(1).asVocab32()) {
    case VOCAB_GET: {
        switch (cmd.get(2).asVocab32()) {
        case VOCAB_HEIGHT:
            reply.clear();
            reply.addVocab32(IfVocab);
            reply.addVocab32(VOCAB_HEIGHT);
            reply.addVocab32(VOCAB_IS);
            reply.addInt32(iFrameGrabberOf->height());
            break;
        case VOCAB_WIDTH:
            reply.clear();
            reply.addVocab32(IfVocab);
            reply.addVocab32(VOCAB_WIDTH);
            reply.addVocab32(VOCAB_IS);
            reply.addInt32(iFrameGrabberOf->width());
            break;
        case ImgVocab: {
            reply.clear();
            ImageType image;
            iFrameGrabberOf->getImage(image);

            if (image.width() == 0 || image.height() == 0 || image.getRawImage() == nullptr) {
                reply.addVocab32(VOCAB_FAILED);
                reply.addString("Could not retrieve image from device.");
                yCError(FRAMEGRABBEROF_RESPONDER) << "Could not retrieve image from device.";
                return false;
            }

            reply.addVocab32(IfVocab);
            reply.addVocab32(ImgVocab);
            reply.addVocab32(VOCAB_IS);
            auto& b = reply.addList();
            yarp::os::Portable::copyPortable(image, b);
        } break;
        case VOCAB_CROP: {
            reply.clear();
            // If the device driver support it, use the device implementation, because it may be more efficient.
            // If not, acquire the whole image and crop it here before sending it.

            if (cmd.size() != 5) {
                reply.addVocab32(VOCAB_FAILED);
                reply.addString("GetImageCrop failed: Invalid command received.");
                yCError(FRAMEGRABBEROF_RESPONDER) << "GetImageCrop failed: Invalid command received, got " << cmd.toString();
                return false;
            }

            yarp::os::Bottle* list = cmd.get(4).asList();
            if (!list) {
                reply.addVocab32(VOCAB_FAILED);
                reply.addString("GetImageCrop failed: Empty vertices list received.");
                yCError(FRAMEGRABBEROF_RESPONDER) << "GetImageCrop failed: Empty vertices list received.";
                return false;
            }

            size_t nPoints = list->size() / 2; //  divided by 2 because each pixel is identified by 2 numbers (u,v)

            yarp::sig::VectorOf<std::pair<int, int>> vertices;
            vertices.resize(nPoints);

            for (size_t i = 0; i < nPoints; i++) {
                vertices[i].first = list->get(i * 2).asInt32();
                vertices[i].second = list->get(i * 2 + 1).asInt32();
            }

            ImageType cropped;

            if (iFrameGrabberOf->getImageCrop(static_cast<cropType_id_t>(cmd.get(3).asVocab32()), vertices, cropped)) {
                // use the device output
            } else {
                // In case the device has not yet implemented this feature, do it here (less efficient)
                if (cmd.get(3).asVocab32() == YARP_CROP_RECT) {
                    if (nPoints != 2) {
                        reply.addVocab32(VOCAB_FAILED);
                        reply.addString("GetImageCrop failed: RECT mode requires 2 vertices.");
                        yCError(FRAMEGRABBEROF_RESPONDER) << "GetImageCrop failed: RECT mode requires 2 vertices, got " << nPoints;
                        return false;
                    }
                    ImageType full;
                    iFrameGrabberOf->getImage(full);

                    if (!yarp::sig::utils::cropRect(full, vertices[0], vertices[1], cropped)) {
                        reply.addVocab32(VOCAB_FAILED);
                        reply.addString("GetImageCrop failed: utils::cropRect error.");
                        yCError(FRAMEGRABBEROF_RESPONDER, "GetImageCrop failed: utils::cropRect error: (%d, %d) (%d, %d)",
                                vertices[0].first,
                                vertices[0].second,
                                vertices[1].first,
                                vertices[1].second);
                        return false;
                    }
                } else if (cmd.get(3).asVocab32() == YARP_CROP_LIST) {
                    reply.addVocab32(VOCAB_FAILED);
                    reply.addString("List type not yet implemented");
                    yCError(FRAMEGRABBEROF_RESPONDER) << "List type not yet implemented";
                    return false;
                } else {
                    reply.addVocab32(VOCAB_FAILED);
                    reply.addString("Crop type unknown");
                    yCError(FRAMEGRABBEROF_RESPONDER) << "Crop type unknown";
                    return false;
                }
            }

            reply.addVocab32(IfVocab);
            reply.addVocab32(VOCAB_CROP);
            reply.addVocab32(VOCAB_IS);
            auto& b = reply.addList();
            yarp::os::Portable::copyPortable(cropped, b);
            return true;
        } break;
        }
        break;

    case VOCAB_SET: // Nothing to do here yet
    default:
        reply.addVocab32(VOCAB_FAILED);
        reply.addString("Received an unknown command");
        yCError(FRAMEGRABBEROF_RESPONDER) << "Received an unknown command " << cmd.toString();
        break;
    }
    }
    return true;
}

} // namespace framegrabber
} // namespace proto
} // namespace yarp


#endif // YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_RESPONDER_INL_H
