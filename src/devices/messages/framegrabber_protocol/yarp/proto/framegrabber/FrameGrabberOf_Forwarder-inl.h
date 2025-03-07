/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_FORWARDER_INL_H
#define YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_FORWARDER_INL_H

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace {
YARP_LOG_COMPONENT(FRAMEGRABBEROF_FORWARDER, "yarp.proto.framegrabber.FrameGrabberOf_Forwarder")
}

namespace yarp::proto::framegrabber {

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::FrameGrabberOf_Forwarder(yarp::os::Port& port) :
        m_port(port)
{
};

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
int FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::height() const
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(IfVocab);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_HEIGHT);

    if (m_port.getOutputCount() == 0) {
        yCWarningThrottle(FRAMEGRABBEROF_FORWARDER, 5.0, "Remote port is not connected");
        return false;
    }

    if (!m_port.write(cmd, response)) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Could not execute RPC command");
        return 0;
    }

    // reply should be [fgi|fgir] [h] [is] <height>
    if (!response.get(0).isVocab32() || (response.get(0).asVocab32() != IfVocab) ||
        !response.get(1).isVocab32() || (response.get(1).asVocab32() != VOCAB_HEIGHT) ||
        !response.get(2).isVocab32() || (response.get(2).asVocab32() != VOCAB_IS) ||
        !response.get(3).isInt32()) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Invalid reply received");
        return 0;
    }

    return response.get(3).asInt32();
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
int FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::width() const
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(IfVocab);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_WIDTH);

    if (m_port.getOutputCount() == 0) {
        yCWarningThrottle(FRAMEGRABBEROF_FORWARDER, 5.0, "Remote port is not connected");
        return false;
    }

    if (!m_port.write(cmd, response)) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Could not execute RPC command");
        return 0;
    }

    // reply should be [fgi|fgir] [w] [is] <width>
    if (!response.get(0).isVocab32() || (response.get(0).asVocab32() != IfVocab) ||
        !response.get(1).isVocab32() || (response.get(1).asVocab32() != VOCAB_WIDTH) ||
        !response.get(2).isVocab32() || (response.get(2).asVocab32() != VOCAB_IS) ||
        !response.get(3).isInt32()) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Invalid reply received");
        return 0;
    }

    return response.get(3).asInt32();
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
yarp::dev::ReturnValue FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::getImage(ImageType& image)
{
    image.zero();

    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(IfVocab);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(ImgVocab);

    if (m_port.getOutputCount() == 0) {
        yCWarningThrottle(FRAMEGRABBEROF_FORWARDER, 5.0, "Remote port is not connected");
        return yarp::dev::ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
    }

    if (!m_port.write(cmd, response)) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Could not execute RPC command");
        return yarp::dev::ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
    }

    // reply should be [fgi|fgir] [imgr|?] [is] (<image>)
    if (!response.get(0).isVocab32() || (response.get(0).asVocab32() != IfVocab) ||
        !response.get(1).isVocab32() || (response.get(1).asVocab32() != ImgVocab) ||
        !response.get(2).isVocab32() || (response.get(2).asVocab32() != VOCAB_IS) ||
        !response.get(3).isList()) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Invalid reply received");
        return yarp::dev::ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
    }

    auto& serializedImage = response.get(3);
    // FIXME Is there a way to avoid this extra copy?
    if (!yarp::os::Portable::copyPortable(serializedImage, image)) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Image is not serialized properly");
        return yarp::dev::ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
    }

    return yarp::dev::ReturnValue_ok;
}


template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
yarp::dev::ReturnValue FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::getImageCrop(cropType_id_t cropType,
                                                                          yarp::sig::VectorOf<std::pair<int, int>> vertices,
                                                                          ImageType& image)
{
    image.zero();

    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_IMAGE);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_CROP);
    cmd.addInt32(cropType);
    yarp::os::Bottle & list = cmd.addList();
    for (size_t i=0; i<vertices.size(); i++)
    {
        list.addInt32(vertices[i].first);
        list.addInt32(vertices[i].second);
    }

    if (m_port.getOutputCount() == 0) {
        yCWarningThrottle(FRAMEGRABBEROF_FORWARDER, 5.0, "Remote port is not connected");
        return yarp::dev::ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
    }

    if (!m_port.write(cmd, response)) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Could not execute RPC command");
        return yarp::dev::ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
    }

    // reply should be [fgi|fgir] [crop] [is] (<image>)
    if (!response.get(0).isVocab32() || (response.get(0).asVocab32() != IfVocab) ||
        !response.get(1).isVocab32() || (response.get(1).asVocab32() != VOCAB_CROP) ||
        !response.get(2).isVocab32() || (response.get(2).asVocab32() != VOCAB_IS) ||
        !response.get(3).isList()) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "Cropped image is not serialized properly");
        return yarp::dev::ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
    }

    auto& serializedImage = response.get(3);
    // FIXME Is there a way to avoid this extra copy?
    if (!yarp::os::Portable::copyPortable(serializedImage, image)) {
        yCWarning(FRAMEGRABBEROF_FORWARDER, "...");
        return yarp::dev::ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
    }

    return yarp::dev::ReturnValue_ok;
}


} // namespace yarp::proto::framegrabber

#endif // YARP_FRAMEGRABBER_PROTOCOL_FRAMEGRABBEROF_FORWARDER_INL_H
