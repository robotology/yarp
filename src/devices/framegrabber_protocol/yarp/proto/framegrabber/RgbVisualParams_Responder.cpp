/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "RgbVisualParams_Responder.h"
#include "CameraVocabs.h"

#include <yarp/os/LogStream.h>

using yarp::proto::framegrabber::RgbVisualParams_Responder;

bool RgbVisualParams_Responder::configure(yarp::dev::IRgbVisualParams* interface)
{
    bool ret = false;
    if (interface) {
        iRgbVisual = interface;
        ret = true;
    } else {
        iRgbVisual = nullptr;
        ret = false;
    }
    return ret;
}

bool RgbVisualParams_Responder::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ret = false;
    response.clear();
    if (!iRgbVisual) {
        yError() << "Rgb Visual parameter Parser has not been correctly configures. IRgbVisualParams interface is not valid";
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    int code = cmd.get(0).asVocab();
    if (code != VOCAB_RGB_VISUAL_PARAMS) {
        yError() << "Rgb Visual Params Parser received a command not belonging to this interface. Required interface is " << yarp::os::Vocab::decode(code);
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    switch (cmd.get(1).asVocab()) {
    case VOCAB_GET: {
        switch (cmd.get(2).asVocab()) {
        case VOCAB_HEIGHT:
            response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
            response.addVocab(VOCAB_HEIGHT);
            response.addVocab(VOCAB_IS);
            response.addInt32(iRgbVisual->getRgbHeight());
            ret = true;
            break;

        case VOCAB_WIDTH:
            response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
            response.addVocab(VOCAB_WIDTH);
            response.addVocab(VOCAB_IS);
            response.addInt32(iRgbVisual->getRgbWidth());
            ret = true;
            break;

        case VOCAB_SUPPORTED_CONF: {
            yarp::sig::VectorOf<yarp::dev::CameraConfig> conf;
            ret = iRgbVisual->getRgbSupportedConfigurations(conf);
            if (ret) {
                response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                response.addVocab(VOCAB_SUPPORTED_CONF);
                response.addVocab(VOCAB_IS);
                response.addInt32(conf.size());
                for (const auto& i : conf) {
                    response.addInt32(i.width);
                    response.addInt32(i.height);
                    response.addFloat64(i.framerate);
                    response.addVocab(i.pixelCoding);
                }
            } else {
                response.addVocab(VOCAB_FAILED);
            }
        } break;


        case VOCAB_RESOLUTION: {
            int width;
            int height;
            ret = iRgbVisual->getRgbResolution(width, height);
            if (ret) {
                response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                response.addVocab(VOCAB_RESOLUTION);
                response.addVocab(VOCAB_IS);
                response.addInt32(width);
                response.addInt32(height);
            } else {
                response.addVocab(VOCAB_FAILED);
            }
        } break;

        case VOCAB_FOV: {
            double hFov;
            double vFov;
            ret = iRgbVisual->getRgbFOV(hFov, vFov);
            if (ret) {
                response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
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
            ret = iRgbVisual->getRgbIntrinsicParam(params);
            if (ret) {
                response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                response.addVocab(VOCAB_INTRINSIC_PARAM);
                response.addVocab(VOCAB_IS);
                yarp::os::Bottle& tmp = response.addList();
                ret &= yarp::os::Property::copyPortable(params, tmp);
            } else {
                response.addVocab(VOCAB_FAILED);
            }
        } break;

        case VOCAB_MIRROR: {
            bool mirror;
            ret = iRgbVisual->getRgbMirroring(mirror);
            if (ret) {
                response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
                response.addVocab(VOCAB_MIRROR);
                response.addVocab(VOCAB_IS);
                response.addInt32(mirror);
            } else {
                response.addVocab(VOCAB_FAILED);
            }
        } break;

        default:
        {
            yError() << "Rgb Visual Parameter interface parser received an unknown GET command. Command is " << cmd.toString();
            response.addVocab(VOCAB_FAILED);
            ret = false;
        } break;
        }
    } break;

    case VOCAB_SET: {
        switch (cmd.get(2).asVocab()) {
        case VOCAB_RESOLUTION:
            ret = iRgbVisual->setRgbResolution(cmd.get(3).asInt32(), cmd.get(4).asInt32());
            response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
            response.addVocab(VOCAB_SET);
            response.addInt32(ret);
            break;

        case VOCAB_FOV:
            ret = iRgbVisual->setRgbFOV(cmd.get(3).asFloat64(), cmd.get(4).asFloat64());
            response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
            response.addVocab(VOCAB_SET);
            response.addInt32(ret);
            break;

        case VOCAB_MIRROR: {
            ret = iRgbVisual->setRgbMirroring(cmd.get(3).asBool());
            response.addVocab(VOCAB_RGB_VISUAL_PARAMS);
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
    } // end VOCAB
    break;

    default:
    {
        yError() << "Rgb Visual parameter interface Parser received a malformed request. Command should either be 'set' or 'get', received " << cmd.toString();
        response.addVocab(VOCAB_FAILED);
        ret = false;
    } break;
    }
    return ret;
}
