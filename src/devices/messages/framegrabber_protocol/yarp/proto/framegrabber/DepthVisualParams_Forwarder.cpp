/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DepthVisualParams_Forwarder.h"
#include "CameraVocabs.h"

using yarp::proto::framegrabber::DepthVisualParams_Forwarder;

DepthVisualParams_Forwarder::DepthVisualParams_Forwarder(yarp::os::Port& port) :
        m_port(port)
{
};

int DepthVisualParams_Forwarder::getDepthHeight()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_HEIGHT);
    m_port.write(cmd, response);
    return response.get(3).asInt32();
}

int DepthVisualParams_Forwarder::getDepthWidth()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_WIDTH);
    m_port.write(cmd, response);
    return response.get(3).asInt32();
}

bool DepthVisualParams_Forwarder::setDepthResolution(int width, int height)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_RESOLUTION);
    cmd.addInt32(width);
    cmd.addInt32(height);
    m_port.write(cmd, response);
    return response.get(2).asBool();
}

bool DepthVisualParams_Forwarder::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
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

bool DepthVisualParams_Forwarder::setDepthFOV(double horizontalFov, double verticalFov)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_FOV);
    cmd.addFloat64(horizontalFov);
    cmd.addFloat64(verticalFov);
    m_port.write(cmd, response);
    return response.get(2).asBool();
}

double DepthVisualParams_Forwarder::getDepthAccuracy()
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_ACCURACY);
    m_port.write(cmd, response);
    return response.get(3).asFloat64();
}

bool DepthVisualParams_Forwarder::setDepthAccuracy(double accuracy)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_ACCURACY);
    cmd.addFloat64(accuracy);
    m_port.write(cmd, response);
    return response.get(2).asBool();
}

bool DepthVisualParams_Forwarder::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_CLIP_PLANES);
    m_port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if ((response.get(0).asVocab32()) == VOCAB_FAILED) {
        nearPlane = 0;
        farPlane = 0;
        return false;
    }
    nearPlane = response.get(3).asFloat64();
    farPlane = response.get(4).asFloat64();
    return true;
}

bool DepthVisualParams_Forwarder::setDepthClipPlanes(double nearPlane, double farPlane)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_CLIP_PLANES);
    cmd.addFloat64(nearPlane);
    cmd.addFloat64(farPlane);
    m_port.write(cmd, response);
    return response.get(2).asBool();
}

bool DepthVisualParams_Forwarder::getDepthIntrinsicParam(yarp::os::Property& intrinsic)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_INTRINSIC_PARAM);
    m_port.write(cmd, response);

    // Minimal check on response, we suppose the response is always correctly formatted
    if ((response.get(0).asVocab32()) == VOCAB_FAILED) {
        intrinsic.clear();
        return false;
    }

    yarp::os::Property::copyPortable(response.get(3), intrinsic); // will it really work??
    return true;
}

bool DepthVisualParams_Forwarder::getDepthMirroring(bool& mirror)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_MIRROR);
    m_port.write(cmd, response);
    if ((response.get(0).asVocab32()) == VOCAB_FAILED) {
        return false;
    }
    mirror = response.get(3).asBool();
    return true;
}

bool DepthVisualParams_Forwarder::setDepthMirroring(bool mirror)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_DEPTH_VISUAL_PARAMS);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_MIRROR);
    cmd.addInt32(mirror);
    m_port.write(cmd, response);
    return response.get(2).asBool();
}
