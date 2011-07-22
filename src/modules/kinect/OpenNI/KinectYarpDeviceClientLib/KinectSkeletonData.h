// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>
#include <yarp/math/Math.h>
#include <yarp/sig/Image.h>
#include <string>
#include <iostream>

//included so that the enumeration can be accessed
#include "IKinectDeviceDriver.h"

#define USER_DETECTED_MSG "[USER_DETECTED]"
#define USER_CALIBRATING_MSG "[USER_CALIBRATING]"
#define USER_LOST_MSG "[USER_LOST]"
#define USER_VOCAB VOCAB4('U','S','E','R')
#define POSITION_VOCAB VOCAB3('P','O','S')
#define ORIENTATION_VOCAB VOCAB3('O','R','I')
#define DMAP_VOCAB VOCAB4('D','M','A','P')

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

#define MAX_USERS 10
#define TOTAL_JOINTS 24

class KinectSkeletonData
{
public:
	KinectSkeletonData(void);
	~KinectSkeletonData(void);
	void storeData(Bottle& b);
	void storeData(ImageOf<PixelRgb>& img);
	void storeData(ImageOf<PixelInt>& img);
	Matrix* getOrientation(int userID);
	Vector* getPosition(int userID);
	double* getOrientationConf(int userID);
	double* getPositionConf(int userID);
	int getSkeletonState(int userID);
	ImageOf<PixelInt> getDepthMap();
	ImageOf<PixelRgb> getImageMap();
private:
	typedef struct USER_SKELETON {
		SKELETON_STATE skeletonState;
		Vector skeletonPointsPos[TOTAL_JOINTS];
		Matrix skeletonPointsOri[TOTAL_JOINTS];
		double skeletonPosConf[TOTAL_JOINTS];
		double skeletonOriConf[TOTAL_JOINTS];
	}UserSkeleton;
	UserSkeleton *_userSkeleton;
	ImageOf<PixelInt> _depthMap;
	ImageOf<PixelRgb> _imageMap;
	void initUserSkeletons();
};
