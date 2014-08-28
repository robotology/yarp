// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#define PORTNAME_SKELETON "/userSkeleton"
#define PORTNAME_DEPTHMAP "/depthMap"
#define PORTNAME_IMAGEMAP "/imageMap"

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Network.h>
#include <yarp/dev/IKinectDeviceDriver.h>
#include <yarp/sig/all.h>
#include <yarp/os/BufferedPort.h>
#include "KinectSkeletonData.h"


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;

namespace yarp {
	namespace dev {
		class KinectDeviceDriverClient;
	}
}

/**
* @ingroup dev_impl_media
*
* A kinect device implementation to get the kinect data from a kinect conected on another computer running the KinectDeviceDriverServer device.
* This implementation opens 4 ports to connect to yarp device KinectDeviceDriverServer ports:
*	- [localPortPrefix]:o - input port (does nothing)
*	- [localPortPrefix]/userSkeleton:i - userSkeleton detection port (only opened if user detection is on)
*	- [localPortPrefix]/depthMap:i - depth map image port
*	- [localPortPrefix]/imageMap:i - rgb camera image port
*/
class yarp::dev::KinectDeviceDriverClient : /*public GenericYarpDriver,*/ public TypedReaderCallback<ImageOf<PixelRgb> >,
									  public TypedReaderCallback<ImageOf<PixelInt> >, public TypedReaderCallback<Bottle>, public yarp::dev::IKinectDeviceDriver,
									  public yarp::dev::DeviceDriver {
public:
	KinectDeviceDriverClient();
	//DeviceDriver
	virtual bool open(yarp::os::Searchable& config);
	virtual bool close();
	//TypedReaderCallback
	virtual void onRead(Bottle& b);
	virtual void onRead(ImageOf<PixelRgb>& img);
	virtual void onRead(ImageOf<PixelInt>& img);
	//IKinectDeviceDriver
	virtual bool getSkeletonOrientation(Matrix *matrixArray, double *confidence,  int userID);
	virtual bool getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID);
	virtual int *getSkeletonState();
	virtual int getSkeletonState(int userID);
	virtual ImageOf<PixelRgb> getImageMap();
	virtual ImageOf<PixelInt> getDepthMap();
private:
	BufferedPort<Bottle> *_outPort;
	BufferedPort<Bottle> *_inUserSkeletonPort;
	BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> > *_inDepthMapPort;
	BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > *_inImageMapPort;
	//PortCtrlMod *_portMod;
	KinectSkeletonData *_skeletonData;
	/**
	* Connect the KinectDeviceDriverClient ports with the KinectDeviceDriverServer ports.
	*
	* @param remotePortPrefix remote ports prefix
	* @param localPortPrefix local ports prefix
	* @return true/false on sucess/failure to connect
	*/
	bool connectPorts(string remotePortPrefix, string localPortPrefix);
};

/**
* @ingroup dev_runtime
* \defgroup cmd_device_kinect_device_client kinect_device_client

Client Kinect device interface implementation

*/