// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis, University of Hertfordshire, k.theofilis@herts.ac.uk
 * Authors: Duarte Aragao, Konstantinos Theofilis
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef OPENNI2_SKELETON_DATA_H
#define OPENNI2_SKELETON_DATA_H

#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>
#include <yarp/math/Math.h>
#include <yarp/sig/Image.h>
#include <string>
#include <iostream>
#include "NiTE.h"

//included so that the enumeration can be accessed
#include <yarp/dev/IOpenNI2DeviceDriver.h>

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
#define TOTAL_JOINTS 15

class OpenNI2SkeletonData
{
public:
    OpenNI2SkeletonData();
    ~OpenNI2SkeletonData();
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
    void storeData(ImageOf<PixelMono16>& img);
    /**
     * Get a users orientation matrices array
     *
     * @param userID  detected user id
     * @return matrices array with the orientation matrices (3x3)
     */
    Vector* getOrientation(int userID);
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
     * @return doubles array with confidence values per each orientation quartenion
     */
    float* getOrientationConf(int userID);
    /**
     * Get a users position confidence array
     *
     * @param userID  detected user id
     * @return doubles array with confidence values per each 3D position vector
     */
    float* getPositionConf(int userID);
    /**
     * Get a single user status
     *
     * @param userID  detected user id
     * @return int of status
     */
    int getSkeletonState(int userID);
    /**
     * Get depth frame
     *
     * @return image with the depth values in (millimeters)
     */
    ImageOf<PixelMono16> getDepthFrame();
    /**
     * Get rgb frame
     *
     * @return image with the rgb values
     */
    ImageOf<PixelRgb> getImageFrame();
private:
    typedef struct USER_SKELETON {
        nite::SkeletonState skeletonState;
        Vector skeletonPointsPos[TOTAL_JOINTS];
        Vector skeletonPointsOri[TOTAL_JOINTS];
        float skeletonPosConf[TOTAL_JOINTS];
        float skeletonOriConf[TOTAL_JOINTS];
        bool visible;
        int uID;
    }UserSkeleton;
    UserSkeleton *userSkeleton;
    ImageOf<PixelMono16> depthFrame;
    ImageOf<PixelRgb> imageFrame;
    void initUserSkeletons();
};
#endif
