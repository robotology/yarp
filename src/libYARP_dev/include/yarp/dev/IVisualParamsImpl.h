/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_IMPLEMENT_VISUAL_INTERFACES
#define YARP_IMPLEMENT_VISUAL_INTERFACES

#include <yarp/dev/api.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IVisualParams.h>

// use namespace impl??
namespace yarp {
    namespace dev {
        class Implement_RgbVisualParams_Parser;
        class Implement_RgbVisualParams_Sender;
        class Implement_DepthVisualParams_Parser;
        class Implement_DepthVisualParams_Sender;
    }
}

//
//  RGB helpers
//
class YARP_dev_API yarp::dev::Implement_RgbVisualParams_Sender: public yarp::dev::IRgbVisualParams
{
protected:
    yarp::os::Port & _port;

public:
    Implement_RgbVisualParams_Sender(yarp::os::Port& port);
    virtual ~Implement_RgbVisualParams_Sender() {};

    virtual int  getRgbHeight();
    virtual int  getRgbWidth();
    virtual bool setRgbResolution(int width, int height);
    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov);
    virtual bool setRgbFOV(double horizontalFov, double verticalFov);
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic);
};


class YARP_dev_API yarp::dev::Implement_RgbVisualParams_Parser : public yarp::dev::DeviceResponder
{
private:
    IRgbVisualParams *iRgbVisual;

public:
    Implement_RgbVisualParams_Parser();
    virtual ~Implement_RgbVisualParams_Parser() {};

    bool configure(IRgbVisualParams *interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);
};


//
//  Depth helpers
//
class YARP_dev_API yarp::dev::Implement_DepthVisualParams_Sender: public IDepthVisualParams
{
private:
    yarp::os::Port & _port;

public:
    Implement_DepthVisualParams_Sender(os::Port &port);
    virtual ~Implement_DepthVisualParams_Sender() {};

    virtual int    getDepthHeight();
    virtual int    getDepthWidth();
    virtual bool   setDepthResolution(int width, int height);
    virtual bool   getDepthFOV(double &horizontalFov, double &verticalFov);
    virtual bool   setDepthFOV(double horizontalFov, double verticalFov);
    virtual double getDepthAccuracy();
    virtual bool   setDepthAccuracy(double accuracy);
    virtual bool   getDepthClipPlanes(double &near, double &far);
    virtual bool   setDepthClipPlanes(double near, double far);
    virtual bool   getDepthIntrinsicParam(yarp::os::Property &intrinsic);
};


class YARP_dev_API yarp::dev::Implement_DepthVisualParams_Parser : public yarp::dev::DeviceResponder
{
private:
    IDepthVisualParams *iDepthVisual;

public:
    Implement_DepthVisualParams_Parser();
    virtual ~Implement_DepthVisualParams_Parser() {};

    bool configure(IDepthVisualParams *interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);
};

#endif  // YARP_IMPLEMENT_VISUAL_INTERFACES
