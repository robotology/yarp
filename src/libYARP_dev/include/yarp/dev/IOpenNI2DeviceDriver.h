// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis, University of Hertfordshire, k.theofilis@herts.ac.uk
 * Authors: Duarte Aragao, Konstantinos Theofilis
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#pragma once

#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>
#include <NiTE.h>

//Constants from the OpenNI2SkeletonData
#define MAX_USERS 10
#define TOTAL_JOINTS 15
#define KINECT_IMG_WIDTH 640
#define KINECT_IMG_HEIGHT 480

/**
 * A full list of available joints.
 */
typedef enum SkeletonJoint
{
    JOINT_HEAD			= 0,
    JOINT_NECK			= 1,
    JOINT_LEFT_SHOULDER = 2,
    JOINT_RIGHT_SHOULDER= 3,
    JOINT_LEFT_ELBOW	= 4,
    JOINT_LEFT_HAND     = 5,
    JOINT_RIGHT_HAND    = 6,
    JOINT_TORSO         = 7,
    JOINT_LEFT_HIP      = 8,
    JOINT_RIGHT_HIP     = 9,
    JOINT_LEFT_KNEE     = 10,
    JOINT_RIGHT_KNEE    = 11,
    JOINT_LEFT_FOOT     = 12,
    JOINT_RIGHT_FOOT    = 13
} SkeletonJoint;

namespace yarp {
    namespace dev {
        class IOpenNI2DeviceDriver;
    }
}
/**
 * @ingroup dev_iface_media
 *
 * Interface for the KinectDeviceDriverLocal and KinectDeviceDiverServer
 */
class yarp::dev::IOpenNI2DeviceDriver {
public:
    /**
     * Gets sensor's skeleton joints orientation quarternion
     *
     * @param matrixArray Array of matrices that is filled with the new orientation matrices
     * @param confidence Array of doubles that is filled with the confidence level of each matrice
     * @param userID id of the user skeleton to be retrieved
     * @return false if the user skeleton is not being tracked
     */
    virtual bool getSkeletonOrientation(yarp::sig::Vector *vectorArray, double *confidence, int userID) = 0;
    /**
     * Gets kinect skeleton joints position 3D vector (in millimeters)
     *
     * @param matrixArray Array of vectors that is filled with the new position vector
     * @param confidence Array of doubles that is filled with the confidence level of each vector
     * @param userID id of the user skeleton to be retrieved
     * @return false if the user skeleton is not being tracked
     */
    virtual bool getSkeletonPosition(yarp::sig::Vector *vectorArray, double *confidence, int userID) = 0;
    /**
     * Gets the user state
     *	0 - NO_USER
     *	1 - USER_LOST
     *	2 - USER_DETECTED
     *	3 - CALIBRATING
     *	4 - SKELETON_TRACKING
     *
     * @return int array of the user skeleton state
     */
    virtual int *getSkeletonState() = 0;
    /**
     * Gets the user skeleton state
     *	0 - NO_USER
     *	1 - USER_LOST
     *	2 - USER_DETECTED
     *	3 - CALIBRATING
     *	4 - SKELETON_TRACKING
     *
     * @param userID id of the user skeleton state to be retrieved
     * @return int value of the user skeleton state
     */
    virtual int getSkeletonState(int userID) = 0;
    /**
     * Get RGB camera image
     *
     * @return camera image
     */
    virtual yarp::sig::ImageOf<yarp::sig::PixelRgb> getImageFrame() = 0;
    /**
     * Get depth camera image (in millimeters)
     *
     * @return camera image
     */
    virtual yarp::sig::ImageOf<yarp::sig::PixelMono16> getDepthFrame() = 0;
};

