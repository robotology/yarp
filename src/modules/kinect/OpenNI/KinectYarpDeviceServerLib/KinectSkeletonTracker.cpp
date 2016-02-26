/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "KinectSkeletonTracker.h"

KinectSkeletonTracker::KinectStatus *KinectSkeletonTracker::_kinectStatus;

#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
{																\
	printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
	return rc;													\
}

#define CHECK_ERRORS(rc, errors, what)		\
	if (rc == XN_STATUS_NO_NODE_PRESENT)	\
{										\
	XnChar strError[1024];				\
	errors.ToString(strError, 1024);	\
	printf("%s\n", strError);			\
	return (rc);						\
}

KinectSkeletonTracker::KinectSkeletonTracker(bool userDetection, bool camerasON, bool mirrorON)
{
	_userDetection = userDetection;
	_camerasON = camerasON;
	_mirrorON = mirrorON;
	initVars();
	init();
}

KinectSkeletonTracker::~KinectSkeletonTracker(void)
{
}

void KinectSkeletonTracker::close(){
	_context->Shutdown();
}

void KinectSkeletonTracker::initVars(){
	_kinectStatus = new KinectStatus();
	_kinectStatus->imgMap.resize(XN_VGA_X_RES,XN_VGA_Y_RES);
	_kinectStatus->imgMap.zero();
	_kinectStatus->depthMap.resize(XN_VGA_X_RES,XN_VGA_Y_RES);
	_kinectStatus->depthMap.zero();
	for(int i = 0; i < MAX_USERS; i++){
		_kinectStatus->userSkeleton[i].skeletonState = NO_USER;
		for(int jointIndex = 0; jointIndex < TOTAL_JOINTS; jointIndex++){
			_kinectStatus->userSkeleton[i].skeletonPointsPos[jointIndex].resize(3);
			_kinectStatus->userSkeleton[i].skeletonPointsPos[jointIndex].zero();
			_kinectStatus->userSkeleton[i].skeletonPointsOri[jointIndex].resize(3,3);
			_kinectStatus->userSkeleton[i].skeletonPointsOri[jointIndex].zero();
		}
	}
}

//returns the kinect data struct (where all the data is)
KinectSkeletonTracker::KinectStatus *KinectSkeletonTracker::getKinect(){
	return _kinectStatus;
}

int KinectSkeletonTracker::init(){

	XnStatus rc = XN_STATUS_OK;
	xn::EnumerationErrors errors;
	XnCallbackHandle h_user,h_calib,h_pose;

	//cameras settings
	XnMapOutputMode mapMode;
	mapMode.nXRes = XN_VGA_X_RES;
	mapMode.nYRes = XN_VGA_Y_RES;
	mapMode.nFPS = 30;

	// Initialize OpenNI
	_context = new Context();

	_context->Init();

	//PrimeSense license
	XnLicense *lic = new XnLicense();
	xnOSStrCopy(lic->strVendor, KINECT_VENDOR, XN_MAX_NAME_LENGTH);
	xnOSStrCopy(lic->strKey, KINECT_LICKEY,XN_MAX_LICENSE_LENGTH);
	_context->AddLicense(*lic);

	if(_camerasON){
		//depth generator setup
		_depthGenerator = new DepthGenerator();
		_depthGenerator->Create(*_context);
		_depthGenerator->SetMapOutputMode(mapMode);
		rc = _context->FindExistingNode(XN_NODE_TYPE_DEPTH, *_depthGenerator);
		CHECK_RC(rc, "Find depth generator");
		//image generator setup
		_imgGenerator = new ImageGenerator();
		_imgGenerator->Create(*_context);
		_imgGenerator->SetMapOutputMode(mapMode);
		rc = _context->FindExistingNode(XN_NODE_TYPE_IMAGE, *_imgGenerator);
		CHECK_RC(rc, "Find image generator");

		_context->SetGlobalMirror(_mirrorON);
	}

	//user generator setup
	if(_userDetection){
		_userGenerator = new UserGenerator();
		_userGenerator->Create(*_context);
		rc = _context->FindExistingNode(XN_NODE_TYPE_USER, *_userGenerator);
		CHECK_RC(rc, "Find user generator");
		//create skeleton
		_userGenerator->GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
		_userGenerator->GetSkeletonCap().SetSmoothing(0.6f);
		_userGenerator->GetSkeletonCap().RegisterCalibrationCallbacks(&CalibrationStart,&CalibrationEnd,_userGenerator,h_calib);
		_userGenerator->GetPoseDetectionCap().RegisterToPoseCallbacks(&PoseDetected,NULL,_userGenerator,h_pose);
		_userGenerator->RegisterUserCallbacks(UserCreated,UserDestroyed,NULL,h_user);
	}
	// Initialization done. Start generating
	rc = _context->StartGeneratingAll();
	CHECK_RC(rc, "StartGenerating");

	return rc;
}

