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

/**
* A full list of available joints.
*/
typedef enum SkeletonJoint
{
    SKEL_HEAD            = 0,
    SKEL_NECK            = 1,
    SKEL_TORSO           = 2,
    SKEL_WAIST           = 3,

    SKEL_LEFT_COLLAR     = 4,
    SKEL_LEFT_SHOULDER   = 5,
    SKEL_LEFT_ELBOW      = 6,
    SKEL_LEFT_WRIST      = 7,
    SKEL_LEFT_HAND       = 8,
    SKEL_LEFT_FINGERTIP  = 9,

    SKEL_RIGHT_COLLAR    =10,
    SKEL_RIGHT_SHOULDER  =11,
    SKEL_RIGHT_ELBOW     =12,
    SKEL_RIGHT_WRIST     =13,
    SKEL_RIGHT_HAND      =14,
    SKEL_RIGHT_FINGERTIP =15,

    SKEL_LEFT_HIP        =16,
    SKEL_LEFT_KNEE       =17,
    SKEL_LEFT_ANKLE      =18,
    SKEL_LEFT_FOOT       =19,

    SKEL_RIGHT_HIP       =20,
    SKEL_RIGHT_KNEE      =21,
    SKEL_RIGHT_ANKLE     =22,
    SKEL_RIGHT_FOOT      =23
} SkeletonJoint;

namespace yarp {
    namespace dev {
        class IKinectDeviceDriver;
    }
}
/**
* @ingroup dev_iface_media
*
* Interface for the KinectDeviceDriverLocal and KinectDeviceDiverServer
*/
class yarp::dev::IKinectDeviceDriver {
public:
    /*!
     * Destructor.
     */
    virtual ~IKinectDeviceDriver() {}

    /**
    * Gets kinect skeleton joints orientation 3x3 matrix
    *
    * @param matrixArray Array of matrices that is filled with the new orientation matrices
    * @param confidence Array of doubles that is filled with the confidence level of each matrice
    * @param userID id of the user skeleton to be retrieved
    * @return false if the user skeleton is not being tracked
    */
    virtual bool getSkeletonOrientation(yarp::sig::Matrix *matrixArray, double *confidence, int userID) = 0;
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
    *    0 - NO_USER
    *    1 - USER_LOST
    *    2 - USER_DETECTED
    *    3 - CALIBRATING
    *    4 - SKELETON_TRACKING
    *
    * @return int array of the user skeleton state
    */
    virtual int *getSkeletonState() = 0;
    /**
    * Gets the user skeleton state
    *    0 - NO_USER
    *    1 - USER_LOST
    *    2 - USER_DETECTED
    *    3 - CALIBRATING
    *    4 - SKELETON_TRACKING
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
    virtual yarp::sig::ImageOf<yarp::sig::PixelRgb> getImageMap() = 0;
    /**
    * Get depth camera image (in millimeters)
    *
    * @return camera image
    */
    virtual yarp::sig::ImageOf<yarp::sig::PixelInt> getDepthMap() = 0;
};
