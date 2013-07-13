// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Duarte Aragao
 * Author: Duarte Aragao
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * Copyright
 * Author: Konstantinos Theofilis, University of Hertfordshire, 2013
 */


#include "OpenNI2DeviceDriverClient.h"

yarp::dev::OpenNI2DeviceDriverClient::OpenNI2DeviceDriverClient(){}

/*******************************************************************************************
 ***GenericYarp****GenericYarp****GenericYarp****GenericYarp****GenericYarp****GenericYarp***
 ********************************************************************************************/

bool yarp::dev::OpenNI2DeviceDriverClient::open(yarp::os::Searchable& config){
    string localPortPrefix,remotePortPrefix;
    inUserSkeletonPort = outPort = NULL;
    
    skeletonData = new OpenNI2SkeletonData();
    
    if(config.check("localName")) localPortPrefix = config.find("localName").asString();
    else {
        printf("\t- Error: localName element not found in PolyDriver.\n");
        return false;
    }
    if(config.check("remoteName")) remotePortPrefix = config.find("remoteName").asString();
    else {
        printf("\t- Error: remoteName element not found in PolyDriver.\n");
        return false;
    }
    Network yarp;
    string remotePortIn = remotePortPrefix+":i";
    if(!yarp.exists(remotePortIn.c_str())){
        printf("\t- Error: remote port not found. (%s)\n\t  Check if OpenNI2DeviceDriverServer is running.\n",remotePortIn.c_str());
        return false;
    }
    
    if(!connectPorts(remotePortPrefix,localPortPrefix)) {
        printf("\t- Error: Could not connect or create ports.\n");
        return false;
    }
    
    inUserSkeletonPort->useCallback(*this);
    inDepthFramePort->useCallback(*this);
    inImageFramePort->useCallback(*this);
    
    return true;
}

bool yarp::dev::OpenNI2DeviceDriverClient::close(){
    return true;
}

bool yarp::dev::OpenNI2DeviceDriverClient::connectPorts(string remotePortPrefix, string localPortPrefix){
    
    string tinUserSkeletonPort = localPortPrefix+PORTNAME_SKELETON+":i";
    string tinDepthFramePort = localPortPrefix+PORTNAME_DEPTHFRAME+":i";
    string tinImageFramePort = localPortPrefix+PORTNAME_IMAGEFRAME+":i";
    string toutLocalPort = localPortPrefix+":o";
    string tinRemotePort = remotePortPrefix+":i";
    string toutUserSkeletonPort = remotePortPrefix+PORTNAME_SKELETON+":o";
    string toutDepthFramePort = remotePortPrefix+PORTNAME_DEPTHFRAME+":o";
    string toutImageFramePort = remotePortPrefix+PORTNAME_IMAGEFRAME+":o";
    
    bool portsOK = true;
    
    // these ports were added later but work in the same way
    inImageFramePort = new BufferedPort<ImageOf<PixelRgb> >();
    portsOK = portsOK && inImageFramePort->open(tinImageFramePort.c_str());
    inDepthFramePort = new BufferedPort<ImageOf<PixelMono16> >();
    portsOK = portsOK && inDepthFramePort->open(tinDepthFramePort.c_str());
    
    outPort = new BufferedPort<Bottle>();
    portsOK = portsOK && outPort->open(toutLocalPort.c_str());
    inUserSkeletonPort = new BufferedPort<Bottle>();
    portsOK = portsOK && inUserSkeletonPort->open(tinUserSkeletonPort.c_str());
    
    bool anyPort = false;
    if(Network::connect(toutLocalPort.c_str(),tinRemotePort.c_str(),NULL,false)) anyPort = true;
    if(Network::connect(toutUserSkeletonPort.c_str(),tinUserSkeletonPort.c_str(),NULL,false)) anyPort = true;
    if(Network::connect(toutDepthFramePort.c_str(),tinDepthFramePort.c_str(),NULL,false)) anyPort = true;
    if(Network::connect(toutImageFramePort.c_str(),tinImageFramePort.c_str(),NULL,false)) anyPort = true;
    
    if(!anyPort || !portsOK) close();
    
    return anyPort && portsOK;
}

void yarp::dev::OpenNI2DeviceDriverClient::onRead(Bottle& b){skeletonData->storeData(b);}
void yarp::dev::OpenNI2DeviceDriverClient::onRead(ImageOf<PixelRgb>& img){skeletonData->storeData(img);}
void yarp::dev::OpenNI2DeviceDriverClient::onRead(ImageOf<PixelMono16>& img){skeletonData->storeData(img);}

/*****************************************************************************************************
 **IOpenNI2DeviceDriverClient****
 *****************************************************************************************************/

// returns false if the user skeleton is not being tracked
bool yarp::dev::OpenNI2DeviceDriverClient::getSkeletonOrientation(Vector *vectorArray, double *confidence,  int userID){
    if(skeletonData->getSkeletonState(userID) != nite::SKELETON_TRACKED)
        return false;
    for(int i = 0; i < TOTAL_JOINTS; i++){
        vectorArray[i].resize(4);
        vectorArray[i].zero();
        if(vectorArray != NULL) vectorArray[i] = skeletonData->getOrientation(userID)[i];
        if(confidence != NULL) confidence[i] = skeletonData->getOrientationConf(userID)[i];
    }
    return true;
}

// returns false if the user skeleton is not being tracked
bool yarp::dev::OpenNI2DeviceDriverClient::getSkeletonPosition(Vector *vectorArray, double *confidence,  int userID){
    if(skeletonData->getSkeletonState(userID) != nite::SKELETON_TRACKED)
        return false;
    for(int i = 0; i < TOTAL_JOINTS; i++){
        vectorArray[i].resize(3);
        vectorArray[i].zero();
        if(vectorArray != NULL) vectorArray[i] = skeletonData->getPosition(userID)[i];
        if(confidence != NULL) confidence[i] = skeletonData->getPositionConf(userID)[i];
    }
    return true;
}

int *yarp::dev::OpenNI2DeviceDriverClient::getSkeletonState(){
    int *userState = new int[MAX_USERS];
    for(int i = 0; i < MAX_USERS; i++){
        userState[i] = getSkeletonState(i);
    }
    return userState;
}

int yarp::dev::OpenNI2DeviceDriverClient::getSkeletonState(int userID){
    return skeletonData->getSkeletonState(userID);
}

ImageOf<PixelRgb> yarp::dev::OpenNI2DeviceDriverClient::getImageFrame(){
    return skeletonData->getImageFrame();
}

ImageOf<PixelMono16> yarp::dev::OpenNI2DeviceDriverClient::getDepthFrame(){
    return skeletonData->getDepthFrame();
}

