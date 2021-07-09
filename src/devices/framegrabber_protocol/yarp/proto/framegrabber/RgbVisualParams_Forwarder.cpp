/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RgbVisualParams_Forwarder.h"
#include "CameraVocabs.h"

using yarp::proto::framegrabber::RgbVisualParams_Forwarder;

RgbVisualParams_Forwarder::RgbVisualParams_Forwarder(yarp::os::Port& port) :
        m_port(port)
{
};

int RgbVisualParams_Forwarder::getRgbHeight()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_HEIGHT);
    m_port.write(cmd, response);
    return response.get(3).asInt32();
}

int RgbVisualParams_Forwarder::getRgbWidth()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_WIDTH);
    m_port.write(cmd, response);
    return response.get(3).asInt32();
}
bool RgbVisualParams_Forwarder::getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig>& configurations)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_SUPPORTED_CONF);
    m_port.write(cmd, response);

    if ((response.get(0).asVocab32()) == VOCAB_FAILED) {
        configurations.clear();
        return false;
    }
    configurations.resize(response.get(3).asInt32());
    for (int i = 0; i < response.get(3).asInt32(); i++) {
        configurations[i].width = response.get(4 + i * 4).asInt32();
        configurations[i].height = response.get(4 + i * 4 + 1).asInt32();
        configurations[i].framerate = response.get(4 + i * 4 + 2).asFloat64();
        configurations[i].pixelCoding = static_cast<YarpVocabPixelTypesEnum>(response.get(4 + i * 4 + 3).asVocab32());
    }
    return true;
}
bool RgbVisualParams_Forwarder::getRgbResolution(int& width, int& height)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_RESOLUTION);
    m_port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if ((response.get(0).asVocab32()) == VOCAB_FAILED) {
        width = 0;
        height = 0;
        return false;
    }
    width = response.get(3).asInt32();
    height = response.get(4).asInt32();
    return true;
}

bool RgbVisualParams_Forwarder::setRgbResolution(int width, int height)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_RESOLUTION);
    cmd.addInt32(width);
    cmd.addInt32(height);
    m_port.write(cmd, response);
    return response.get(2).asBool();
}

bool RgbVisualParams_Forwarder::getRgbFOV(double& horizontalFov, double& verticalFov)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_FOV);
    m_port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if ((response.get(0).asVocab32()) == VOCAB_FAILED) {
        horizontalFov = 0;
        verticalFov = 0;
        return false;
    }
    horizontalFov = response.get(3).asFloat64();
    verticalFov = response.get(4).asFloat64();
    return true;
}

bool RgbVisualParams_Forwarder::setRgbFOV(double horizontalFov, double verticalFov)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_FOV);
    cmd.addFloat64(horizontalFov);
    cmd.addFloat64(verticalFov);
    m_port.write(cmd, response);
    return response.get(2).asBool();
}

bool RgbVisualParams_Forwarder::getRgbIntrinsicParam(yarp::os::Property& intrinsic)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_INTRINSIC_PARAM);
    m_port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if ((response.get(0).asVocab32()) == VOCAB_FAILED) {
        intrinsic.clear();
        return false;
    }
    bool ret;
    ret = yarp::os::Property::copyPortable(response.get(3), intrinsic);
    if (!response.get(4).isNull()) {
        yarp::os::Property& p = intrinsic.addGroup("right");
        ret &= yarp::os::Property::copyPortable(response.get(4), p);
        return ret;
    }
    return ret;
}

bool RgbVisualParams_Forwarder::getRgbMirroring(bool& mirror)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_MIRROR);
    m_port.write(cmd, response);
    if ((response.get(0).asVocab32()) == VOCAB_FAILED) {
        return false;
    }
    mirror = response.get(3).asBool();
    return true;
}

bool RgbVisualParams_Forwarder::setRgbMirroring(bool mirror)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_RGB_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_MIRROR);
    cmd.addInt32(mirror);
    m_port.write(cmd, response);
    return response.get(2).asBool();
}
