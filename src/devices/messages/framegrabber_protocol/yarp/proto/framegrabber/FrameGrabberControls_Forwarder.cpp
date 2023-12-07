/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabberControls_Forwarder.h"
#include "CameraVocabs.h"

#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>

using yarp::proto::framegrabber::FrameGrabberControls_Forwarder;

FrameGrabberControls_Forwarder::FrameGrabberControls_Forwarder(yarp::os::Port& m_port) :
        m_port(m_port)
{
};


bool FrameGrabberControls_Forwarder::getCameraDescription(CameraDescriptor* camera)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_CAMERA_DESCRIPTION);
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
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_HAS);
    cmd.addVocab32(VOCAB_FEATURE);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *hasFeature = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::setFeature(int feature, double value)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_FEATURE);
    cmd.addInt32(feature);
    cmd.addFloat64(value);
    return m_port.write(cmd, response);
}

bool FrameGrabberControls_Forwarder::setFeature(int feature, double value1, double value2)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_FEATURE2);
    cmd.addInt32(feature);
    cmd.addFloat64(value1);
    cmd.addFloat64(value2);
    return m_port.write(cmd, response);
}

bool FrameGrabberControls_Forwarder::getFeature(int feature, double* value)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_FEATURE);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *value = response.get(3).asFloat64();
    return ret;
}

bool FrameGrabberControls_Forwarder::getFeature(int feature, double* value1, double* value2)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_FEATURE2);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *value1 = response.get(3).asFloat64();
    *value2 = response.get(4).asFloat64();
    return ret;
}

bool FrameGrabberControls_Forwarder::hasOnOff(int feature, bool* hasOnOff)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_HAS);
    cmd.addVocab32(VOCAB_ONOFF);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *hasOnOff = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::setActive(int feature, bool onoff)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_ACTIVE);
    cmd.addInt32(feature);
    cmd.addInt32(onoff);
    return m_port.write(cmd, response);
}

bool FrameGrabberControls_Forwarder::getActive(int feature, bool* isActive)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_ACTIVE);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *isActive = response.get(3).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::hasAuto(int feature, bool* hasAuto)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_HAS);
    cmd.addVocab32(VOCAB_AUTO);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *hasAuto = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::hasManual(int feature, bool* hasManual)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_HAS);
    cmd.addVocab32(VOCAB_MANUAL);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *hasManual = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::hasOnePush(int feature, bool* hasOnePush)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_HAS);
    cmd.addVocab32(VOCAB_ONEPUSH);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *hasOnePush = response.get(4).asInt32() != 0 ? true : false;
    return ret;
}

bool FrameGrabberControls_Forwarder::setMode(int feature, FeatureMode mode)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_MODE);
    cmd.addInt32(feature);
    cmd.addInt32(mode);
    return m_port.write(cmd, response);
}

bool FrameGrabberControls_Forwarder::getMode(int feature, FeatureMode* mode)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_MODE);
    cmd.addInt32(feature);
    bool ret = m_port.write(cmd, response);

    *mode = static_cast<FeatureMode>(response.get(3).asInt32());
    return ret;
}

bool FrameGrabberControls_Forwarder::setOnePush(int feature)
{
    yarp::os::Bottle cmd;
    yarp::os::Bottle response;
    cmd.addVocab32(VOCAB_FRAMEGRABBER_CONTROL);
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_ONEPUSH);
    cmd.addInt32(feature);
    return m_port.write(cmd, response);
}
