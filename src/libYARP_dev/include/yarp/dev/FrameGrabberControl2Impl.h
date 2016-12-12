/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_FRAMEGRABBERCONTROL2IMPL_H
#define YARP_DEV_FRAMEGRABBERCONTROL2IMPL_H

#include <yarp/os/ConstString.h>
#include <yarp/dev/FrameGrabberInterfaces.h>        // to include VOCAB definitions

/*! \file FrameGrabberControl2.h define common interfaces to discover
 * remote camera capabilities */

namespace yarp{
    namespace dev {
        class FrameGrabberControls2_Parser;
        class FrameGrabberControls2_Sender;
    }
}


/**
 * This classes implement a sender / parser for IFrameGrabberControls2
 * interface messages
 */
class YARP_dev_API yarp::dev::FrameGrabberControls2_Sender: public yarp::dev::IFrameGrabberControls2
{
private:
    yarp::os::Port &_port;

public:
    FrameGrabberControls2_Sender(yarp::os::Port &port);
    virtual ~FrameGrabberControls2_Sender() {};
    virtual bool getCameraDescription(CameraDescriptor *camera);
    virtual bool hasFeature(int feature, bool *hasFeature);
    virtual bool setFeature(int feature, double value);
    virtual bool getFeature(int feature, double *value);
    virtual bool setFeature(int feature, double value1, double value2);
    virtual bool getFeature(int feature, double *value1, double *value2);
    virtual bool hasOnOff(int feature, bool *HasOnOff);
    virtual bool setActive(int feature, bool onoff);
    virtual bool getActive(int feature, bool *isActive);
    virtual bool hasAuto(int feature, bool *hasAuto);
    virtual bool hasManual(int feature, bool *hasManual);
    virtual bool hasOnePush(int feature, bool *hasOnePush);
    virtual bool setMode(int feature, FeatureMode mode);
    virtual bool getMode(int feature, FeatureMode *mode);
    virtual bool setOnePush(int feature);
};


class YARP_dev_API yarp::dev::FrameGrabberControls2_Parser : public yarp::dev::DeviceResponder
{
private:
    IFrameGrabberControls2 *fgCtrl2;

public:
    FrameGrabberControls2_Parser();
    virtual ~FrameGrabberControls2_Parser() {};

    bool configure(IFrameGrabberControls2 *interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);
};


#endif  // YARP_DEV_FRAMEGRABBERCONTROL2IMPL_H
