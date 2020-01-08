/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    int  getRgbHeight() override;
    int  getRgbWidth() override;
    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations) override;
    bool getRgbResolution(int &width, int &height) override;
    bool setRgbResolution(int width, int height) override;
    bool getRgbFOV(double &horizontalFov, double &verticalFov) override;
    bool setRgbFOV(double horizontalFov, double verticalFov) override;
    bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) override;
    bool getRgbMirroring(bool &mirror) override;
    bool setRgbMirroring(bool mirror) override;
};


class YARP_dev_API yarp::dev::Implement_RgbVisualParams_Parser : public yarp::dev::DeviceResponder
{
private:
    IRgbVisualParams *iRgbVisual;

public:
    Implement_RgbVisualParams_Parser();
    virtual ~Implement_RgbVisualParams_Parser() {};

    bool configure(IRgbVisualParams *interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
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

    int    getDepthHeight() override;
    int    getDepthWidth() override;
    bool   setDepthResolution(int width, int height) override;
    bool   getDepthFOV(double &horizontalFov, double &verticalFov) override;
    bool   setDepthFOV(double horizontalFov, double verticalFov) override;
    double getDepthAccuracy() override;
    bool   setDepthAccuracy(double accuracy) override;
    bool   getDepthClipPlanes(double &nearPlane, double &farPlane) override;
    bool   setDepthClipPlanes(double nearPlane, double farPlane) override;
    bool   getDepthIntrinsicParam(yarp::os::Property &intrinsic) override;
    bool   getDepthMirroring(bool &mirror) override;
    bool   setDepthMirroring(bool mirror) override;
};


class YARP_dev_API yarp::dev::Implement_DepthVisualParams_Parser : public yarp::dev::DeviceResponder
{
private:
    IDepthVisualParams *iDepthVisual;

public:
    Implement_DepthVisualParams_Parser();
    virtual ~Implement_DepthVisualParams_Parser() {};

    bool configure(IDepthVisualParams *interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

#endif  // YARP_DEV_IVISUALPARAMSIMPL_H
