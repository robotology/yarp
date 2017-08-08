/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Author: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_DEV_IVISUALPARAMSIMPL_H
#define YARP_DEV_IVISUALPARAMSIMPL_H

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

    virtual int  getRgbHeight() YARP_OVERRIDE;
    virtual int  getRgbWidth() YARP_OVERRIDE;
    virtual bool getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations) YARP_OVERRIDE;
    virtual bool getRgbResolution(int &width, int &height) YARP_OVERRIDE;
    virtual bool setRgbResolution(int width, int height) YARP_OVERRIDE;
    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov) YARP_OVERRIDE;
    virtual bool setRgbFOV(double horizontalFov, double verticalFov) YARP_OVERRIDE;
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) YARP_OVERRIDE;
    virtual bool getRgbMirroring(bool &mirror) YARP_OVERRIDE;
    virtual bool setRgbMirroring(bool mirror) YARP_OVERRIDE;
};


class YARP_dev_API yarp::dev::Implement_RgbVisualParams_Parser : public yarp::dev::DeviceResponder
{
private:
    IRgbVisualParams *iRgbVisual;

public:
    Implement_RgbVisualParams_Parser();
    virtual ~Implement_RgbVisualParams_Parser() {};

    bool configure(IRgbVisualParams *interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) YARP_OVERRIDE;
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

    virtual int    getDepthHeight() YARP_OVERRIDE;
    virtual int    getDepthWidth() YARP_OVERRIDE;
    virtual bool   setDepthResolution(int width, int height) YARP_OVERRIDE;
    virtual bool   getDepthFOV(double &horizontalFov, double &verticalFov) YARP_OVERRIDE;
    virtual bool   setDepthFOV(double horizontalFov, double verticalFov) YARP_OVERRIDE;
    virtual double getDepthAccuracy() YARP_OVERRIDE;
    virtual bool   setDepthAccuracy(double accuracy) YARP_OVERRIDE;
    virtual bool   getDepthClipPlanes(double &nearPlane, double &farPlane) YARP_OVERRIDE;
    virtual bool   setDepthClipPlanes(double nearPlane, double farPlane) YARP_OVERRIDE;
    virtual bool   getDepthIntrinsicParam(yarp::os::Property &intrinsic) YARP_OVERRIDE;
    virtual bool   getDepthMirroring(bool &mirror) YARP_OVERRIDE;
    virtual bool   setDepthMirroring(bool mirror) YARP_OVERRIDE;
};


class YARP_dev_API yarp::dev::Implement_DepthVisualParams_Parser : public yarp::dev::DeviceResponder
{
private:
    IDepthVisualParams *iDepthVisual;

public:
    Implement_DepthVisualParams_Parser();
    virtual ~Implement_DepthVisualParams_Parser() {};

    bool configure(IDepthVisualParams *interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) YARP_OVERRIDE;
};

#endif  // YARP_DEV_IVISUALPARAMSIMPL_H
