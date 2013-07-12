// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright
* Author: Konstantinos Theofilis, University of Hertfordshire, 2013
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
*/


#include "OpenNI2SkeletonTracker.h"
#include "OpenNI2DeviceDriverServer.h"

yarp::dev::OpenNI2DeviceDriverServer::OpenNI2DeviceDriverServer(void)
{
}

yarp::dev::OpenNI2DeviceDriverServer::~OpenNI2DeviceDriverServer(void)
{
}

void yarp::dev::OpenNI2DeviceDriverServer::openPorts(string portPrefix, bool userTracking, bool camerasON){
    
	string receivingPortName = portPrefix+":i";
	receivingPort = new BufferedPort<Bottle>();
	receivingPort->open(receivingPortName.c_str());

	if(userTracking){
		string skeletonPortName = portPrefix+PORTNAME_SKELETON+":o";
		skeletonPort = new BufferedPort<Bottle>();
		skeletonPort->open(skeletonPortName.c_str());
	}

	if(camerasON){
		depthFramePort = new BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono16> >();
		string strTemp = portPrefix+PORTNAME_DEPTHFRAME+":o";
		depthFramePort->open(strTemp.c_str());
		imageFramePort = new BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >();
		strTemp = portPrefix+PORTNAME_IMAGEFRAME+":o";
		imageFramePort->open(strTemp.c_str());
	}
}

void yarp::dev::OpenNI2DeviceDriverServer::sendSensorData(){
	OpenNI2SkeletonTracker::UserSkeleton *userSkeleton = OpenNI2SkeletonTracker::getSensor()->userSkeleton;
	double *joint;
	int index = 0;

	//Creating a Stamp with the current system time in seconds.milliseconds
	Stamp timestamp(index,Time::now());
	//cameras data
	if(camerasON){
		//image frame data
		imageFramePort->prepare() = OpenNI2SkeletonTracker::getSensor()->imageFrame;
		imageFramePort->setEnvelope(timestamp);
		imageFramePort->write();
		//depth frame data
		depthFramePort->prepare() = OpenNI2SkeletonTracker::getSensor()->depthFrame;
		depthFramePort->setEnvelope(timestamp);
		depthFramePort->write();
	}

	//skeleton data
	if(userTracking)
		for(int i = 0; i < MAX_USERS; i++){
			if(userSkeleton[i].skeletonState == nite::SKELETON_TRACKED){
				Bottle &botSkeleton = skeletonPort->prepare();
				botSkeleton.clear();
				skeletonPort->setEnvelope(timestamp);
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
                    botSkeleton.addDouble(userSkeleton[i].skeletonOriConfidence[jointIndex]);
				}
				skeletonPort->write();
            }
//			else if(userSkeleton[i].skeletonState == USER_DETECTED){
//				Bottle &botDetected = skeletonPort->prepare();
//				botDetected.clear();
//				skeletonPort->setEnvelope(timestamp);
//				botDetected.addString(USER_DETECTED_MSG);
//				botDetected.addInt(i);
//				skeletonPort->write();}
			else if(userSkeleton[i].skeletonState == nite::SKELETON_CALIBRATING) {
				Bottle &botCalib = skeletonPort->prepare();
				botCalib.clear();
				skeletonPort->setEnvelope(timestamp);
				botCalib.addString("CALIBRATING FOR USER");
				botCalib.addInt(i);
				skeletonPort->write();
			}else if(userSkeleton[i].skeletonState == nite::SKELETON_NONE){
				//userSkeleton[i].skeletonState = NO_USER;
				Bottle &botCalib = skeletonPort->prepare();
				botCalib.clear();
				skeletonPort->setEnvelope(timestamp);
				botCalib.addString(USER_LOST_MSG);
				botCalib.addInt(i);
				skeletonPort->write();
			}
		}
}


