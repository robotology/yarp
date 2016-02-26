/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "KinectSkeletonTracker.h"
#include "KinectDeviceDriverServer.h"

/*yarp::dev::KinectDeviceDriverServer::KinectDeviceDriverServer(bool openPorts, bool userDetection){
//when this is used as a local Yarp Device the ports are only opened later
_openPorts = openPorts;
_userDetection = userDetection;
}*/

yarp::dev::KinectDeviceDriverServer::KinectDeviceDriverServer(void)
{
}

yarp::dev::KinectDeviceDriverServer::~KinectDeviceDriverServer(void)
{
}

void yarp::dev::KinectDeviceDriverServer::openPorts(string portPrefix, bool userDetection, bool camerasON){
	//std::cout << "openPorts()" << endl;
	_openPorts = true;

	string receivingPortName = portPrefix+":i";
	_receivingPort = new BufferedPort<Bottle>();
	_receivingPort->open(receivingPortName.c_str());

	if(userDetection){
		string skeletonPortName = portPrefix+PORTNAME_SKELETON+":o";
		_skeletonPort = new BufferedPort<Bottle>();
		_skeletonPort->open(skeletonPortName.c_str());
	}

	if(camerasON){
		_depthMapPort = new BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> >();
		string strTemp = portPrefix+PORTNAME_DEPTHMAP+":o";
		_depthMapPort->open(strTemp.c_str());
		_imgMapPort = new BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >();
		strTemp = portPrefix+PORTNAME_IMAGEMAP+":o";
		_imgMapPort->open(strTemp.c_str());
	}
}

void yarp::dev::KinectDeviceDriverServer::sendKinectData(){
	KinectSkeletonTracker::UserSkeleton *userSkeleton = KinectSkeletonTracker::getKinect()->userSkeleton;
	double *joint;
	int index = 0;

	//Creating a Stamp with the current system time in seconds.milliseconds
	Stamp timestamp(index,Time::now());

	//cameras data
	if(_camerasON){
		//image map data
		_imgMapPort->prepare() = KinectSkeletonTracker::getKinect()->imgMap;
		_imgMapPort->setEnvelope(timestamp);
		_imgMapPort->write();

		//image depth map data
		_depthMapPort->prepare() = KinectSkeletonTracker::getKinect()->depthMap;
		_depthMapPort->setEnvelope(timestamp);
		_depthMapPort->write();
	}

	//skeleton data
	if(_userDetection)
		for(int i = 0; i < MAX_USERS; i++){
			if(userSkeleton[i].skeletonState == SKELETON_TRACKING){
				Bottle &botSkeleton = _skeletonPort->prepare();
				botSkeleton.clear();
				_skeletonPort->setEnvelope(timestamp);
				//user number
				Bottle &userBot = botSkeleton.addList();
				userBot.addVocab(USER_VOCAB);
				userBot.addInt(i);
				for(int jointIndex = 0; jointIndex < TOTAL_JOINTS; jointIndex++){
					//position
					botSkeleton.addVocab(POSITION_VOCAB);
					joint = userSkeleton[i].skeletonPointsPos[jointIndex].data();
					Bottle &botList = botSkeleton.addList();
					botList.addDouble(joint[0]);
					botList.addDouble(joint[1]);
					botList.addDouble(joint[2]);
					botSkeleton.addDouble(userSkeleton[i].skeletonPosConfidence[jointIndex]);
					//orientation
					joint = userSkeleton[i].skeletonPointsOri[jointIndex].data();
					botSkeleton.addVocab(ORIENTATION_VOCAB);
					Bottle &botList2 = botSkeleton.addList();
					botList2.addDouble(joint[0]);
					botList2.addDouble(joint[1]);
					botList2.addDouble(joint[2]);
					botList2.addDouble(joint[3]);
					botList2.addDouble(joint[4]);
					botList2.addDouble(joint[5]);
					botList2.addDouble(joint[6]);
					botList2.addDouble(joint[7]);
					botList2.addDouble(joint[8]);
					botSkeleton.addDouble(userSkeleton[i].skeletonOriConfidence[jointIndex]);
				}
				_skeletonPort->write();
			}else if(userSkeleton[i].skeletonState == USER_DETECTED){
				Bottle &botDetected = _skeletonPort->prepare();
				botDetected.clear();
				_skeletonPort->setEnvelope(timestamp);
				botDetected.addString(USER_DETECTED_MSG);
				botDetected.addInt(i);
				_skeletonPort->write();
			}else if(userSkeleton[i].skeletonState == CALIBRATING){
				Bottle &botCalib = _skeletonPort->prepare();
				botCalib.clear();
				_skeletonPort->setEnvelope(timestamp);
				botCalib.addString(USER_CALIBRATING_MSG);
				botCalib.addInt(i);
				_skeletonPort->write();
			}else if(userSkeleton[i].skeletonState == USER_LOST){
				userSkeleton[i].skeletonState = NO_USER;
				Bottle &botCalib = _skeletonPort->prepare();
				botCalib.clear();
				_skeletonPort->setEnvelope(timestamp);
				botCalib.addString(USER_LOST_MSG);
				botCalib.addInt(i);
				_skeletonPort->write();
			}
		}
}

