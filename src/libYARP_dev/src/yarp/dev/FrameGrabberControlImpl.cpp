/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/LogStream.h>
#include <yarp/dev/FrameGrabberControlImpl.h>
#include <yarp/dev/GenericVocabs.h>

using namespace yarp::os;
using namespace yarp::dev;

//
// Interface helper implementation
//

// Sender
FrameGrabberControls_Sender::FrameGrabberControls_Sender(Port& port) : _port(port) {};


bool FrameGrabberControls_Sender::getCameraDescription(CameraDescriptor* camera)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_CAMERA_DESCRIPTION);
    yInfo() << _port.isOpen();
    bool ret = _port.write(cmd,response);

    // response should be [fgc2] [camd] [is] [busType] [description]
    camera->busType = (BusType) response.get(3).asInt32();
    camera->deviceDescription = response.get(4).asString();
    return ret;
}

bool FrameGrabberControls_Sender::hasFeature(int feature, bool* hasFeature)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *hasFeature = response.get(4).asInt32() !=0? true:false;
    return ret;
}

bool FrameGrabberControls_Sender::setFeature(int feature, double value)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt32(feature);
    cmd.addFloat64(value);
    return _port.write(cmd,response);
}

bool FrameGrabberControls_Sender::setFeature(int feature, double  value1, double  value2)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FEATURE2);
    cmd.addInt32(feature);
    cmd.addFloat64(value1);
    cmd.addFloat64(value2);
    return _port.write(cmd,response);
}

bool FrameGrabberControls_Sender::getFeature(int feature, double* value)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *value = response.get(3).asFloat64();
    return ret;
}

bool FrameGrabberControls_Sender::getFeature(int feature, double *value1, double *value2)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FEATURE2);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *value1 = response.get(3).asFloat64();
    *value2 = response.get(4).asFloat64();
    return ret;
}

bool FrameGrabberControls_Sender::hasOnOff(int feature, bool* _hasOnOff)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONOFF);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *_hasOnOff = response.get(4).asInt32() !=0? true:false;
    return ret;
}

bool FrameGrabberControls_Sender::setActive(int feature, bool onoff)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt32(feature);
    cmd.addInt32(onoff);
    return _port.write(cmd,response);
}

bool FrameGrabberControls_Sender::getActive(int feature, bool* _isActive)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *_isActive = response.get(3).asInt32() !=0? true:false;
    return ret;
}

bool FrameGrabberControls_Sender::hasAuto(int feature, bool* _hasAuto)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_AUTO);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *_hasAuto = response.get(4).asInt32() !=0? true:false;;
    return ret;
}

bool FrameGrabberControls_Sender::hasManual(int feature, bool* _hasManual)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_MANUAL);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *_hasManual = response.get(4).asInt32() !=0? true:false;
    return ret;
}

bool FrameGrabberControls_Sender::hasOnePush(int feature, bool* _hasOnePush)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *_hasOnePush = response.get(4).asInt32() !=0? true:false;
    return ret;
}

bool FrameGrabberControls_Sender::setMode(int feature, FeatureMode mode)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_MODE);
    cmd.addInt32(feature);
    cmd.addInt32(mode);
    return _port.write(cmd,response);
}

bool FrameGrabberControls_Sender::getMode(int feature, FeatureMode* mode)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_MODE);
    cmd.addInt32(feature);
    bool ret = _port.write(cmd,response);

    *mode = (FeatureMode) response.get(3).asInt32();
    return ret;
}

bool FrameGrabberControls_Sender::setOnePush(int feature)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt32(feature);
    return _port.write(cmd,response);
}

//
//  Parser
//

FrameGrabberControls_Parser::FrameGrabberControls_Parser() : fgCtrl(nullptr) { }

bool FrameGrabberControls_Parser::configure(IFrameGrabberControls *interface)
{
    bool ret = false;
    if(interface)
    {
        fgCtrl = interface;
        ret = true;
    }
    else
    {
        fgCtrl = nullptr;
        ret = false;
    }
    return ret;
}


bool FrameGrabberControls_Parser::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ok = false;
    int action = cmd.get(1).asVocab();
    int param  = cmd.get(2).asVocab();

