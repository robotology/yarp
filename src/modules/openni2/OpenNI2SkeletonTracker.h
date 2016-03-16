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
#ifdef OPENNI2_DRIVER_USES_NITE2
    #include <NiTE.h>
#endif

#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>

//included so that the enumeration can be accessed
#include <yarp/dev/IOpenNI2DeviceDriver.h>

using namespace std;

using namespace yarp::sig;

#define MAX_USERS 10
#define TOTAL_JOINTS 15
#define MINIMUM_CONFIDENCE 0.6
#define DEFAULT_COLOR_MODE 0
#define DEFAULT_DEPTH_MODE 0

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
    typedef struct USER_SKELETON
    {
#ifdef OPENNI2_DRIVER_USES_NITE2
        nite::SkeletonState skeletonState;
#endif
        Vector skeletonPointsPos[TOTAL_JOINTS];
        float skeletonPosConfidence[TOTAL_JOINTS];
        Vector skeletonPointsOri[TOTAL_JOINTS];
        float skeletonOriConfidence[TOTAL_JOINTS];
        bool visible;
        bool stillTracking;
        int uID;
    }UserSkeleton;

    int getDeviceStatus();
     
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
    OpenNI2SkeletonTracker(bool withTracking = false, bool withColorON = true, bool witRgbMirrorON = false, bool withDepthMirrorON = false, double minConf = MINIMUM_CONFIDENCE, bool withOniPlayback = false, string withFileDevice = "", bool withOniRecord  = false, string withOniOutputFile = "", bool withLoop = false, bool withFrameSync = false, bool withImageRegistration = false, bool prMode = false, int depthMode = DEFAULT_DEPTH_MODE, int colorMode = DEFAULT_COLOR_MODE);
    ~OpenNI2SkeletonTracker(void);
    void close();
    /**
     * Sensor data update function. This function updates the data structs with the latest sensor data.
     */
    void updateSensor();
#ifdef OPENNI2_DRIVER_USES_NITE2
    void updateUserState(const nite::UserData& user, unsigned long long ts);
    void updateJointInformation(const nite::UserData& user, nite::JointType joint, int jIndex);
#endif
    /**
     * get the static SensorStatus object
     */
    static SensorStatus *getSensor();
private:
    static SensorStatus *sensorStatus;
    bool userTracking, colorON, rgbMirrorON, depthMirrorON, oniPlayback, oniRecord, loop, frameSync, imageRegistration, printMode;
    int deviceStatus;
    double minConfidence;
    string fileDevice;
    string oniOutputFile;
    int frameCount;
    int fpsCount;
    int depthVideoMode;
    int colorVideoMode;
    string deviceName;

    // OpenNI2 objects
    openni::Device device;
    openni::Device* pDevice;
    openni::Recorder recorder;
    openni::PlaybackControl* playbackControl;
    openni::VideoStream depthStream;
    openni::VideoStream imageStream;
    openni::VideoMode depthMode;
    openni::VideoMode imageMode;
    openni::VideoFrameRef depthFrameRef;
    openni::VideoFrameRef imageFrameRef;
    const openni::SensorInfo* depthInfo;
    const openni::SensorInfo* colorInfo;

    //NiTE objects
#ifdef OPENNI2_DRIVER_USES_NITE2
    nite::UserTrackerFrameRef userTrackerFrameRef;
    nite::UserTracker userTracker;
#endif
    int init();
    void initVars();
};
#endif
