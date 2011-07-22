// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>

//Constants from the KinectSkeletonData
#define MAX_USERS 10
#define TOTAL_JOINTS 24
#define KINECT_IMG_WIDTH 640
#define KINECT_IMG_HEIGHT 480

typedef enum
{
	NO_USER,
	USER_LOST,
	USER_DETECTED,
	CALIBRATING,
	SKELETON_TRACKING
} SKELETON_STATE;

namespace yarp {
	namespace dev {
		class IKinectDeviceDriver;
	}
}

class yarp::dev::IKinectDeviceDriver {
public:
	virtual bool getSkeletonOrientation(yarp::sig::Matrix *matrixArray, double *confidence, int userID) = 0;//returns false if the user skeleton is not being tracked
	virtual bool getSkeletonPosition(yarp::sig::Vector *vectorArray, double *confidence, int userID) = 0;//returns false if the user skeleton is not being tracked
	virtual int getSkeletonState(int userID) = 0;
	virtual yarp::sig::ImageOf<yarp::sig::PixelRgb> getImageMap() = 0;
	virtual yarp::sig::ImageOf<yarp::sig::PixelInt> getDepthMap() = 0;
};