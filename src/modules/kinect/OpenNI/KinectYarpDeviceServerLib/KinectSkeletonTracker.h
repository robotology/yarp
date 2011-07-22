// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#include <string>
#include <time.h>
#include <iostream>

// Headers for OpenNI
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnHash.h>
#include <XnLog.h>

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Image.h>

//included so that the enumeration can be accessed
#include "IKinectDeviceDriver.h"

#define KINECT_VENDOR "PrimeSense"
#define KINECT_LICKEY "0KOIk2JeIBYClPWVnMoRKn5cdY4="

using namespace std;
using namespace xn;

using namespace yarp::sig;

void XN_CALLBACK_TYPE UserCreated(UserGenerator& generator, XnUserID userID, void* pCookie);
void XN_CALLBACK_TYPE UserDestroyed(UserGenerator& generator, XnUserID userID, void* pCookie);
void XN_CALLBACK_TYPE CalibrationStart(SkeletonCapability &skeleton, XnUserID userID, void *pCookie);
void XN_CALLBACK_TYPE CalibrationEnd(SkeletonCapability &skeleton, XnUserID userID, XnBool bSuccess, void *pCookie);
void XN_CALLBACK_TYPE PoseDetected(PoseDetectionCapability &poseDetection, const XnChar* strPose, XnUserID userID, void *pCookie);

#define MAX_USERS 10
#define TOTAL_JOINTS 24

class KinectSkeletonTracker
{
public:
	typedef struct USER_SKELETON {
		int skeletonState;
		Vector skeletonPointsPos[TOTAL_JOINTS];
		float skeletonPosConfidence[TOTAL_JOINTS];
		Matrix skeletonPointsOri[TOTAL_JOINTS];
		float skeletonOriConfidence[TOTAL_JOINTS];
	}UserSkeleton;
	typedef struct KINECT_STATUS {
		ImageOf<yarp::sig::PixelInt> depthMap;
		ImageOf<yarp::sig::PixelRgb> imgMap;
		UserSkeleton userSkeleton[MAX_USERS+1];
	}KinectStatus;
	KinectSkeletonTracker(bool userDetection = false);
	~KinectSkeletonTracker(void);
	void close();
	void updateKinect();
	static KinectStatus *getKinect();
private:
	static KinectStatus *_kinectStatus;
	bool _userDetection;
	// OpenNI objects
	Context *_context;
	UserGenerator *_userGenerator;
	DepthGenerator *_depthGenerator;
	ImageGenerator *_imgGenerator;
	int init();
	void initVars();
};
