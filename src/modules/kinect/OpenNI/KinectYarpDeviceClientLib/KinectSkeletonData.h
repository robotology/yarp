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
#include <yarp/dev/IKinectDeviceDriver.h>

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
	/**
	* This should be called each time the user skeleton data is updated
	*
	* @param b Bottle with the user/skeleton data
	*/
	void storeData(Bottle& b);
	/**
	* This should be called each time the kinect rgb camera data is updated
	*
	* @param img rgb camera image
	*/
	void storeData(ImageOf<PixelRgb>& img);
	/**
	* This should be called each time the kinect depth camera data is updated
	*
	* @param img depth camera image (in millimeters)
	*/
	void storeData(ImageOf<PixelInt>& img);
	/**
	* Get a users orientation matrices array
	*
	* @param userID  detected user id
	* @return matrices array with the orientation matrices (3x3)
	*/
	Matrix* getOrientation(int userID);
	/**
	* Get a users position vector array
	*
	* @param userID  detected user id
	* @return vectors array with the 3D position (in millimeters)
	*/
	Vector* getPosition(int userID);
	/**
	* Get a users orientation confidence array
	*
	* @param userID  detected user id
	* @return doubles array with confidence values per each orientation matrix
	*/
	double* getOrientationConf(int userID);
	/**
	* Get a users position confidence array
	*
	* @param userID  detected user id
	* @return doubles array with confidence values per each 3D position vector
	*/
	double* getPositionConf(int userID);
	/**
	* Get a single user status
	*
	* @param userID  detected user id
	* @return int of status
	*/
	int getSkeletonState(int userID);
	/**
	* Get depth map image
	*
	* @return image with the depth values in (millimeters)
	*/
	ImageOf<PixelInt> getDepthMap();
	/**
	* Get rgb map image
	*
	* @return image with the rgb values
	*/
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
