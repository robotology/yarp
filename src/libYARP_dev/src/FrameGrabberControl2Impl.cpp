/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/LogStream.h>
#include <yarp/dev/FrameGrabberControl2Impl.h>

using namespace yarp::os;
using namespace yarp::dev;

//
// Interface helper implementation
//

// Sender
FrameGrabberControls2_Sender::FrameGrabberControls2_Sender(Port& port) : _port(port) {};


bool FrameGrabberControls2_Sender::getCameraDescription(CameraDescriptor* camera)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_CAMERA_DESCRIPTION);
    yInfo() << _port.isOpen();
    bool ret = _port.write(cmd,response);

    // response should be [fgc2] [camd] [is] [busType] [description]
    camera->busType = (BusType) response.get(3).asInt();
    camera->deviceDescription = response.get(4).asString();
    return ret;
}

bool FrameGrabberControls2_Sender::hasFeature(int feature, bool* hasFeature)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *hasFeature = response.get(4).asInt() !=0? true:false;
    return ret;
}

bool FrameGrabberControls2_Sender::setFeature(int feature, double value)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt(feature);
    cmd.addDouble(value);
    return _port.write(cmd,response);
}

bool FrameGrabberControls2_Sender::setFeature(int feature, double  value1, double  value2)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FEATURE2);
    cmd.addInt(feature);
    cmd.addDouble(value1);
    cmd.addDouble(value2);
    return _port.write(cmd,response);
}

bool FrameGrabberControls2_Sender::getFeature(int feature, double* value)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *value = response.get(3).asDouble();
    return ret;
}

bool FrameGrabberControls2_Sender::getFeature(int feature, double *value1, double *value2)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FEATURE2);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *value1 = response.get(3).asDouble();
    *value2 = response.get(4).asDouble();
    return ret;
}

bool FrameGrabberControls2_Sender::hasOnOff(int feature, bool* _hasOnOff)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONOFF);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *_hasOnOff = response.get(4).asInt() !=0? true:false;
    return ret;
}

bool FrameGrabberControls2_Sender::setActive(int feature, bool onoff)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt(feature);
    cmd.addInt(onoff);
    return _port.write(cmd,response);
}

bool FrameGrabberControls2_Sender::getActive(int feature, bool* _isActive)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *_isActive = response.get(3).asInt() !=0? true:false;
    return ret;
}

bool FrameGrabberControls2_Sender::hasAuto(int feature, bool* _hasAuto)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_AUTO);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *_hasAuto = response.get(4).asInt() !=0? true:false;;
    return ret;
}

bool FrameGrabberControls2_Sender::hasManual(int feature, bool* _hasManual)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_MANUAL);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *_hasManual = response.get(4).asInt() !=0? true:false;
    return ret;
}

bool FrameGrabberControls2_Sender::hasOnePush(int feature, bool* _hasOnePush)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *_hasOnePush = response.get(4).asInt() !=0? true:false;
    return ret;
}

bool FrameGrabberControls2_Sender::setMode(int feature, FeatureMode mode)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_MODE);
    cmd.addInt(feature);
    cmd.addInt(mode);
    return _port.write(cmd,response);
}

bool FrameGrabberControls2_Sender::getMode(int feature, FeatureMode* mode)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_MODE);
    cmd.addInt(feature);
    bool ret = _port.write(cmd,response);

    *mode = (FeatureMode) response.get(3).asInt();
    return ret;
}

bool FrameGrabberControls2_Sender::setOnePush(int feature)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt(feature);
    return _port.write(cmd,response);
}

//
//  Parser
//

FrameGrabberControls2_Parser::FrameGrabberControls2_Parser() : fgCtrl2(nullptr) { }

bool FrameGrabberControls2_Parser::configure(IFrameGrabberControls2* interface)
{
    bool ret = false;
    if(interface)
    {
        fgCtrl2 = interface;
        ret = true;
    }
    else
    {
        fgCtrl2 = nullptr;
        ret = false;
    }
    return ret;
}


