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
#include <yarp/dev/IKinectDeviceDriver.h>

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

/**
* Class used by the KinectDeviceDriverServer to interface with the Kinect sensor.
* To compile the OpenNI/NITE frameworks are needed.
*/
class KinectSkeletonTracker
{
public:
	/**
	* Struct with the data from a single user skeleton.
	*/
	typedef struct USER_SKELETON {
		int skeletonState;
		Vector skeletonPointsPos[TOTAL_JOINTS];
		float skeletonPosConfidence[TOTAL_JOINTS];
		Matrix skeletonPointsOri[TOTAL_JOINTS];
		float skeletonOriConfidence[TOTAL_JOINTS];
	}UserSkeleton;
	/**
	* Struct with the data from the RGB camera, the depth camera, and a set of userSkeletons
	*/
	typedef struct KINECT_STATUS {
		ImageOf<yarp::sig::PixelInt> depthMap;
		ImageOf<yarp::sig::PixelRgb> imgMap;
		UserSkeleton userSkeleton[MAX_USERS+1];
	}KinectStatus;
	/**
	* @param userDetection indicates if user callbacks and skeleton tracking should be on
	*/
	KinectSkeletonTracker(bool userDetection = false, bool camerasON = true, bool mirrorON = true);
	~KinectSkeletonTracker(void);
	void close();
	/**
	* Kinect data update function. This function updates the data structs with the latest kinect data.
	*/
	void updateKinect(bool wait);
	/**
	* get the static KinectStatus object
	*/
	static KinectStatus *getKinect();
private:
	static KinectStatus *_kinectStatus;
	bool _userDetection, _camerasON, _mirrorON;
	// OpenNI objects
	Context *_context;
	UserGenerator *_userGenerator;
	DepthGenerator *_depthGenerator;
	ImageGenerator *_imgGenerator;
	int init();
	void initVars();
};