void KinectSkeletonTracker::updateKinect(bool wait){
	// Read next available data
	if(wait) _context->WaitAnyUpdateAll();
	else _context->WaitNoneUpdateAll();

	//get camera image
	if(_camerasON && _imgGenerator->IsDataNew()){
		ImageMetaData imgMD;
		_imgGenerator->GetMetaData(imgMD);
		unsigned char* pImg = (unsigned char*)_imgGenerator->GetImageMap();
		memcpy(getKinect()->imgMap.getRawImage(), pImg, getKinect()->imgMap.getRawImageSize());
	}

	//get depth image (in milimeters)
	// Get information about the depth image
	if(_camerasON && _depthGenerator->IsDataNew()){
		DepthMetaData depthMD;
		_depthGenerator->GetMetaData(depthMD);
		const XnDepthPixel* pDepth = _depthGenerator->GetDepthMap();
		int index = 0;
		for(int x = 0; x < getKinect()->depthMap.width(); x++){//can't be a memcpy because the depthMap is a different type from pixel
			for(int y = 0; y < getKinect()->depthMap.height(); y++){
				index = (y*getKinect()->depthMap.width())+x;
				getKinect()->depthMap.pixel(x,y) = pDepth[index];
			}
		}
	}

	//user skeleton detection data
	if(_userDetection && _userGenerator->IsDataNew()){
		//get data per user
		XnUserID users[MAX_USERS];
		XnUInt16 nUsers = MAX_USERS;
		_userGenerator->GetUsers(users,nUsers);
		XnSkeletonJointTransformation joint;
		int jointIndex;
		for(int userIndex = 0; userIndex < MAX_USERS; userIndex++){
			if(users[userIndex]>MAX_USERS) continue;
			if(_userGenerator->GetSkeletonCap().IsTracking(users[userIndex])){
				getKinect()->userSkeleton[users[userIndex]].skeletonState = SKELETON_TRACKING;
				for(int totalJoints = TOTAL_JOINTS; totalJoints != 0; totalJoints--){
					jointIndex = (totalJoints-1);
					_userGenerator->GetSkeletonCap().GetSkeletonJoint(users[userIndex],(XnSkeletonJoint)totalJoints,joint);
					UserSkeleton *userSkeleton = &getKinect()->userSkeleton[users[userIndex]];
					//position
					userSkeleton->skeletonPointsPos[jointIndex][0] = joint.position.position.X;
					userSkeleton->skeletonPointsPos[jointIndex][1] = joint.position.position.Y;
					userSkeleton->skeletonPointsPos[jointIndex][2] = joint.position.position.Z;
					userSkeleton->skeletonPosConfidence[jointIndex] = joint.position.fConfidence;
					//orientation
					userSkeleton->skeletonPointsOri[jointIndex][0][0] = (double)joint.orientation.orientation.elements[0];
					userSkeleton->skeletonPointsOri[jointIndex][0][1] = (double)joint.orientation.orientation.elements[3];
					userSkeleton->skeletonPointsOri[jointIndex][0][2] = (double)joint.orientation.orientation.elements[6];
					userSkeleton->skeletonPointsOri[jointIndex][1][0] = (double)joint.orientation.orientation.elements[1];
					userSkeleton->skeletonPointsOri[jointIndex][1][1] = (double)joint.orientation.orientation.elements[4];
					userSkeleton->skeletonPointsOri[jointIndex][1][2] = (double)joint.orientation.orientation.elements[7];
					userSkeleton->skeletonPointsOri[jointIndex][2][0] = (double)joint.orientation.orientation.elements[2];
					userSkeleton->skeletonPointsOri[jointIndex][2][1] = (double)joint.orientation.orientation.elements[5];
					userSkeleton->skeletonPointsOri[jointIndex][2][2] = (double)joint.orientation.orientation.elements[8];
					userSkeleton->skeletonOriConfidence[jointIndex] = joint.orientation.fConfidence;
				}
			}
		}
	}
}


/*********************************************************************************************************************************
*************************************************USER*GENERATOR*CALLBACKS*********************************************************
*********************************************************************************************************************************/
void XN_CALLBACK_TYPE UserCreated(UserGenerator& generator, XnUserID userID, void* pCookie){
	generator.GetPoseDetectionCap().StartPoseDetection("Psi",userID);
	KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState = USER_DETECTED;
}

void XN_CALLBACK_TYPE UserDestroyed(UserGenerator& generator, XnUserID userID, void* pCookie){
	KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState = USER_LOST;
}

void XN_CALLBACK_TYPE PoseDetected(PoseDetectionCapability &poseDetection, const XnChar* strPose, XnUserID userID, void *pCookie){
	UserGenerator* userGenerator = (UserGenerator*)pCookie;
	userGenerator->GetSkeletonCap().RequestCalibration(userID,false);
	userGenerator->GetPoseDetectionCap().StopPoseDetection(userID);
	KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState = CALIBRATING;
}

void XN_CALLBACK_TYPE CalibrationStart(SkeletonCapability &skeleton, XnUserID userID, void *pCookie){
}

void XN_CALLBACK_TYPE CalibrationEnd(SkeletonCapability &skeleton, XnUserID userID, XnBool bSuccess, void *pCookie){
	if(bSuccess){
		skeleton.StartTracking(userID);
		KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState = SKELETON_TRACKING;
	}else{
		UserGenerator *userGenerator = (UserGenerator*)pCookie;
		userGenerator->GetPoseDetectionCap().StartPoseDetection("Psi",userID);
		KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState = USER_DETECTED;
	}
}
