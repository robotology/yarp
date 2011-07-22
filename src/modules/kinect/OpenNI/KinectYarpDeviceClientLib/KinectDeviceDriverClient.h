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
#include "IKinectDeviceDriver.h"
#include "KinectSkeletonData.h"
#include "../lib/GenericYarpDriver.h"
#include "../lib/PortCtrlMod.h"
#include <yarp/sig/all.h>


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;

namespace yarp {
	namespace dev {
		class KinectDeviceDriverClient;
	}
}

class yarp::dev::KinectDeviceDriverClient : public GenericYarpDriver, public TypedReaderCallback<ImageOf<PixelRgb>>, 
									  public TypedReaderCallback<ImageOf<PixelInt>>, public yarp::dev::IKinectDeviceDriver, 
									  public yarp::dev::DeviceDriver {
public:
	KinectDeviceDriverClient();
	//GenericYarp
	bool open(yarp::os::Searchable& config);
	bool interruptModule();
	bool close();
	bool updateInterface();
	bool shellRespond(const Bottle& command, Bottle& reply);
	void onRead(Bottle& b);
	void onRead(ImageOf<PixelRgb>& img);
	void onRead(ImageOf<PixelInt>& img);
	//IKinectDeviceDriver
	bool getSkeletonOrientation(Matrix *matrixArray, double *confidence,  int userID);
	bool getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID);
	int getSkeletonState(int userID);
	ImageOf<PixelRgb> getImageMap();
	ImageOf<PixelInt> getDepthMap();
private:
	BufferedPort<Bottle> *_outPort;
	BufferedPort<Bottle> *_inUserSkeletonPort;
	BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt>> *_inDepthMapPort;
	BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> *_inImageMapPort;
	bool connectPorts(string remotePortPrefix, string localPortPrefix);
	PortCtrlMod *_portMod;
	KinectSkeletonData *_skeletonData;
};