// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis, University of Hertfordshire, k.theofilis@herts.ac.uk
 * Authors: Duarte Aragao, Konstantinos Theofilis
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
 * An OpenNI2 sensor device implementation to get the kinect data from a kinect conected locally.
 * This implementation opens 4 ports:
 *	- [portPrefix]:i - input port (does nothing)
 *	- [portPrefix]/userSkeleton:o - userSkeleton detection port (only opened if user detection is on)
 *	- [portPrefix]/depthFrame:o - depth frame port
 *	- [portPrefix]/imageFrame:o - rgb camera frame port
 */
class yarp::dev::OpenNI2DeviceDriverServer: public IService, public yarp::dev::IOpenNI2DeviceDriver,
public yarp::dev::DeviceDriver{
public:
    OpenNI2DeviceDriverServer(void);
    ~OpenNI2DeviceDriverServer(void);
    // GenericYarpDriver
    virtual bool updateInterface();
    // DeviceDriver
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();
    // IService
    virtual bool startService();
    virtual bool updateService();
    virtual bool stopService();
    // IKinectDeviceDriver
    virtual bool getSkeletonOrientation(Vector *vectorArray, float *confidence,  int userID);
    virtual bool getSkeletonPosition(Vector *vectorArray, float *confidence,  int userID);
    virtual int *getSkeletonState();
    virtual nite::SkeletonState getSkeletonState(int userID);
    virtual ImageOf<PixelRgb> getImageFrame();
    virtual ImageOf<PixelMono16> getDepthFrame();
private:
    BufferedPort<Bottle> *skeletonPort;
    BufferedPort<Bottle> *receivingPort;
    BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono16> > *depthFramePort;
    BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > *imageFramePort;
    OpenNI2SkeletonTracker *skeleton;
    bool withOpenPorts, userTracking, camerasON, mirrorON;
    /**
     * Opens the depth sensor and rgb camera image ports
     */
    void openPorts(string portPrefix, bool withUserTracking, bool withCamerasON);
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
 
 Local OpenNI2 device interface implementation
 
 */
#endif