//     yTrace() << "cmd received\n\t" << cmd.toString().c_str();


    if(!fgCtrl)
    {
        yError() << " Selected camera device has no IFrameGrabberControl interface";
        return false;
    }

    response.clear();

    switch (action)
    {
        case VOCAB_HAS:
        {
            response.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
            response.addVocab(VOCAB_HAS);
            response.addVocab(VOCAB_FEATURE);
            response.addInt32(param);

            switch (param)
            {
                case VOCAB_FEATURE:
                {
                    bool _hasFeat;
                    ok = fgCtrl->hasFeature(cmd.get(3).asInt32(), &_hasFeat);
                    response.addInt32(_hasFeat);
                } break;

                case VOCAB_ONOFF:
                {
                    bool _hasOnOff;
                    ok = fgCtrl->hasOnOff(cmd.get(3).asInt32(), &_hasOnOff);
                    response.addInt32(_hasOnOff);
                } break;

                case VOCAB_AUTO:
                {
                    bool _hasAuto;

                    ok = fgCtrl->hasAuto(cmd.get(3).asInt32(), &_hasAuto);
                    response.addInt32(_hasAuto);
                } break;

                case VOCAB_MANUAL:
                {
                    bool _hasManual;
                    ok = fgCtrl->hasManual(cmd.get(3).asInt32(), &_hasManual);
                    response.addInt32(_hasManual);
                } break;

                case VOCAB_ONEPUSH:
                {
                    bool _hasOnePush;
                    ok = fgCtrl->hasOnePush(cmd.get(3).asInt32(), &_hasOnePush);
                    response.addInt32(_hasOnePush);
                } break;

                default:
                {
                    yError() << "Unknown command 'HAS " << Vocab::decode(param) << "' received on IFrameGrabber2 interface";
                    response.clear();
                    ok = false;
                } break;
            } break; // end switch (param)

        } break; // end VOCAB_HAS

        case VOCAB_SET:
        {
            switch (param)
            {
                case VOCAB_FEATURE:
                {
                    ok = fgCtrl->setFeature(cmd.get(3).asInt32(), cmd.get(4).asFloat64());
                } break;

                case VOCAB_FEATURE2:
                {
                    ok = fgCtrl->setFeature(cmd.get(3).asInt32(), cmd.get(4).asFloat64(), cmd.get(5).asFloat64());
                } break;

                case VOCAB_ACTIVE:
                {
                    ok = fgCtrl->setActive(cmd.get(3).asInt32(), cmd.get(4).asInt32());
                } break;

                case VOCAB_MODE:
                {
                    ok = fgCtrl->setMode(cmd.get(3).asInt32(), (FeatureMode) cmd.get(4).asInt32());
                } break;

                case VOCAB_ONEPUSH:
                {
                    ok = fgCtrl->setOnePush(cmd.get(3).asInt32());
                } break;

                default:
                {
                    yError() << "Unknown command 'SET " << Vocab::decode(param) << "' received on IFrameGrabber2 interface";
                    response.clear();
                    ok = false;
                }
            } break; // end switch (param)

        } break; // end VOCAB_SET

        case VOCAB_GET:
        {
            response.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
            response.addVocab(param);
            response.addVocab(VOCAB_IS);
            switch (param)
            {
                case VOCAB_CAMERA_DESCRIPTION:
                {
                    CameraDescriptor camera;
                    ok = fgCtrl->getCameraDescription(&camera);
                    response.addInt32(camera.busType);
                    response.addString(camera.deviceDescription);
                    yDebug() << "Response is " << response.toString();
                } break;

                case VOCAB_FEATURE:
                {
                    double value;
                    ok = fgCtrl->getFeature(cmd.get(3).asInt32(), &value);
                    response.addFloat64(value);
                } break;

                case VOCAB_FEATURE2:
                {
                    double value1, value2;
                    ok = fgCtrl->getFeature(cmd.get(3).asInt32(), &value1, &value2);
                    response.addFloat64(value1);
                    response.addFloat64(value2);
                } break;

                case VOCAB_ACTIVE:
                {
                    bool _isActive;
                    ok = fgCtrl->getActive(cmd.get(3).asInt32(), &_isActive);
                    response.addInt32(_isActive);
                } break;

                case VOCAB_MODE:
                {
                    FeatureMode _mode;
                    ok = fgCtrl->getMode(cmd.get(3).asInt32(), &_mode);
                    response.addInt32(_mode);
                } break;

                default:
                {
                    yError() << "Unknown command 'GET " << Vocab::decode(param) << "' received on IFrameGrabber2 interface";
                    response.clear();
                    ok = false;
                }

            } break; // end switch (param)

        } break; // end VOCAB_GET
    }
//     yTrace() << "response is\n\t" << response.toString().c_str();
    return ok;
}
