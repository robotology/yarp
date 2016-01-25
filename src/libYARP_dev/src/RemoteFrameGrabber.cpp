// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium, European Commission FP6 Project IST-004370
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * Author: Paul Fitzpatrick, Alessandro Scalzo
*/


#include <yarp/dev/TestFrameGrabber.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/RemoteFrameGrabber.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// should move more of implementation into this file

// just to keep linkers from complaining about empty archive
bool dummyRemoteFrameGrabberMethod() {
    return false;
}

bool RemoteFrameGrabber::getCameraDescription(CameraDescriptor* camera)
{
    yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_CAMERA_DESCRIPTION);
    bool ret = port.write(cmd,response);

    // response should be [fgc2] [camd] [is] [busType] [description]
    camera->busType = (BusType) response.get(3).asInt();
    camera->deviceDescription = response.get(4).asString();

    yDebug() << "ret is " << ret << "; resp is " << response.toString();
    yDebug() << "Type is " << camera->busType << " description is " << camera->deviceDescription;
    return true;
}

bool RemoteFrameGrabber::hasFeature(int feature, bool* hasFeature)
{
    yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *hasFeature = (bool) response.get(4).asInt() ;
    return ret;
}

bool RemoteFrameGrabber::setFeature(int feature, double value)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt(feature);
    cmd.addDouble(value);
    return port.write(cmd,response);
}

bool RemoteFrameGrabber::setFeature(int feature, double  value1, double  value2)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FEATURE2);
    cmd.addInt(feature);
    cmd.addDouble(value1);
    cmd.addDouble(value2);
    return port.write(cmd,response);
}

bool RemoteFrameGrabber::getFeature(int feature, double* value)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *value = response.get(3).asDouble();
    return ret;
}

bool RemoteFrameGrabber::getFeature(int feature, double *value1, double *value2)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FEATURE2);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *value1 = response.get(3).asDouble();
    *value2 = response.get(4).asDouble();
    return ret;
}

bool RemoteFrameGrabber::hasOnOff(int feature, bool* _hasOnOff)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONOFF);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_hasOnOff = (bool) response.get(4).asInt();
    return ret;
}

bool RemoteFrameGrabber::setActive(int feature, bool onoff)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt(feature);
    cmd.addInt(onoff);
    return port.write(cmd,response);
}

bool RemoteFrameGrabber::getActive(int feature, bool* _isActive)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_isActive = (bool) response.get(3).asInt();
    return ret;
}

bool RemoteFrameGrabber::hasAuto(int feature, bool* _hasAuto)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_AUTO);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    yDebug() << "HAS_AUTO response is " << response.toString();
    *_hasAuto = (bool) response.get(4).asInt();
    return ret;
}

bool RemoteFrameGrabber::hasManual(int feature, bool* _hasManual)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_MANUAL);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_hasManual = response.get(4).asInt();
    return ret;
}

bool RemoteFrameGrabber::hasOnePush(int feature, bool* _hasOnePush)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_hasOnePush = (bool) response.get(4).asInt();
    return ret;
}

bool RemoteFrameGrabber::setMode(int feature, FeatureMode mode)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_MODE);
    cmd.addInt(feature);
    cmd.addInt(mode);
    return port.write(cmd,response);
}

bool RemoteFrameGrabber::getMode(int feature, FeatureMode* mode)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_MODE);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *mode = (FeatureMode) response.get(3).asInt();
    return ret;
}

bool RemoteFrameGrabber::setOnePush(int feature)
{
//     yTrace();
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt(feature);
    return port.write(cmd,response);
}
