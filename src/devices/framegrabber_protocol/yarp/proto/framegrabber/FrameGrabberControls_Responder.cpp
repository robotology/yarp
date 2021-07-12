/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabberControls_Responder.h"
#include "CameraVocabs.h"

#include <yarp/os/LogStream.h>

using yarp::proto::framegrabber::FrameGrabberControls_Responder;

FrameGrabberControls_Responder::FrameGrabberControls_Responder() :
        fgCtrl(nullptr)
{
}

bool FrameGrabberControls_Responder::configure(yarp::dev::IFrameGrabberControls* interface)
{
    bool ret = false;
    if (interface) {
        fgCtrl = interface;
        ret = true;
    } else {
        fgCtrl = nullptr;
        ret = false;
    }
    return ret;
}


bool FrameGrabberControls_Responder::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ok = false;
    int action = cmd.get(1).asVocab32();
    int param = cmd.get(2).asVocab32();

    if (!fgCtrl) {
        yError() << " Selected camera device has no IFrameGrabberControl interface";
        return false;
    }

    response.clear();

    switch (action) {
    case VOCAB_HAS: {
        response.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
        response.addVocab32(VOCAB_HAS);
        response.addVocab32(VOCAB_FEATURE);
        response.addInt32(param);

        switch (param) {
        case VOCAB_FEATURE: {
            bool _hasFeat;
            ok = fgCtrl->hasFeature(cmd.get(3).asInt32(), &_hasFeat);
            response.addInt32(_hasFeat);
        } break;

        case VOCAB_ONOFF: {
            bool _hasOnOff;
            ok = fgCtrl->hasOnOff(cmd.get(3).asInt32(), &_hasOnOff);
            response.addInt32(_hasOnOff);
        } break;

        case VOCAB_AUTO: {
            bool _hasAuto;

            ok = fgCtrl->hasAuto(cmd.get(3).asInt32(), &_hasAuto);
            response.addInt32(_hasAuto);
        } break;

        case VOCAB_MANUAL: {
            bool _hasManual;
            ok = fgCtrl->hasManual(cmd.get(3).asInt32(), &_hasManual);
            response.addInt32(_hasManual);
        } break;

        case VOCAB_ONEPUSH: {
            bool _hasOnePush;
            ok = fgCtrl->hasOnePush(cmd.get(3).asInt32(), &_hasOnePush);
            response.addInt32(_hasOnePush);
        } break;

        default:
        {
            yError() << "Unknown command 'HAS " << yarp::os::Vocab32::decode(param) << "' received on IFrameGrabber2 interface";
            response.clear();
            ok = false;
        } break;
        }
        break; // end switch (param)

    } break; // end VOCAB_HAS

    case VOCAB_SET: {
        switch (param) {
        case VOCAB_FEATURE: {
            ok = fgCtrl->setFeature(cmd.get(3).asInt32(), cmd.get(4).asFloat64());
        } break;

        case VOCAB_FEATURE2: {
            ok = fgCtrl->setFeature(cmd.get(3).asInt32(), cmd.get(4).asFloat64(), cmd.get(5).asFloat64());
        } break;

        case VOCAB_ACTIVE: {
            ok = fgCtrl->setActive(cmd.get(3).asInt32(), cmd.get(4).asInt32());
        } break;

        case VOCAB_MODE: {
            ok = fgCtrl->setMode(cmd.get(3).asInt32(), static_cast<FeatureMode>(cmd.get(4).asInt32()));
        } break;

        case VOCAB_ONEPUSH: {
            ok = fgCtrl->setOnePush(cmd.get(3).asInt32());
        } break;

        default:
        {
            yError() << "Unknown command 'SET " << yarp::os::Vocab32::decode(param) << "' received on IFrameGrabber2 interface";
            response.clear();
            ok = false;
        }
        }
        break; // end switch (param)

    } break; // end VOCAB_SET

    case VOCAB_GET: {
        response.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
        response.addVocab32(param);
        response.addVocab32(VOCAB_IS);
        switch (param) {
        case VOCAB_CAMERA_DESCRIPTION: {
            CameraDescriptor camera;
            ok = fgCtrl->getCameraDescription(&camera);
            response.addInt32(camera.busType);
            response.addString(camera.deviceDescription);
            yDebug() << "Response is " << response.toString();
        } break;

        case VOCAB_FEATURE: {
            double value;
            ok = fgCtrl->getFeature(cmd.get(3).asInt32(), &value);
            response.addFloat64(value);
        } break;

        case VOCAB_FEATURE2: {
            double value1;
            double value2;
            ok = fgCtrl->getFeature(cmd.get(3).asInt32(), &value1, &value2);
            response.addFloat64(value1);
            response.addFloat64(value2);
        } break;

        case VOCAB_ACTIVE: {
            bool _isActive;
            ok = fgCtrl->getActive(cmd.get(3).asInt32(), &_isActive);
            response.addInt32(_isActive);
        } break;

        case VOCAB_MODE: {
            FeatureMode _mode;
            ok = fgCtrl->getMode(cmd.get(3).asInt32(), &_mode);
            response.addInt32(_mode);
        } break;

        default:
        {
            yError() << "Unknown command 'GET " << yarp::os::Vocab32::decode(param) << "' received on IFrameGrabber2 interface";
            response.clear();
            ok = false;
        }
        }
        break; // end switch (param)

    } break; // end VOCAB_GET
    }
    //     yTrace() << "response is\n\t" << response.toString().c_str();
    return ok;
}
