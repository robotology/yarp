/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "FrameGrabberControls_Forwarder.h"
// #include "CameraVocabs.h" // FIXME
#include <yarp/dev/GenericVocabs.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>

using yarp::proto::framegrabber::FrameGrabberControls_Forwarder;

FrameGrabberControls_Forwarder::FrameGrabberControls_Forwarder(yarp::os::Port& port) :
        m_port(port)
{
};


bool FrameGrabberControls_Forwarder::getCameraDescription(CameraDescriptor* camera)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_CAMERA_DESCRIPTION);
    yInfo() << m_port.isOpen();
    bool ret = m_port.write(cmd, response);

    // response should be [fgc2] [camd] [is] [busType] [description]
    camera->busType = static_cast<BusType>(response.get(3).asInt32());
    camera->deviceDescription = response.get(4).asString();
    return ret;
}

bool FrameGrabberControls_Forwarder::hasFeature(int feature, bool* hasFeature)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *hasFeature = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::setFeature(int feature, double value)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt32(feature);
    cmd.addFloat64(value);
    return m_port.write(cmd, response);
}

bool FrameGrabberControls_Forwarder::setFeature(int feature, double value1, double value2)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_FEATURE2);
    cmd.addInt32(feature);
    cmd.addFloat64(value1);
    cmd.addFloat64(value2);
    return m_port.write(cmd, response);
}

bool FrameGrabberControls_Forwarder::getFeature(int feature, double* value)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FEATURE);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *value = response.get(3).asFloat64();
    return ret;
}

bool FrameGrabberControls_Forwarder::getFeature(int feature, double* value1, double* value2)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_FEATURE2);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *value1 = response.get(3).asFloat64();
    *value2 = response.get(4).asFloat64();
    return ret;
}

bool FrameGrabberControls_Forwarder::hasOnOff(int feature, bool* _hasOnOff)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONOFF);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *_hasOnOff = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::setActive(int feature, bool onoff)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt32(feature);
    cmd.addInt32(onoff);
    return m_port.write(cmd, response);
}

bool FrameGrabberControls_Forwarder::getActive(int feature, bool* _isActive)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ACTIVE);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *_isActive = response.get(3).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::hasAuto(int feature, bool* _hasAuto)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_AUTO);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *_hasAuto = response.get(4).asInt32() != 0 ? true : false;
    ;
    return ret;
}

bool FrameGrabberControls_Forwarder::hasManual(int feature, bool* _hasManual)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_MANUAL);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *_hasManual = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::hasOnePush(int feature, bool* _hasOnePush)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_HAS);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *_hasOnePush = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::setMode(int feature, FeatureMode mode)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_MODE);
    cmd.addInt32(feature);
    cmd.addInt32(mode);
    return m_port.write(cmd, response);
}

bool FrameGrabberControls_Forwarder::getMode(int feature, FeatureMode* mode)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_MODE);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *mode = static_cast<FeatureMode>(response.get(3).asInt32());
    return ret;
}

bool FrameGrabberControls_Forwarder::setOnePush(int feature)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ONEPUSH);
    cmd.addInt32(feature);
    return m_port.write(cmd, response);
}
