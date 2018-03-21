/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis <k.theofilis@herts.ac.uk>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef OPENNI2_CLIENT_H
#define OPENNI2_CLIENT_H

#define PORTNAME_SKELETON "/userSkeleton"
#define PORTNAME_DEPTHFRAME "/depthFrame"
#define PORTNAME_IMAGEFRAME "/imageFrame"

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Network.h>
#include <yarp/dev/IOpenNI2DeviceDriver.h>
#include <yarp/sig/all.h>
#include <yarp/os/BufferedPort.h>
#include "OpenNI2SkeletonData.h"


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;

namespace yarp {
    namespace dev {
        class OpenNI2DeviceDriverClient;
    }
}

/**
 * @ingroup dev_impl_media
 *
 * A device implementation to get the sensor data from a sensor conected on another computer running the OpenNI2DeviceDriverServer device.
 * This implementation opens 4 ports to connect to yarp device OpenNI2DeviceDriverServer ports:
 *	- [clientName]:o - input port (does nothing)
 *	- [clientName]/userSkeleton:i - userSkeleton detection port
 *	- [clientName]/depthFrame:i - depth image port
 *	- [clientName]/imageFrame:i - rgb camera image port
 */
class yarp::dev::OpenNI2DeviceDriverClient : /*public GenericYarpDriver,*/ public TypedReaderCallback<ImageOf<PixelRgb> >,
public TypedReaderCallback<ImageOf<PixelMono16> >, public TypedReaderCallback<Bottle>, public yarp::dev::IOpenNI2DeviceDriver,
public yarp::dev::DeviceDriver {
public:
    OpenNI2DeviceDriverClient();
    // DeviceDriver
    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;
    // TypedReaderCallback
    using TypedReaderCallback<ImageOf<PixelRgb> >::onRead;
    using TypedReaderCallback<ImageOf<PixelMono16> >::onRead;
    using TypedReaderCallback<Bottle>::onRead;
    virtual void onRead(Bottle& b) override;
    virtual void onRead(ImageOf<PixelRgb>& img) override;
    virtual void onRead(ImageOf<PixelMono16>& img) override;
    // IOpenNI2DeviceDriver
    virtual bool getSkeletonOrientation(Vector *vectorArray, float *confidence,  int userID) override;
    virtual bool getSkeletonPosition(Vector *vectorArray, float *confidence,  int userID) override;
    virtual nite::SkeletonState getSkeletonState(int userID) override;
    virtual ImageOf<PixelRgb> getImageFrame() override;
    virtual ImageOf<PixelMono16> getDepthFrame() override;
private:
    BufferedPort<Bottle> *outPort;
    BufferedPort<Bottle> *inUserSkeletonPort;
    BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono16> > *inDepthFramePort;
    BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > *inImageFramePort;
    OpenNI2SkeletonData *skeletonData;
    /**
     * Connect the OpenNI2DeviceDriverClient ports with the OpenNI2DeviceDriverServer ports.
     *
     * @param remoteName server ports prefix
     * @param localName client ports prefix
     * @return true/false on success/failure to connect
     */
    bool connectPorts(string remotePortPrefix, string localPortPrefix);
};

/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_openni2_device_client openni2_device_client
 
 Client OpenNI2 device interface implementation
 
 */
#endif
