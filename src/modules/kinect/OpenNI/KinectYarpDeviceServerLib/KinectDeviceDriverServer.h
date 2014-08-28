// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#include <iostream>
#include <string>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/all.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/os/all.h>
#include <yarp/os/Stamp.h>

//this include could not be done here do to yarp compilation error :)
//#include "KinectSkeletonTracker.h"
class KinectSkeletonTracker;

//The interface is taken from the client
#include <yarp/dev/IKinectDeviceDriver.h>

#define PORTNAME_SKELETON "/userSkeleton"
#define PORTNAME_DEPTHMAP "/depthMap"
#define PORTNAME_IMAGEMAP "/imageMap"

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
		class KinectDeviceDriverServer;
	}
}

/**
* @ingroup dev_impl_media
*
* A kinect device implementation to get the kinect data from a kinect conected locally.
* This implementation opens 4 ports:
*	- [portPrefix]:i - input port (does nothing)
*	- [portPrefix]/userSkeleton:o - userSkeleton detection port (only opened if user detection is on)
*	- [portPrefix]/depthMap:o - depth map image port
*	- [portPrefix]/imageMap:o - rgb camera image port
*/
class yarp::dev::KinectDeviceDriverServer: public IService, public yarp::dev::IKinectDeviceDriver,
	public yarp::dev::DeviceDriver{
public:
	//KinectDeviceDriverServer(bool openPorts = false, bool userDetection = false);
	KinectDeviceDriverServer(void);
	~KinectDeviceDriverServer(void);
	//GenericYarpDriver
	virtual bool updateInterface(bool wait);
	//DeviceDriver
	virtual bool open(yarp::os::Searchable& config);
	virtual bool close();
	//IService
	virtual bool startService();
	virtual bool updateService();
	virtual bool stopService();
	//IKinectDeviceDriver
	virtual bool getSkeletonOrientation(Matrix *matrixArray, double *confidence,  int userID);
	virtual bool getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID);
	virtual int *getSkeletonState();
	virtual int getSkeletonState(int userID);
	virtual ImageOf<PixelRgb> getImageMap();
	virtual ImageOf<PixelInt> getDepthMap();
private:
	BufferedPort<Bottle> *_skeletonPort;
	BufferedPort<Bottle> *_receivingPort;
	BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> > *_depthMapPort;
	BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > *_imgMapPort;
	KinectSkeletonTracker *_skeleton;
	bool _openPorts, _userDetection, _camerasON, _mirrorON;
	/**
	* Opens the depth map a rgb camera image ports
	*/
	void openPorts(string portPrefix, bool userDetection, bool camerasON);
	/**
	* Sends the kinect data to the rgb and depth map ports. Also sends the userSkeleton data to the mainBottle port.
	*
	* @param mainBottle BufferedPort for the userSkeleton data (only needed if user detection is on)
	*/
	void sendKinectData();
};

/**
* @ingroup dev_runtime
* \defgroup cmd_device_kinect_device_server kinect_device_server

Local Kinect device interface implementation

*/
