// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis, University of Hertfordshire, k.theofilis@herts.ac.uk
 * Authors: Duarte Aragao, Konstantinos Theofilis
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "OpenNI2SkeletonTracker.h"

#define USER_MESSAGE(msg) \
{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

OpenNI2SkeletonTracker::SensorStatus *OpenNI2SkeletonTracker::sensorStatus;

OpenNI2SkeletonTracker::OpenNI2SkeletonTracker(bool withTracking, bool withCamerasOn, bool withMirrorOn, double minConf, bool withOniPlayback, string withFileDevice, bool withOniRecord, string withOniOutputFile, bool withLoop, bool withFrameSync, bool withImageRegistration)
{
    userTracking= withTracking;
    camerasON = withCamerasOn;
    mirrorON = withMirrorOn;

    if (minConf != MINIMUM_CONFIDENCE){
        minConfidence = minConf;
    }

    else{
        minConfidence = MINIMUM_CONFIDENCE;
    }

    oniPlayback = withOniPlayback;
    oniRecord = withOniRecord;
    if (oniPlayback) {
        fileDevice = withFileDevice;
    }
    
    if (oniRecord) {
         oniOutputFile = withOniOutputFile;
    }
    
    loop = withLoop;
    frameSync = withFrameSync;
    imageRegistration = withImageRegistration;
    init();
    initVars();
}

OpenNI2SkeletonTracker::~OpenNI2SkeletonTracker(void)
{
}

void OpenNI2SkeletonTracker::close(){
   
    if (oniRecord) {
        recorder.stop();
        cout << "Stopping recorder device...";
        recorder.destroy(); 
        cout << "Done" << endl;
    }
    if (userTracking) {
        cout << "Destroying user tracker...";
        for (int i=0; i < MAX_USERS; i++) {
           userTracker.stopSkeletonTracking(i+1);
        }
        userTracker.destroy();
        nite::NiTE::shutdown();
        cout << "Done" << endl;
    }
    
    if (camerasON) {
        depthStream.stop();
        imageStream.stop();
        cout << "Destroying depth stream...";
        depthStream.destroy();
        cout << "Done" << endl;
        cout << "Destroying RGB stream...";
        imageStream.destroy();
        cout << "Done" << endl;
    }
    
    delete getSensor();
    cout << "Closing sensor device...";
    device.close();
    openni::OpenNI::shutdown();
    cout << "Done" << endl;
}

int OpenNI2SkeletonTracker::init(){
    openni::Status rc = openni::OpenNI::initialize();
    deviceStatus = 0;

    if (rc != openni::STATUS_OK)
    {
        printf("Initialize failed\n%s\n", openni::OpenNI::getExtendedError());
        deviceStatus = rc;
        return rc;
    }

    if (!oniPlayback){
        rc = device.open(openni::ANY_DEVICE);
    }
    else {
        rc = device.open(fileDevice.c_str());
        cout << "Playback from " << fileDevice.c_str() << endl;
        playbackControl = device.getPlaybackControl();
        playbackControl->setRepeatEnabled(loop);
    }

    if (rc != openni::STATUS_OK) {
        printf("Couldn't open device\n%s\n", openni::OpenNI::getExtendedError());
        deviceStatus = rc;
        return rc;
    }

    cout << "OpenNI v" << openni::OpenNI::getVersion().openni::Version::major << "." << openni::OpenNI::getVersion().openni::Version::minor << "." << openni::OpenNI::getVersion().openni::Version::maintenance << "." << openni::OpenNI::getVersion().openni::Version::build <<endl;
    
     
    cout << "NiTE v" << nite::NiTE::getVersion().nite::Version::major << "." << nite::NiTE::getVersion().nite::Version::minor << "." << nite::NiTE::getVersion().nite::Version::maintenance << "." << nite::NiTE::getVersion().nite::Version::build << endl;
    if (oniRecord) {
        recorder.create(oniOutputFile.c_str());
    }

    if(camerasON){
	
	bool modeIsSupported = false;
	modeIsSupported = device.isImageRegistrationModeSupported(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	if (modeIsSupported){
	    cout << "Image registration mode is supported" << endl;
	    
	    if (imageRegistration){
		device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
		bool modeStatus = device.getImageRegistrationMode();
		if (modeStatus=0){
		    cout << "Image registration mode is off" << endl;
		}
		else{
		    cout << "Image registration mode is on" << endl;
		}
	    }
	}
	else{
	    cout << "Image registration mode is not supported" << endl;
	}
	  
        // setup and start depth stream
        if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
        {
            
            rc = depthStream.create(device, openni::SENSOR_DEPTH);
            
            if (rc != openni::STATUS_OK)
            {
                printf("Couldn't create depth stream\n%s\n", openni::OpenNI::getExtendedError());
                deviceStatus = rc;
                return rc;
            }
        }
        if (oniRecord) {
            recorder.attach(depthStream);
        }

        rc = depthStream.start();
        
        if (rc != openni::STATUS_OK)
        {
            printf("Couldn't start the depth stream\n%s\n", openni::OpenNI::getExtendedError());
            deviceStatus = rc;
            return rc;
        }
        
        else {
	    fpsCount = depthStream.getVideoMode().getFps();
            cout << "Resolution: " << depthStream.getVideoMode().getResolutionX() << "x" << depthStream.getVideoMode().getResolutionY() << " - " << fpsCount << " fps" << endl;
            cout << "Depth stream started..." << endl;
            frameCount = playbackControl->getNumberOfFrames(depthStream);
        }
            
        // setup and start colour stream
        if (device.getSensorInfo(openni::SENSOR_COLOR) != NULL)
        {
            rc = imageStream.create(device, openni::SENSOR_COLOR);
            if (rc != openni::STATUS_OK)
            {
                printf("Couldn't create RGB stream\n%s\n", openni::OpenNI::getExtendedError());
                deviceStatus = rc;
                return rc;
            }
        }
        
        if (oniRecord) {
            recorder.attach(imageStream);
        }

        rc = imageStream.start();
        if (rc != openni::STATUS_OK)
        {
            printf("Couldn't start the RGB stream\n%s\n", openni::OpenNI::getExtendedError());
            deviceStatus = rc;
            return rc;
        }
        
        else {
            cout << "RGB stream started..." << endl;
        }
    }

    if (userTracking){
        
        // setup and start user tracking

        nite::Status niteRc = nite::NiTE::initialize();
        niteRc = userTracker.create(&device);

        if (niteRc != nite::STATUS_OK)
        {
            printf("Couldn't create user tracker\n");
            return niteRc;
        }
        
        else {
            cout << "User tracker started..." << endl;
        }
        
        printf("\nStart moving around to get detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");
    }
   
    if (oniRecord) {
        rc = recorder.start();

        if (rc != openni::STATUS_OK) {
            printf("Couldn't start recorder\n");
        }

        else {
            cout << "Recorder started..." << endl;
        }
    }

    return rc;
}

void OpenNI2SkeletonTracker::initVars(){
    sensorStatus = new SensorStatus;
   
    if (camerasON) {
    // read frames from streams
    imageStream.readFrame(&imageFrameRef);
    depthStream.readFrame(&depthFrameRef);
    
    // get depth mode properties and prepare depthFrame
    depthMode = depthStream.getVideoMode();
    sensorStatus->depthFrame.resize(depthMode.getResolutionX(), depthMode.getResolutionY());
    sensorStatus->depthFrame.zero();
    
    // get RGB mode properties and prepare imageFrame
    imageMode = imageStream.getVideoMode();
    sensorStatus->imageFrame.resize(imageMode.getResolutionX(), imageMode.getResolutionY());
    sensorStatus->imageFrame.zero();
    }
    
    // initialise UserSkeleton struct
    for (int i = 0; i < MAX_USERS; i++) {
        sensorStatus->userSkeleton[i].skeletonState = nite::SKELETON_NONE;
        sensorStatus->userSkeleton[i].uID = i+1;
        sensorStatus->userSkeleton[i].visible = false;
        sensorStatus->userSkeleton[i].stillTracking = false;
        for (int jointIndex = 0; jointIndex < TOTAL_JOINTS; jointIndex++){
            sensorStatus->userSkeleton[i].skeletonPointsPos[jointIndex].resize(3);
            sensorStatus->userSkeleton[i].skeletonPointsPos[jointIndex].zero();
            sensorStatus->userSkeleton[i].skeletonPointsOri[jointIndex].resize(4);
            sensorStatus->userSkeleton[i].skeletonPointsOri[jointIndex].zero();
        }
    }
}

// returns the sensor data struct (where all the data is)
OpenNI2SkeletonTracker::SensorStatus *OpenNI2SkeletonTracker::getSensor(){
    return sensorStatus;
}

void OpenNI2SkeletonTracker::updateSensor(){
    // get camera image
    if(camerasON && imageStream.isValid()){
        imageStream.readFrame(&imageFrameRef);
        
        if (imageFrameRef.isValid()){
            getSensor()->imageFrame.setQuantum(1);
            
            // put image in Yarp format
            void* tmpImage = (void*)imageFrameRef.getData();
            getSensor()->imageFrame.setExternal(tmpImage, imageMode.getResolutionX(), imageMode.getResolutionY());
        }
    }
    
    // Get depth image (in millimetres)
    if(camerasON && depthStream.isValid()){
        depthStream.readFrame(&depthFrameRef);
       
        if (depthFrameRef.getFrameIndex() == frameCount) {
            cout << "Finished playback of " << fileDevice.c_str() << endl;
            cout << "Duration: " << ((double)frameCount/fpsCount) << " sec (" << frameCount << " frames at " << fpsCount << "fps)" << endl;
        }
        if (depthFrameRef.isValid()){
            getSensor()->depthFrame.setQuantum(1);
            
            // put image in Yarp format
            void* tmpDepth = (void*)depthFrameRef.getData();
            getSensor()->depthFrame.setExternal(tmpDepth, depthMode.getResolutionX(), depthMode.getResolutionY());
        }
    }
    // user skeleton tracking data
   
    if(userTracking && userTracker.isValid()){
        
        // reset the stillTracking variable
        for (int i = 0; i < MAX_USERS; i++) {
            sensorStatus->userSkeleton[i].stillTracking = false;
        }
        
        nite::Status niteRc = userTracker.readFrame(&userTrackerFrameRef);
        if (niteRc != nite::STATUS_OK)
        {
            printf("Get next frame failed\n");
        }
        
        // get an array of the user data
        const nite::Array<nite::UserData>& users = userTrackerFrameRef.getUsers();
        for (int i = 0; i < users.getSize(); i++) {
            const nite::UserData& user = users[i];
            
            
            // update user states
            updateUserState(user, userTrackerFrameRef.getTimestamp());
            
            // if user is new, start skeleton tracking
            if (user.isNew())
            {
                userTracker.startSkeletonTracking(user.getId());
            }
            
            if (user.getSkeleton().getState() == nite::SKELETON_TRACKED){
                updateJointInformation(user, nite::JOINT_HEAD, 0);
                updateJointInformation(user, nite::JOINT_NECK, 1);
                updateJointInformation(user, nite::JOINT_LEFT_SHOULDER, 2);
                updateJointInformation(user, nite::JOINT_RIGHT_SHOULDER, 3);
                updateJointInformation(user, nite::JOINT_LEFT_ELBOW, 4);
                updateJointInformation(user, nite::JOINT_RIGHT_ELBOW, 5);
                updateJointInformation(user, nite::JOINT_LEFT_HAND, 6);
                updateJointInformation(user, nite::JOINT_RIGHT_HAND, 7);
                updateJointInformation(user, nite::JOINT_TORSO, 8);
                updateJointInformation(user, nite::JOINT_LEFT_HIP, 9);
                updateJointInformation(user, nite::JOINT_RIGHT_HIP, 10);
                updateJointInformation(user, nite::JOINT_LEFT_KNEE, 11);
                updateJointInformation(user, nite::JOINT_RIGHT_KNEE, 12);
                updateJointInformation(user, nite::JOINT_LEFT_FOOT, 13);
                updateJointInformation(user, nite::JOINT_RIGHT_FOOT, 14);
            }
        }
    }
}

void OpenNI2SkeletonTracker::updateJointInformation(const nite::UserData& user, nite::JointType joint, int jIndex){
    
    int i = user.getId();
    UserSkeleton *userSkeleton = &getSensor()->userSkeleton[i-1];
    
    // the method is called only if the skeleton is tracked, so set the stillTracking variable
    userSkeleton->stillTracking = true;
    
    if (user.getSkeleton().getJoint(joint).getPositionConfidence() > minConfidence) {
        
        // position
        double x = user.getSkeleton().getJoint(joint).getPosition().x;
        userSkeleton->skeletonPointsPos[jIndex][0]= x;
        double y = user.getSkeleton().getJoint(joint).getPosition().y;
        userSkeleton->skeletonPointsPos[jIndex][1] = y;
        double z = user.getSkeleton().getJoint(joint).getPosition().z;
        userSkeleton->skeletonPointsPos[jIndex][2] = z;
    }
    
    if (user.getSkeleton().getJoint(joint).getOrientationConfidence() > minConfidence){
        
        // orientation
        userSkeleton->skeletonPointsOri[jIndex][0] = user.getSkeleton().getJoint(joint).getOrientation().w;
        userSkeleton->skeletonPointsOri[jIndex][1] = user.getSkeleton().getJoint(joint).getOrientation().x;
        userSkeleton->skeletonPointsOri[jIndex][2] = user.getSkeleton().getJoint(joint).getOrientation().y;
        userSkeleton->skeletonPointsOri[jIndex][3] = user.getSkeleton().getJoint(joint).getOrientation().z;
    }
    
    // update confidences
    userSkeleton->skeletonPosConfidence[jIndex] = user.getSkeleton().getJoint(joint).getPositionConfidence();
    userSkeleton->skeletonOriConfidence[jIndex] = user.getSkeleton().getJoint(joint).getOrientationConfidence();
    
}


void OpenNI2SkeletonTracker::updateUserState(const nite::UserData& user, unsigned long long ts)
{
    int tmpID = user.getId();
    // if user is new
    if (user.isNew())
        USER_MESSAGE("New")
        
    // if user is detected
    else if (user.isVisible() && !getSensor()->userSkeleton[tmpID-1].visible)
        USER_MESSAGE("Visible")
            
    // if user is out of scene
    else if (!user.isVisible() && getSensor()->userSkeleton[tmpID-1].visible)
            USER_MESSAGE("Out of Scene")
                
    //if user is lost    
    else if (user.isLost())
            USER_MESSAGE("Lost")
                    
    getSensor()->userSkeleton[tmpID-1].visible = user.isVisible();
    
    if (getSensor()->userSkeleton[tmpID-1].skeletonState != user.getSkeleton().getState())
    {
        getSensor()->userSkeleton[tmpID-1].skeletonState = user.getSkeleton().getState();
        
        switch(user.getSkeleton().getState())
        {
            case nite::SKELETON_NONE:
                getSensor()->userSkeleton[tmpID-1].skeletonState = nite::SKELETON_NONE;
                USER_MESSAGE("Stopped tracking.")
                break;
            case nite::SKELETON_CALIBRATING:
                USER_MESSAGE("Calibrating...")
                getSensor()->userSkeleton[tmpID-1].skeletonState = nite::SKELETON_CALIBRATING;
                break;
            case nite::SKELETON_TRACKED:
                USER_MESSAGE("Tracking")
                getSensor()->userSkeleton[tmpID-1].skeletonState = nite::SKELETON_TRACKED;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
                USER_MESSAGE("Calibration failed - Not in Pose...")
                getSensor()->userSkeleton[tmpID-1].skeletonState = nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_HANDS:
                USER_MESSAGE("Calibration failed in hands...")
                getSensor()->userSkeleton[tmpID-1].skeletonState = nite::SKELETON_CALIBRATION_ERROR_HANDS;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_LEGS:
                USER_MESSAGE("Calibration failed in legs...")
                getSensor()->userSkeleton[tmpID-1].skeletonState = nite::SKELETON_CALIBRATION_ERROR_LEGS;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_HEAD:
                USER_MESSAGE("Calibration failed in head...")
                getSensor()->userSkeleton[tmpID-1].skeletonState = nite::SKELETON_CALIBRATION_ERROR_HEAD;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_TORSO:
                USER_MESSAGE("Calibration failed in torso...")
                getSensor()->userSkeleton[tmpID-1].skeletonState = nite::SKELETON_CALIBRATION_ERROR_TORSO;
                    break;
            }
    }
}

int OpenNI2SkeletonTracker::getDeviceStatus(){
    return deviceStatus;
}