/*************************************************************************************************************
**************************************************************************************************************
******DeviceDriver*****DeviceDriver*****DeviceDriver******DeviceDriver*****DeviceDriver*****DeviceDriver******
**************************************************************************************************************
*************************************************************************************************************/

bool yarp::dev::KinectDeviceDriverServer::open(yarp::os::Searchable& config){
	//this function is used in case of the Yarp Device being used as local
	std::cout << "Starting Kinect Yarp Device please wait..." << endl;
	string portPrefix;

	if(config.check("noCameras")) _camerasON = false;
	else _camerasON = true;

	if(config.check("noMirror")) _mirrorON = false;
	else _mirrorON = true;

	if(config.check("userDetection")) _userDetection = true;
	else _userDetection = false;

	if(config.check("portPrefix")){
		portPrefix = config.find("portPrefix").asString();
		_openPorts = true;
		openPorts(portPrefix, _userDetection,_camerasON);
	}else _openPorts  = false;

	_skeleton = new KinectSkeletonTracker(_userDetection,_camerasON,_mirrorON);
	std::cout << "Kinect Yarp Device started. Enjoy!" << endl;
	return true;
}

bool yarp::dev::KinectDeviceDriverServer::close(){
	_skeleton->close();
	if(_openPorts){
		if(_userDetection) _skeletonPort->close();
		if(_camerasON){
			_depthMapPort->close();
			_imgMapPort->close();
		}
		_openPorts = false;
	}
	return true;
}

/*************************************************************************************************************
**************************************************************************************************************
****GenericYarpDriver****GenericYarpDriver****GenericYarpDriver****GenericYarpDriver****GenericYarpDriver*****
**************************************************************************************************************
*************************************************************************************************************/

bool yarp::dev::KinectDeviceDriverServer::updateInterface(bool wait){
	//std::cout << "updateInterface()" << endl;
	//update kinect data
	_skeleton->updateKinect(wait);
	//send kinect data to ports
	if(_openPorts)
		sendKinectData();
	return true;
}

/*************************************************************************************************************
**************************************************************************************************************
***IService****IService****IService****IService****IService****IService****IService****IService****IService***
**************************************************************************************************************
*************************************************************************************************************/

bool yarp::dev::KinectDeviceDriverServer::startService(){
	//std::cout << "startService()" << endl;
	//returns false so that the updateService is started
	return false;
}

bool yarp::dev::KinectDeviceDriverServer::updateService(){
	//std::cout << "updateService()" << endl;
	updateInterface(true);
	return true;
}

bool yarp::dev::KinectDeviceDriverServer::stopService(){
	//std::cout << "stopService()" << endl;
	return close();
}

/*************************************************************************************************************
**************************************************************************************************************
**IKinectDeviceDriverClient****IKinectDeviceDriverClient****IKinectDeviceDriverClient****IKinectDeviceDriver**
**************************************************************************************************************
*************************************************************************************************************/

//returns false if the user skeleton is not being tracked
bool yarp::dev::KinectDeviceDriverServer::getSkeletonOrientation(Matrix *matrixArray, double *confidence,  int userID){
	updateInterface(false);
	if(KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState != SKELETON_TRACKING)
		return false;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		matrixArray[i].resize(3,3);
		matrixArray[i].zero();
		matrixArray[i] = KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonPointsOri[i];
		confidence[i] = (double)KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonPosConfidence[i];
	}
	return true;
}

//returns false if the user skeleton is not being tracked
bool yarp::dev::KinectDeviceDriverServer::getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID){
	updateInterface(false);
	if(KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState != SKELETON_TRACKING)
		return false;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		vectorArray[i].resize(3);
		vectorArray[i].zero();
		vectorArray[i] = KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonPointsPos[i];
		confidence[i] = (double)KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonPosConfidence[i];
	}
	return true;
}

int *yarp::dev::KinectDeviceDriverServer::getSkeletonState(){
	updateInterface(false);
	int *userState = new int[MAX_USERS];
	for(int i = 0; i < MAX_USERS; i++){
		userState[i] = getSkeletonState(i);
	}
	return userState;
}

int yarp::dev::KinectDeviceDriverServer::getSkeletonState(int userID){
	updateInterface(false);
	return KinectSkeletonTracker::getKinect()->userSkeleton[userID].skeletonState;
}

ImageOf<PixelRgb> yarp::dev::KinectDeviceDriverServer::getImageMap(){
	updateInterface(false);
	return KinectSkeletonTracker::getKinect()->imgMap;
}

ImageOf<PixelInt> yarp::dev::KinectDeviceDriverServer::getDepthMap(){
	updateInterface(false);
	return KinectSkeletonTracker::getKinect()->depthMap;
}
