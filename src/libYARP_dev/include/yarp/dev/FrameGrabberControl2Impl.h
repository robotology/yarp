/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_FRAMEGRABBERCONTROL2IMPL_H
#define YARP_DEV_FRAMEGRABBERCONTROL2IMPL_H

#include <string>
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
    virtual bool getCameraDescription(CameraDescriptor *camera) override;
    virtual bool hasFeature(int feature, bool *hasFeature) override;
    virtual bool setFeature(int feature, double value) override;
    virtual bool getFeature(int feature, double *value) override;
    virtual bool setFeature(int feature, double value1, double value2) override;
    virtual bool getFeature(int feature, double *value1, double *value2) override;
    virtual bool hasOnOff(int feature, bool *HasOnOff) override;
    virtual bool setActive(int feature, bool onoff) override;
    virtual bool getActive(int feature, bool *isActive) override;
    virtual bool hasAuto(int feature, bool *hasAuto) override;
    virtual bool hasManual(int feature, bool *hasManual) override;
    virtual bool hasOnePush(int feature, bool *hasOnePush) override;
    virtual bool setMode(int feature, FeatureMode mode) override;
    virtual bool getMode(int feature, FeatureMode *mode) override;
    virtual bool setOnePush(int feature) override;
};


class YARP_dev_API yarp::dev::FrameGrabberControls2_Parser : public yarp::dev::DeviceResponder
{
private:
    IFrameGrabberControls2 *fgCtrl2;

public:
    FrameGrabberControls2_Parser();
    virtual ~FrameGrabberControls2_Parser() {};

    bool configure(IFrameGrabberControls2 *interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};


#endif  // YARP_DEV_FRAMEGRABBERCONTROL2IMPL_H
