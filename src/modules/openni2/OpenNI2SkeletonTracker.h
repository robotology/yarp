// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis, University of Hertfordshire, k.theofilis@herts.ac.uk
 * Authors: Duarte Aragao, Konstantinos Theofilis
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef OPENNI2_SKELETON_TRACKER_H
#define OPENNI2_SKELETON_TRACKER_H

#include <string>
#include <time.h>
#include <iostream>

#include <OpenNI.h>
#include <NiTE.h>

#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>

//included so that the enumeration can be accessed
#include <yarp/dev/IOpenNI2DeviceDriver.h>

using namespace std;

using namespace yarp::sig;

#define MAX_USERS 10
#define TOTAL_JOINTS 15

/**
 * Class used by the OpenNI2DeviceDriverServer to interface with the sensor.
 * To compile the OpenNI/NITE frameworks are needed.
 */
class OpenNI2SkeletonTracker
{
public:
    /**
     * Struct with the data from a single user skeleton.
     */
    typedef struct USER_SKELETON {
        nite::SkeletonState skeletonState;
        Vector skeletonPointsPos[TOTAL_JOINTS];
        float skeletonPosConfidence[TOTAL_JOINTS];
        Vector skeletonPointsOri[TOTAL_JOINTS];
        float skeletonOriConfidence[TOTAL_JOINTS];
        bool visible;
        //bool stillTracking;
        int uID;
    }UserSkeleton;
    /**
     * Struct with the data from the RGB camera, the depth camera, and a set of userSkeletons
     */
    typedef struct SENSOR_STATUS {
        ImageOf<yarp::sig::PixelMono16> depthFrame;
        ImageOf<yarp::sig::PixelRgb> imageFrame;
        UserSkeleton userSkeleton[MAX_USERS];
    }SensorStatus;
    /**
     * @param userDetection indicates if user callbacks and skeleton tracking should be on
     */
    OpenNI2SkeletonTracker(bool withTracking = false, bool camerasON = true, bool mirrorON = true);
    ~OpenNI2SkeletonTracker(void);
    void close();
    /**
     * Sensor data update function. This function updates the data structs with the latest sensor data.
     */
    void updateSensor(bool wait);
    void updateUserState(const nite::UserData& user, unsigned long long ts);
    void updateJointInformation(const nite::UserData& user, nite::JointType joint, int jIndex);
    
    /**
     * get the static SensorStatus object
     */
    static SensorStatus *getSensor();
private:
    static SensorStatus *sensorStatus;
    bool userTracking, camerasON, mirrorON;
    
    // OpenNI2 and NiTE objects
    openni::Device device;
    nite::UserTracker userTracker;
    openni::VideoStream depthStream;
    openni::VideoStream imageStream;
    openni::VideoMode depthMode;
    openni::VideoMode imageMode;
    openni::VideoFrameRef depthFrameRef;
    openni::VideoFrameRef imageFrameRef;
    nite::UserTrackerFrameRef userTrackerFrameRef;
    int init();
    void initVars();
};
#endif