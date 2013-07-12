// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Konstantinos Theofilis, University of Hertfordshire, 2013
* Copyright Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao

*/


#include "OpenNI2SkeletonTracker.h"

#define USER_MESSAGE(msg) \
{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

OpenNI2SkeletonTracker::SensorStatus *OpenNI2SkeletonTracker::sensorStatus;

OpenNI2SkeletonTracker::OpenNI2SkeletonTracker(bool withTracking, bool withCamerasOn, bool withMirrorOn)
{
	userTracking= withTracking;
	camerasON = withCamerasOn;
	mirrorON = withMirrorOn;
    init();
    initVars();
}

OpenNI2SkeletonTracker::~OpenNI2SkeletonTracker(void)
{
}

void OpenNI2SkeletonTracker::close(){
    
    if (userTracking) {
        cout << "Destroying user tracker...";
        delete getSensor();
        nite::NiTE::shutdown();
        cout << "Done" << endl;
    }

    if (camerasON) {
        cout << "Destroying depth stream...";
        depthStream.destroy();
        cout << "Done" << endl;
        cout << "Destroying RGB stream...";
        imageStream.destroy();
        cout << "Done" << endl;
    }
    
    cout << "Closing sensor device...";
	device.close();
    cout << "Done" << endl;
}

int OpenNI2SkeletonTracker::init(){
    
    openni::Status rc = openni::OpenNI::initialize();
    if (rc != openni::STATUS_OK)
    {
        printf("Initialize failed\n%s\n", openni::OpenNI::getExtendedError());
		return 1;
    }

    rc = device.open(openni::ANY_DEVICE);

	if (rc != openni::STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", openni::OpenNI::getExtendedError());
		return 2;
	}

    if(camerasON){
        
        // setup and start depth stream
        if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
        {

            rc = depthStream.create(device, openni::SENSOR_DEPTH);

            if (rc != openni::STATUS_OK)
            {
                printf("Couldn't create depth stream\n%s\n", openni::OpenNI::getExtendedError());
                return 3;
            }
        }
        
        rc = depthStream.start();
        if (rc != openni::STATUS_OK)
        {
			printf("Couldn't start the depth stream\n%s\n", openni::OpenNI::getExtendedError());
            return 4;
		}
        
        else {
        cout << "Depth stream started..." << endl;
        }
        
        // setup and start colour stream
        if (device.getSensorInfo(openni::SENSOR_COLOR) != NULL)
        {
            rc = imageStream.create(device, openni::SENSOR_COLOR);
            if (rc != openni::STATUS_OK)
            {
                printf("Couldn't create RGB stream\n%s\n", openni::OpenNI::getExtendedError());
                return 3;
            }
        }
        
        rc = imageStream.start();
        if (rc != openni::STATUS_OK)
        {
			printf("Couldn't start the RGB stream\n%s\n", openni::OpenNI::getExtendedError());
            return 4;
		}
        
        else {
            cout << "RGB stream started..." << endl;
        }
	}
    
    if (userTracking){
        
        // setup and start user tracking
        nite::Status niteRc = nite::NiTE::initialize();
        niteRc = userTracker.create();

        if (niteRc != nite::STATUS_OK)
        {
            printf("Couldn't create user tracker\n");
            return 3;
        }
        
        else {
            cout << "User tracker started..." << endl;
        }
        
        printf("\nStart moving around to get detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");
    }
   
	return rc;
}

void OpenNI2SkeletonTracker::initVars(){
    sensorStatus = new SensorStatus;
    
    // read frames from streams
    imageStream.readFrame(&imageFrameRef);
    depthStream.readFrame(&depthFrameRef);
    
    // get depth mode properties and prepare depthFrame
    depthMode = depthStream.getVideoMode();
    sensorStatus->depthFrame.resize(depthMode.getResolutionX(), depthMode.getResolutionY());
    sensorStatus->depthFrame.zero();
    
    // get RGB mode properties nd prepare imageFrame
    imageMode = imageStream.getVideoMode();
    sensorStatus->imageFrame.resize(imageMode.getResolutionX(), imageMode.getResolutionY());
    sensorStatus->imageFrame.zero();
    
    // initialise UserSkeleton struct
    for (int i = 0; i < MAX_USERS; i++) {
        sensorStatus->userSkeleton[i].skeletonState = nite::SKELETON_NONE;
        sensorStatus->userSkeleton[i].uID = 0;
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

void OpenNI2SkeletonTracker::updateSensor(bool wait){
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
            if (depthFrameRef.isValid()){
                getSensor()->depthFrame.setQuantum(1);
                
                // put image in Yarp format
                void* tmpDepth = (void*)depthFrameRef.getData();
                getSensor()->depthFrame.setExternal(tmpDepth, depthMode.getResolutionX(), depthMode.getResolutionY());
            }
        }
	// user skeleton tracking data
    if(userTracking && userTracker.isValid()){
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
    UserSkeleton *userSkeleton = &getSensor()->userSkeleton[i];
    
    if (user.getSkeleton().getJoint(joint).getPositionConfidence() > 0.6) {
    
    // position
    double x = user.getSkeleton().getJoint(joint).getPosition().x;
    userSkeleton->skeletonPointsPos[jIndex][0]= x;
    double y = user.getSkeleton().getJoint(joint).getPosition().y;
    userSkeleton->skeletonPointsPos[jIndex][1] = y;
    double z = user.getSkeleton().getJoint(joint).getPosition().x;
    userSkeleton->skeletonPointsPos[jIndex][2] = z;
    }
    
    if (user.getSkeleton().getJoint(joint).getOrientationConfidence() > 0.6){
        
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
    // if user is new
	if (user.isNew())
		USER_MESSAGE("New")
        
    // if user is detected
    else if (user.isVisible() && !getSensor()->userSkeleton[user.getId()].visible)
        USER_MESSAGE("Visible")
        
    // if user is out of scene
    else if (!user.isVisible() && getSensor()->userSkeleton[user.getId()].visible)
    USER_MESSAGE("Out of Scene")
        
    //if user is lost    
    else if (user.isLost())
    USER_MESSAGE("Lost")
                    
    getSensor()->userSkeleton[user.getId()].visible = user.isVisible();
    getSensor()->userSkeleton[user.getId()].uID = user.getId();
   	if (getSensor()->userSkeleton[user.getId()].skeletonState != user.getSkeleton().getState())
	{
        getSensor()->userSkeleton[user.getId()].skeletonState = user.getSkeleton().getState();
		switch(user.getSkeleton().getState())
		{
            case nite::SKELETON_NONE:
                getSensor()->userSkeleton[user.getId()].skeletonState = nite::SKELETON_NONE;
                USER_MESSAGE("Stopped tracking.")
                break;
            case nite::SKELETON_CALIBRATING:
                USER_MESSAGE("Calibrating...")
                getSensor()->userSkeleton[user.getId()].skeletonState = nite::SKELETON_CALIBRATING;
                break;
            case nite::SKELETON_TRACKED:
                USER_MESSAGE("Tracking")
                getSensor()->userSkeleton[user.getId()].skeletonState = nite::SKELETON_TRACKED;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
                USER_MESSAGE("Calibration failed - Not in Pose...")
                getSensor()->userSkeleton[user.getId()].skeletonState = nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_HANDS:
                USER_MESSAGE("Calibration failed in hands...")
                getSensor()->userSkeleton[user.getId()].skeletonState = nite::SKELETON_CALIBRATION_ERROR_HANDS;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_LEGS:
                USER_MESSAGE("Calibration failed in legs...")
                getSensor()->userSkeleton[user.getId()].skeletonState = nite::SKELETON_CALIBRATION_ERROR_LEGS;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_HEAD:
                USER_MESSAGE("Calibration failed in head...")
                getSensor()->userSkeleton[user.getId()].skeletonState = nite::SKELETON_CALIBRATION_ERROR_HEAD;
                break;
            case nite::SKELETON_CALIBRATION_ERROR_TORSO:
                USER_MESSAGE("Calibration failed in torso...")
                getSensor()->userSkeleton[user.getId()].skeletonState = nite::SKELETON_CALIBRATION_ERROR_TORSO;
                break;
		}
	}
}

