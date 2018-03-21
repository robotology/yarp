/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis <k.theofilis@herts.ac.uk>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef OPENNI2_SERVER_H
#define OPENNI2_SERVER_H

#include <iostream>
#include <string>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/all.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/os/all.h>
#include <yarp/os/Stamp.h>
#include "OpenNI2SkeletonTracker.h"

// The interface is taken from the client
#include <yarp/dev/IOpenNI2DeviceDriver.h>

#define PORTNAME_SKELETON "/userSkeleton"
#define PORTNAME_DEPTHFRAME "/depthFrame"
#define PORTNAME_IMAGEFRAME "/imageFrame"

#define USER_DETECTED_MSG "[USER_DETECTED]"
#define USER_CALIBRATING_MSG "[USER_CALIBRATING]"
#define USER_LOST_MSG "[USER_LOST]"
#define USER_VOCAB VOCAB4('U','S','E','R')
#define DMAP_VOCAB VOCAB4('D','M','A','P')
#define POSITION_VOCAB VOCAB3('P','O','S')
#define ORIENTATION_VOCAB VOCAB3('O','R','I')

using namespace std;
using namespace yarp::sig;
using namespace yarp::os;

namespace yarp {
    namespace dev {
        class OpenNI2DeviceDriverServer;
    }
}

/**
 * @ingroup dev_impl_media
 *
 * An OpenNI2 sensor device implementation to get the data from a sensor conected locally.
 * This implementation opens 4 ports:
 *  - [portPrefix]:i - input port (does nothing)
 *  - [portPrefix]/userSkeleton:o - userSkeleton detection port (only opened if user detection is on)
 *  - [portPrefix]/depthFrame:o - depth frame port
 *  - [portPrefix]/imageFrame:o - rgb camera frame port
 */
class yarp::dev::OpenNI2DeviceDriverServer: public yarp::dev::DeviceDriver,
                                            public yarp::dev::IService,
                                            public yarp::dev::IOpenNI2DeviceDriver
{
public:
    OpenNI2DeviceDriverServer();
    ~OpenNI2DeviceDriverServer();
    // GenericYarpDriver
    virtual bool updateInterface();
    // DeviceDriver
    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;
    // IService
    virtual bool startService() override;
    virtual bool updateService() override;
    virtual bool stopService() override;
    // IOpenNI2DeviceServer
    virtual bool getSkeletonOrientation(Vector *vectorArray, float *confidence,  int userID) override;
    virtual bool getSkeletonPosition(Vector *vectorArray, float *confidence,  int userID) override;

// #ifdef OPENNI2_DRIVER_USES_NITE2
    virtual nite::SkeletonState getSkeletonState(int userID) override;
// #endif
    virtual ImageOf<PixelRgb> getImageFrame() override;
    virtual ImageOf<PixelMono16> getDepthFrame() override;
private:
    BufferedPort<Bottle> *skeletonPort;
    BufferedPort<Bottle> *receivingPort;
    BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono16> > *depthFramePort;
    BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > *imageFramePort;
    OpenNI2SkeletonTracker *skeleton;
    bool withOpenPorts, userTracking, colorON, depthMirrorON, rgbMirrorON, oniPlayback, oniRecord, loop, frameSync, imageRegistration;
    string fileDevice;
    string oniOutputFile;

    /**
     * Opens the depth sensor and rgb camera image ports
     */
    void openPorts(string portPrefix, bool withUserTracking, bool withColorOn);
    /**
     * Sends the sensor data to the rgb and depth frame ports. Also sends the userSkeleton data to the mainBottle port.
     *
     * @param mainBottle BufferedPort for the userSkeleton data (only needed if user detection is on)
     */
    void sendSensorData();
};

/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_openni2_device_server openni2_device_server

 OpenNI2 device interface implementation - http://wiki.icub.org/wiki/OpenNI2

 */
#endif
