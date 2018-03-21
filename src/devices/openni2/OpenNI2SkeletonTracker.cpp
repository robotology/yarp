/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis <k.theofilis@herts.ac.uk>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "OpenNI2SkeletonTracker.h"

#define USER_MESSAGE(msg) \
{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

OpenNI2SkeletonTracker::SensorStatus *OpenNI2SkeletonTracker::sensorStatus;

OpenNI2SkeletonTracker::OpenNI2SkeletonTracker(bool withTracking, bool withColorOn, bool withRgbMirrorOn, bool withDepthMirrorOn, double minConf, bool withOniPlayback, string withFileDevice, bool withOniRecord, string withOniOutputFile, bool withLoop, bool withFrameSync, bool withImageRegistration, bool prMode, int depthMode, int colorMode) :
    userTracking(withTracking),
    colorON(withColorOn),
    rgbMirrorON(withRgbMirrorOn),
    depthMirrorON(withDepthMirrorOn),
    oniPlayback(withOniPlayback),
    oniRecord(withOniRecord),
    loop(withLoop),
    frameSync(withFrameSync),
    imageRegistration(withImageRegistration),
    printMode(prMode),
    minConfidence(minConf),
    depthVideoMode(DEFAULT_DEPTH_MODE),
    colorVideoMode(DEFAULT_COLOR_MODE),
    pDevice(nullptr),
    playbackControl(nullptr),
    depthInfo(nullptr),
    colorInfo(nullptr)
{
    if (colorMode <= 11 && colorMode >= 0){
    colorVideoMode=colorMode;
    }
    
    if (depthMode <= 10 && depthMode >= 0){
    depthVideoMode=depthMode;
    }
    
    if (oniPlayback) {
        fileDevice = withFileDevice;
    }
    
    if (oniRecord) {
         oniOutputFile = withOniOutputFile;
    }
    
    init();
    initVars();
}

OpenNI2SkeletonTracker::~OpenNI2SkeletonTracker(void){
}

void OpenNI2SkeletonTracker::close(){
    if (oniRecord) {
        recorder.stop();
        cout << "Stopping recorder device...";
        recorder.destroy(); 
        cout << "Done" << endl;
    }
#ifdef OPENNI2_DRIVER_USES_NITE2
    if (userTracking) {
        cout << "Destroying user tracker...";
        for (int i=0; i < MAX_USERS; i++) {
           userTracker.stopSkeletonTracking(i+1);
        }
        userTracker.destroy();
        nite::NiTE::shutdown();
        cout << "Done" << endl;
    }
#endif
        depthStream.stop();
        imageStream.stop();
        cout << "Destroying depth stream...";
        depthStream.destroy();
        cout << "Done" << endl;
    
    if (colorON){
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

    deviceName = device.getDeviceInfo().getName();
    cout << "Device Name: " << deviceName << endl;

    cout << "OpenNI v" << openni::OpenNI::getVersion().openni::Version::major << "." << openni::OpenNI::getVersion().openni::Version::minor << "." << openni::OpenNI::getVersion().openni::Version::maintenance << "." << openni::OpenNI::getVersion().openni::Version::build <<endl;
    
#ifdef OPENNI2_DRIVER_USES_NITE2
    cout << "NiTE v" << nite::NiTE::getVersion().nite::Version::major << "." << nite::NiTE::getVersion().nite::Version::minor << "." << nite::NiTE::getVersion().nite::Version::maintenance << "." << nite::NiTE::getVersion().nite::Version::build << endl;
#endif
    if (oniRecord) {
        recorder.create(oniOutputFile.c_str());
    }

    if(deviceName!="Kinect"){
	// check if Image registration mode is supported and set accordingly
	    bool modeIsSupported = false;
	    modeIsSupported = device.isImageRegistrationModeSupported(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	    if (modeIsSupported){
	        cout << "Image registration mode is supported" << endl;
	    
	        if (imageRegistration){
		    device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
		    cout << "Image registration mode is on" << endl;
		    }
	        else{
		    device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_OFF);
		    cout << "Image registration mode is off" << endl;
	        }
	    }
	    else{
	        cout << "Image registration mode is not supported" << endl;
	    }
    } 
	// if FrameSync option is enabled
	if (frameSync){
	    device.setDepthColorSyncEnabled(true);
	    cout << "Depth/Color frame sync enabled" << endl;
	}
	else{
	    device.setDepthColorSyncEnabled(false);
	    cout << "Depth/Color frame sync disabled" << endl;
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

            // if not playback, set resolution and fps for depth stream
            depthInfo = device.getSensorInfo(openni::SENSOR_DEPTH);
            const openni::Array<openni::VideoMode>& depthModes = depthInfo->getSupportedVideoModes();
            if (!oniPlayback)
            {
                depthStream.setVideoMode(depthModes[depthVideoMode]);
            }

            // If not playback, set depth mirroring (default: off)
            if (!oniPlayback) 
            {   
                depthStream.setMirroringEnabled(depthMirrorON);
                bool dMirror = depthStream.getMirroringEnabled();
                
                if (!dMirror && depthMirrorON) 
                {
                    cout << "WARNING: Could not turn on mirroring for depth stream" << endl;
                }
                    
                else if (dMirror && depthMirrorON)
                {
                    cout << "Depth stream mirroring: ON" << endl;
                }
                    
                else 
                {
                    cout << "Depth stream mirroring: OFF" << endl;
                }
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
            cout << "Depth video mode: " << depthVideoMode << " - " << depthStream.getVideoMode().getResolutionX() << "x" << depthStream.getVideoMode().getResolutionY() << " - " << fpsCount << " fps" << endl;
            cout << "Depth stream started..." << endl;
            frameCount = playbackControl->getNumberOfFrames(depthStream);
        }
    
    if (colorON){        
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
           
            // if not playback, set resolution and fps settings for RGB stream
            colorInfo = device.getSensorInfo(openni::SENSOR_COLOR);
            const openni::Array<openni::VideoMode>& colorModes = colorInfo->getSupportedVideoModes();
            if (!oniPlayback)
            {
                imageStream.setVideoMode(colorModes[colorVideoMode]);
            }
      
            // If not playback, set RGB mirroring (default: off)
            if (!oniPlayback)
            {
               imageStream.setMirroringEnabled(rgbMirrorON);
               bool rgbMirror = imageStream.getMirroringEnabled();

               if (!rgbMirror && rgbMirrorON)
               {
                   cout << "WARNING: Could not turn on mirroring for RGB stream" << endl;
               }

               else if (rgbMirror && rgbMirrorON)
               {
                   cout << "RGB mirroring: ON" << endl;
               }

               else
               {
                   cout << "RGB stream mirroring: OFF" << endl;
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
                fpsCount = imageStream.getVideoMode().getFps();
                cout << "RGB video mode: " << colorVideoMode << " - " << imageStream.getVideoMode().getResolutionX() << "x" << imageStream.getVideoMode().getResolutionY() << " - " << fpsCount << " fps" << endl;
                cout << "RGB stream started..." << endl;
            }
        }
        else {
            colorON = false;
            cout << "No RGB camera found, RGB stream disabled" << endl;
        }

    }
        // print video modes
        if (printMode)
        {
            cout << "Depth video modes available:" << endl;
            const openni::Array<openni::VideoMode>& depthModes = depthInfo->getSupportedVideoModes();
            for (int i = 0; i<depthModes.getSize(); i++)
            {
                if (depthModes[i].getPixelFormat() == openni::PIXEL_FORMAT_DEPTH_1_MM)
                {
                    printf("%i: %ix%i, %i fps\n", i, depthModes[i].getResolutionX(), depthModes[i].getResolutionY(), depthModes[i].getFps()); 
            
                }
            }

            if (colorON)
            {
                colorInfo = device.getSensorInfo(openni::SENSOR_COLOR);
                const openni::Array<openni::VideoMode>& colorModes= colorInfo->getSupportedVideoModes();
                cout << "RGB video modes available:" << endl;
                for (int i = 0; i<colorModes.getSize(); i++) {
                    if (colorModes[i].getPixelFormat() == openni::PIXEL_FORMAT_RGB888)
                    {
                        printf("%i: %ix%i, %i fps\n", i, colorModes[i].getResolutionX(), colorModes[i].getResolutionY(), colorModes[i].getFps()); 
                    }
                }
            }

        }
#ifdef OPENNI2_DRIVER_USES_NITE2
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
#endif

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
   
    // read frames from streams
    if (colorON){
        imageStream.readFrame(&imageFrameRef);
    }
    depthStream.readFrame(&depthFrameRef);
    
    // get depth mode properties and prepare depthFrame
    depthMode = depthStream.getVideoMode();
    sensorStatus->depthFrame.resize(depthMode.getResolutionX(), depthMode.getResolutionY());
    sensorStatus->depthFrame.zero();
    
    if (colorON){
    // get RGB mode properties and prepare imageFrame
    imageMode = imageStream.getVideoMode();
    sensorStatus->imageFrame.resize(imageMode.getResolutionX(), imageMode.getResolutionY());
    sensorStatus->imageFrame.zero();
    }
    
#ifdef OPENNI2_DRIVER_USES_NITE2
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
#endif
}

// returns the sensor data struct (where all the data is)
OpenNI2SkeletonTracker::SensorStatus *OpenNI2SkeletonTracker::getSensor(){
    return sensorStatus;
}

void OpenNI2SkeletonTracker::updateSensor(){
    // get camera image
    if(colorON){ 
            if (imageStream.isValid()){
        imageStream.readFrame(&imageFrameRef);
            }
        if (imageFrameRef.isValid()){
            getSensor()->imageFrame.setQuantum(1);
            
            // put image in Yarp format
            void* tmpImage = (void*)imageFrameRef.getData();
            getSensor()->imageFrame.setExternal(tmpImage, imageMode.getResolutionX(), imageMode.getResolutionY());
        }
    }
    
    // Get depth image (in millimetres)
    if(depthStream.isValid()){
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
#ifdef OPENNI2_DRIVER_USES_NITE2
    if(userTracking && userTracker.isValid())
    {
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
#endif
}

#ifdef OPENNI2_DRIVER_USES_NITE2
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


void OpenNI2SkeletonTracker::updateUserState(const nite::UserData& user, unsigned long long ts){
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
#endif

int OpenNI2SkeletonTracker::getDeviceStatus(){
    return deviceStatus;
}
