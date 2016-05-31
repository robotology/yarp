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

#ifdef _MSC_VER
/* This template is used to define a non private member variable of RemoteFrameGrabber (reader).
As such it needs to be explicitly exported (warning C4251). The warnig could be suppressed altogether 
by defining the member private thus ensuring that it is never used dirctly in user code.*/
template class YARP_dev_API yarp::os::PortReaderBuffer<yarp::sig::ImageOf<yarp::sig::PixelRgb> >;
#endif

// should move more of implementation into this file

// just to keep linkers from complaining about empty archive
bool dummyRemoteFrameGrabberMethod() {
    return false;
}

bool RemoteFrameGrabber::getCameraDescription(CameraDescriptor* camera)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_CAMERA_DESCRIPTION);
    bool ret = port.write(cmd,response);

    // response should be [fgc2] [camd] [is] [busType] [description]
    camera->busType = (BusType) response.get(3).asInt();
    camera->deviceDescription = response.get(4).asString();
    return ret;
}

bool RemoteFrameGrabber::hasFeature(int feature, bool* hasFeature)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *hasFeature = response.get(4).asInt() !=0? true:false;
    return ret;
}

bool RemoteFrameGrabber::setFeature(int feature, double value)
{
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
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONOFF);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_hasOnOff = response.get(4).asInt() !=0? true:false;
    return ret;
}

bool RemoteFrameGrabber::setActive(int feature, bool onoff)
{
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
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_isActive = response.get(3).asInt() !=0? true:false;
    return ret;
}

bool RemoteFrameGrabber::hasAuto(int feature, bool* _hasAuto)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_AUTO);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_hasAuto = response.get(4).asInt() !=0? true:false;;
    return ret;
}

bool RemoteFrameGrabber::hasManual(int feature, bool* _hasManual)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_MANUAL);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_hasManual = response.get(4).asInt() !=0? true:false;
    return ret;
}

bool RemoteFrameGrabber::hasOnePush(int feature, bool* _hasOnePush)
{
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt(feature);
    bool ret = port.write(cmd,response);

    *_hasOnePush = response.get(4).asInt() !=0? true:false;
    return ret;
}

bool RemoteFrameGrabber::setMode(int feature, FeatureMode mode)
{
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
    yarp::os::Bottle cmd, response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL2);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt(feature);
    return port.write(cmd,response);
}