bool FrameGrabberControls2_Parser::respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response)
{
    bool ok = false;
    int action = cmd.get(1).asVocab();
    int param  = cmd.get(2).asVocab();

//     yTrace() << "cmd received\n\t" << cmd.toString().c_str();


    if(!fgCtrl2)
    {
        yError() << " Selected camera device has no IFrameGrabberControl2 interface";
        return false;
    }

    response.clear();

    switch (action)
    {
        case VOCAB_HAS:
        {
            response.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
            response.addVocab(VOCAB_HAS);
            response.addVocab(VOCAB_FEATURE);
            response.addInt(param);

            switch (param)
            {
                case VOCAB_FEATURE:
                {
                    bool _hasFeat;
                    ok = fgCtrl2->hasFeature(cmd.get(3).asInt(), &_hasFeat);
                    response.addInt(_hasFeat);
                } break;

                case VOCAB_ONOFF:
                {
                    bool _hasOnOff;
                    ok = fgCtrl2->hasOnOff(cmd.get(3).asInt(), &_hasOnOff);
                    response.addInt(_hasOnOff);
                } break;

                case VOCAB_AUTO:
                {
                    bool _hasAuto;
                    ok = fgCtrl2->hasAuto(cmd.get(3).asInt(), &_hasAuto);
                    response.addInt(_hasAuto);
                } break;

                case VOCAB_MANUAL:
                {
                    bool _hasManual;
                    ok = fgCtrl2->hasManual(cmd.get(3).asInt(), &_hasManual);
                    response.addInt(_hasManual);
                } break;

                case VOCAB_ONEPUSH:
                {
                    bool _hasOnePush;
                    ok = fgCtrl2->hasOnePush(cmd.get(3).asInt(), &_hasOnePush);
                    response.addInt(_hasOnePush);
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
                    ok = fgCtrl2->setFeature(cmd.get(3).asInt(), cmd.get(4).asDouble());
                } break;

                case VOCAB_FEATURE2:
                {
                    ok = fgCtrl2->setFeature(cmd.get(3).asInt(), cmd.get(4).asDouble(), cmd.get(5).asDouble());
                } break;

                case VOCAB_ACTIVE:
                {
                    ok = fgCtrl2->setActive(cmd.get(3).asInt(), cmd.get(4).asInt());
                } break;

                case VOCAB_MODE:
                {
                    ok = fgCtrl2->setMode(cmd.get(3).asInt(), (FeatureMode) cmd.get(4).asInt());
                } break;

                case VOCAB_ONEPUSH:
                {
                    ok = fgCtrl2->setOnePush(cmd.get(3).asInt());
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
            response.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
            response.addVocab(param);
            response.addVocab(VOCAB_IS);
            switch (param)
            {
                case VOCAB_CAMERA_DESCRIPTION:
                {
                    CameraDescriptor camera;
                    ok = fgCtrl2->getCameraDescription(&camera);
                    response.addInt(camera.busType);
                    response.addString(camera.deviceDescription);
                    yDebug() << "Response is " << response.toString();
                } break;

                case VOCAB_FEATURE:
                {
                    double value;
                    ok = fgCtrl2->getFeature(cmd.get(3).asInt(), &value);
                    response.addDouble(value);
                } break;

                case VOCAB_FEATURE2:
                {
                    double value1, value2;
                    ok = fgCtrl2->getFeature(cmd.get(3).asInt(), &value1, &value2);
                    response.addDouble(value1);
                    response.addDouble(value2);
                } break;

                case VOCAB_ACTIVE:
                {
                    bool _isActive;
                    ok = fgCtrl2->getActive(cmd.get(3).asInt(), &_isActive);
                    response.addInt(_isActive);
                } break;

                case VOCAB_MODE:
                {
                    FeatureMode _mode;
                    ok = fgCtrl2->getMode(cmd.get(3).asInt(), &_mode);
                    response.addInt(_mode);
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
