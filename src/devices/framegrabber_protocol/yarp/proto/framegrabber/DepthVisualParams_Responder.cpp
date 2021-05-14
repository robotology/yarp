/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "DepthVisualParams_Responder.h"
#include "CameraVocabs.h"

#include <yarp/os/LogStream.h>

using yarp::proto::framegrabber::DepthVisualParams_Responder;

DepthVisualParams_Responder::DepthVisualParams_Responder() :
        iDepthVisual(nullptr)
{
}
// DepthVisualParams_Responder::~DepthVisualParams_Responder() { }

bool DepthVisualParams_Responder::configure(yarp::dev::IDepthVisualParams* interface)
{
    bool ret = false;
    if (interface) {
        iDepthVisual = interface;
        ret = true;
    } else {
        iDepthVisual = nullptr;
        ret = false;
    }
    return ret;
}

bool DepthVisualParams_Responder::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ret = false;
    response.clear();
    if (!iDepthVisual) {
        yError() << "Depth Visual parameter Parser has not been correctly configured. IDepthVisualParams interface is not valid";
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    int code = cmd.get(0).asVocab();
    if (code != VOCAB_DEPTH_VISUAL_PARAMS) {
        yError() << "Depth Visual Params Parser received a command not belonging to this interface. Required interface was " << yarp::os::Vocab::decode(code);
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    switch (cmd.get(1).asVocab()) {
    case VOCAB_GET: {
        switch (cmd.get(2).asVocab()) {
        case VOCAB_HEIGHT: {
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_HEIGHT);
            response.addVocab(VOCAB_IS);
            response.addInt32(iDepthVisual->getDepthHeight());
        } break;

        case VOCAB_WIDTH: {
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_WIDTH);
            response.addVocab(VOCAB_IS);
            response.addInt32(iDepthVisual->getDepthWidth());
        } break;

        case VOCAB_FOV: {
            double hFov;
            double vFov;
            ret = iDepthVisual->getDepthFOV(hFov, vFov);
            if (ret) {
                response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                response.addVocab(VOCAB_FOV);
                response.addVocab(VOCAB_IS);
                response.addFloat64(hFov);
                response.addFloat64(vFov);
            } else {
                response.addVocab(VOCAB_FAILED);
            }
        } break;

        case VOCAB_INTRINSIC_PARAM: {
            yarp::os::Property params;
            ret = iDepthVisual->getDepthIntrinsicParam(params);
            if (ret) {
                response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                response.addVocab(VOCAB_INTRINSIC_PARAM);
                response.addVocab(VOCAB_IS);
                yarp::os::Bottle& tmp = response.addList();
                ret &= yarp::os::Property::copyPortable(params, tmp);
            } else {
                response.addVocab(VOCAB_FAILED);
            }
        } break;

        case VOCAB_ACCURACY: {
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_ACCURACY);
            response.addVocab(VOCAB_IS);
            response.addFloat64(iDepthVisual->getDepthAccuracy());
        } break;

        case VOCAB_CLIP_PLANES: {
            double nearPlane;
            double farPlane;
            iDepthVisual->getDepthClipPlanes(nearPlane, farPlane);
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_CLIP_PLANES);
            response.addVocab(VOCAB_IS);
            response.addFloat64(nearPlane);
            response.addFloat64(farPlane);
        } break;

        case VOCAB_MIRROR: {
            bool mirror;
            ret = iDepthVisual->getDepthMirroring(mirror);
            if (ret) {
                response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
                response.addVocab(VOCAB_MIRROR);
                response.addVocab(VOCAB_IS);
                response.addInt32(mirror);
            } else {
                response.addVocab(VOCAB_FAILED);
            }
        } break;

        default:
        {
            yError() << "Depth Visual Parameter interface parser received am unknown GET command. Command is " << cmd.toString();
            response.addVocab(VOCAB_FAILED);
            ret = false;
        } break;
        }
    } break;

    case VOCAB_SET: {
        switch (cmd.get(2).asVocab()) {
        case VOCAB_RESOLUTION: {
            ret = iDepthVisual->setDepthResolution(cmd.get(3).asInt32(), cmd.get(4).asInt32());
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_SET);
            response.addInt32(ret);
        } break;

        case VOCAB_FOV: {
            ret = iDepthVisual->setDepthFOV(cmd.get(3).asFloat64(), cmd.get(4).asFloat64());
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_SET);
            response.addInt32(ret);
        } break;

        case VOCAB_ACCURACY: {
            ret = iDepthVisual->setDepthAccuracy(cmd.get(3).asFloat64());
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_SET);
            response.addInt32(ret);
        } break;

        case VOCAB_CLIP_PLANES: {
            ret = iDepthVisual->setDepthClipPlanes(cmd.get(3).asFloat64(), cmd.get(4).asFloat64());
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_SET);
            response.addInt32(ret);
        } break;

        case VOCAB_MIRROR: {
            ret = iDepthVisual->setDepthMirroring(cmd.get(3).asBool());
            response.addVocab(VOCAB_DEPTH_VISUAL_PARAMS);
            response.addVocab(VOCAB_SET);
            response.addInt32(ret);
        } break;

        default:
        {
            yError() << "Rgb Visual Parameter interface parser received am unknown SET command. Command is " << cmd.toString();
            response.addVocab(VOCAB_FAILED);
            ret = false;
        } break;
        }
    } break;

    default:
    {
        yError() << "Rgb Visual parameter interface Parser received a malformed request. Command should either be 'set' or 'get', received " << cmd.toString();
        response.addVocab(VOCAB_FAILED);
        ret = false;
    } break;
    }
    return ret;
}
