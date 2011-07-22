// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#include <iostream>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/all.h>

#include "../lib/GenericYarpDriver.h"
//#include "KinectSkeletonTracker.h"
class KinectSkeletonTracker;

#include "../lib/PortCtrlMod.h"

//The interface is taken from the client
#include "IKinectDeviceDriver.h"

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

using namespace yarp::sig;

namespace yarp {
	namespace dev {
		class KinectDeviceDriverServer;
	}
}

class yarp::dev::KinectDeviceDriverServer: public GenericYarpDriver, public yarp::dev::IKinectDeviceDriver, 
	public yarp::dev::DeviceDriver{
public:
	KinectDeviceDriverServer(bool openPorts = false, bool userDetection = false);
	KinectDeviceDriverServer(string portPrefix, bool userDetection = false);
	~KinectDeviceDriverServer(void);
	bool open(yarp::os::Searchable& config);
	void onRead(Bottle &bot);
	bool updateInterface();
	bool shellRespond(const Bottle& command, Bottle& reply);
	bool close();
	//IKinectDeviceDriver
	bool getSkeletonOrientation(Matrix *matrixArray, double *confidence,  int userID);
	bool getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID);
	int getSkeletonState(int userID);
	ImageOf<PixelRgb> getImageMap();
	ImageOf<PixelInt> getDepthMap();
private:
	KinectSkeletonTracker *_skeleton;
	BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt>> *_depthMapPort;
	BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> *_imgMapPort;
	bool _openPorts, _userDetection;
	void openPorts(string portPrefix);
	void sendKinectData(BufferedPort<Bottle> *mainBottle);
};