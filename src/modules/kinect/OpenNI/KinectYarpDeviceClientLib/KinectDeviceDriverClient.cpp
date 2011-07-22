// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "KinectDeviceDriverClient.h"

yarp::dev::KinectDeviceDriverClient::KinectDeviceDriverClient():GenericYarpDriver(){}

/*******************************************************************************************
********************************************************************************************
***GenericYarp****GenericYarp****GenericYarp****GenericYarp****GenericYarp****GenericYarp***
********************************************************************************************
*******************************************************************************************/

bool yarp::dev::KinectDeviceDriverClient::open(yarp::os::Searchable& config){
	string localPortPrefix,remotePortPrefix;
	_inUserSkeletonPort = _outPort = NULL;

	_skeletonData = new KinectSkeletonData();

	if(config.check("localPortPrefix")) localPortPrefix = config.find("localPortPrefix").asString();
	else {
		printf("\t- Error: localPortPrefix element not found in PolyDriver.\n");
		return false;
	}
	if(config.check("remotePortPrefix")) remotePortPrefix = config.find("remotePortPrefix").asString();
	else {
		printf("\t- Error: remotePortPrefix element not found in PolyDriver.\n");
		return false;
	}
	Network yarp;
	string remotePortIn = remotePortPrefix+":i";
	if(!yarp.exists(remotePortIn.c_str())){
		printf("\t- Error: remotePortPrefix port not found.\n\t  Check if KinectDeviceDriverServer is running.\n");
		return false;
	}

	if(!connectPorts(remotePortPrefix,localPortPrefix)) {
		printf("\t- Error: Could not connect ports.\n");
		return false;
	}

	_portMod = new PortCtrlMod();
	_portMod->setInterfaceDriver(this);
	_inDepthMapPort->useCallback(*this);
	_inImageMapPort->useCallback(*this);

	return true;
}

bool yarp::dev::KinectDeviceDriverClient::interruptModule(){
	return close();
}

bool yarp::dev::KinectDeviceDriverClient::close(){
	if(_portMod!=NULL)
		return _portMod->close();
	else return false;
}

bool yarp::dev::KinectDeviceDriverClient::connectPorts(string remotePortPrefix, string localPortPrefix){

	string inUserSkeletonPort =	localPortPrefix+PORTNAME_SKELETON+":i";
	string inDepthMapPort =		localPortPrefix+PORTNAME_DEPTHMAP+":i";
	string inImageMapPort =		localPortPrefix+PORTNAME_IMAGEMAP+":i";
	string outLocalPort =		localPortPrefix+":o";
	string inRemotePort =		remotePortPrefix+":i";
	string outUserSkeletonPort = remotePortPrefix+PORTNAME_SKELETON+":o";
	string outDepthMapPort =	remotePortPrefix+PORTNAME_DEPTHMAP+":o";
	string outImageMapPort =	remotePortPrefix+PORTNAME_IMAGEMAP+":o";

	//this ports were added later but work in the same way
	_inImageMapPort = new BufferedPort<ImageOf<PixelRgb>>();
	_inImageMapPort->open(inImageMapPort.c_str());
	_inDepthMapPort = new BufferedPort<ImageOf<PixelInt>>();
	_inDepthMapPort->open(inDepthMapPort.c_str());
	//server ports
	setupPorts(inUserSkeletonPort,outLocalPort);
	_outPort = _sendingPort;
	_inUserSkeletonPort = _receivingPort;


	if(!Network::connect(outLocalPort.c_str(),inRemotePort.c_str(),false) ||
		!Network::connect(outUserSkeletonPort.c_str(),inUserSkeletonPort.c_str(),false) ||
		!Network::connect(outDepthMapPort.c_str(),inDepthMapPort.c_str(),false) ||
		!Network::connect(outImageMapPort.c_str(),inImageMapPort.c_str(),false)) {
			close();
			return false;
	}

	return true;
}

void yarp::dev::KinectDeviceDriverClient::onRead(Bottle& b){_skeletonData->storeData(b);}
void yarp::dev::KinectDeviceDriverClient::onRead(ImageOf<PixelRgb>& img){_skeletonData->storeData(img);}
void yarp::dev::KinectDeviceDriverClient::onRead(ImageOf<PixelInt>& img){_skeletonData->storeData(img);}

bool yarp::dev::KinectDeviceDriverClient::updateInterface(){
	return true;
}

bool yarp::dev::KinectDeviceDriverClient::shellRespond(const Bottle& command, Bottle& reply){
	return true;
}

/*************************************************************************************************************
**************************************************************************************************************
**IKinectDeviceDriverClient****IKinectDeviceDriverClient****IKinectDeviceDriverClient****IKinectDeviceDriver**
**************************************************************************************************************
*************************************************************************************************************/

//returns false if the user skeleton is not being tracked
bool yarp::dev::KinectDeviceDriverClient::getSkeletonOrientation(Matrix *matrixArray, double *confidence,  int userID){
	if(_skeletonData->getSkeletonState(userID) != SKELETON_TRACKING)
		return false;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		matrixArray[i].resize(3,3);
		matrixArray[i].zero();
		if(matrixArray != NULL) matrixArray[i] = _skeletonData->getOrientation(userID)[i];
		if(confidence != NULL) confidence[i] = _skeletonData->getOrientationConf(userID)[i];
	}
	return true;
}

//returns false if the user skeleton is not being tracked
bool yarp::dev::KinectDeviceDriverClient::getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID){
	if(_skeletonData->getSkeletonState(userID) != SKELETON_TRACKING)
		return false;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		vectorArray[i].resize(3);
		vectorArray[i].zero();
		if(vectorArray != NULL) vectorArray[i] = _skeletonData->getPosition(userID)[i];
		if(confidence != NULL) confidence[i] = _skeletonData->getPositionConf(userID)[i];
	}
	return true;
}

int yarp::dev::KinectDeviceDriverClient::getSkeletonState(int userID){
	return _skeletonData->getSkeletonState(userID);
}

ImageOf<PixelRgb> yarp::dev::KinectDeviceDriverClient::getImageMap(){
	return _skeletonData->getImageMap();
}

ImageOf<PixelInt> yarp::dev::KinectDeviceDriverClient::getDepthMap(){
	return _skeletonData->getDepthMap();
}