//device driver stuff
bool yarp::dev::OpenNI2DeviceDriverServer::open(yarp::os::Searchable& config){
	//this function is used in case of the Yarp Device being used as local
	std::cout << "Starting OpenNI2 YARP Device please wait..." << endl;
	string portPrefix;

	if(config.check("noCameras")) camerasON = false;
	else camerasON = true;

	if(config.check("noMirror")) mirrorON = false;
	else mirrorON = true;

	if(config.check("noUserTracking")) userTracking = false;
	else userTracking = true;

	if(config.check("name")){
		portPrefix = config.find("name").asString();
		withOpenPorts = true;
		openPorts(portPrefix, userTracking, camerasON);
    }
    else {
        portPrefix = "/OpenNI2";
        withOpenPorts  = true;
        openPorts(portPrefix, userTracking, camerasON);
    }
	skeleton = new OpenNI2SkeletonTracker(userTracking,camerasON,mirrorON);
	std::cout << "OpenNI2 Yarp Device started." << endl;
	return true;
}

bool yarp::dev::OpenNI2DeviceDriverServer::close(){
	skeleton->close();
	if(withOpenPorts){
		cout << "Closing ports...";
        
        if(userTracking) {
            skeletonPort->close();
        }
        
		if(camerasON){
			depthFramePort->close();
			imageFramePort->close();
		}
        receivingPort->close();
		withOpenPorts = false;
        cout << "Done" << endl;
	}
	return true;
}

bool yarp::dev::OpenNI2DeviceDriverServer::updateInterface(bool wait){
	//std::cout << "updateInterface()" << endl;
	//update sensor data
	skeleton->updateSensor(wait);
	//send sensor data to ports
	if(withOpenPorts){
        sendSensorData();
    }
	return true;
}


bool yarp::dev::OpenNI2DeviceDriverServer::startService(){
	//std::cout << "startService()" << endl;
	//returns false so that the updateService is started
	return false;
}

bool yarp::dev::OpenNI2DeviceDriverServer::updateService(){
	//std::cout << "updateService()" << endl;
	updateInterface(true);
	return true;
}

bool yarp::dev::OpenNI2DeviceDriverServer::stopService(){
	//std::cout << "stopService()" << endl;
	return close();
}


//returns false if the user skeleton is not being tracked
bool yarp::dev::OpenNI2DeviceDriverServer::getSkeletonOrientation(Vector *vectorArray, double *confidence,  int userID){
	updateInterface(false);
	if(OpenNI2SkeletonTracker::getSensor()->userSkeleton[userID].skeletonState != nite::SKELETON_TRACKED)
		return false;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		vectorArray[i].resize(4);
		vectorArray[i].zero();
		vectorArray[i] = OpenNI2SkeletonTracker::getSensor()->userSkeleton[userID].skeletonPointsOri[i];
		confidence[i] = (double)OpenNI2SkeletonTracker::getSensor()->userSkeleton[userID].skeletonPosConfidence[i];
	}
	return true;
}

//returns false if the user skeleton is not being tracked
bool yarp::dev::OpenNI2DeviceDriverServer::getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID){
	updateInterface(false);
	if(OpenNI2SkeletonTracker::getSensor()->userSkeleton[userID].skeletonState != nite::SKELETON_TRACKED)
		return false;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		vectorArray[i].resize(3);
		vectorArray[i].zero();
		vectorArray[i] = OpenNI2SkeletonTracker::getSensor()->userSkeleton[userID].skeletonPointsPos[i];
		confidence[i] = (double)OpenNI2SkeletonTracker::getSensor()->userSkeleton[userID].skeletonPosConfidence[i];
	}
	return true;
}

int *yarp::dev::OpenNI2DeviceDriverServer::getSkeletonState(){
	updateInterface(false);
	int *userState = new int[MAX_USERS];
	for(int i = 0; i < MAX_USERS; i++){
		userState[i] = getSkeletonState(i);
	}
	return userState;
}

int yarp::dev::OpenNI2DeviceDriverServer::getSkeletonState(int userID){
	updateInterface(false);
	return OpenNI2SkeletonTracker::getSensor()->userSkeleton[userID].skeletonState;
}

ImageOf<PixelRgb> yarp::dev::OpenNI2DeviceDriverServer::getImageFrame(){
	updateInterface(false);
	return OpenNI2SkeletonTracker::getSensor()->imageFrame;
}

ImageOf<PixelMono16> yarp::dev::OpenNI2DeviceDriverServer::getDepthFrame(){
	updateInterface(false);
	return OpenNI2SkeletonTracker::getSensor()->depthFrame;
}

