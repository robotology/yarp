/*
 * Copyright (C) 2011 Duarte Aragao
 * Copyright (C) 2013 Konstantinos Theofilis, University of Hertfordshire, k.theofilis@herts.ac.uk
 * Authors: Duarte Aragao, Konstantinos Theofilis
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "OpenNI2SkeletonData.h"

OpenNI2SkeletonData::OpenNI2SkeletonData()
{
    initUserSkeletons();
}

OpenNI2SkeletonData::~OpenNI2SkeletonData()
{
}

void OpenNI2SkeletonData::initUserSkeletons(){
    userSkeleton = new UserSkeleton[MAX_USERS];
    UserSkeleton *tuserSkeleton;
    for(int i = 0; i < MAX_USERS; i++){
        tuserSkeleton = &(userSkeleton[i]);
        for(int j = 0; j < TOTAL_JOINTS; j++){
            tuserSkeleton->skeletonPointsOri[j].resize(4);
            tuserSkeleton->skeletonPointsOri[j].zero();
            tuserSkeleton->skeletonPointsPos[j].resize(3);
            tuserSkeleton->skeletonPointsPos[j].zero();
            tuserSkeleton->skeletonPosConf[j] = -1;
            tuserSkeleton->skeletonOriConf[j] = -1;
#ifdef OPENNI2_DRIVER_USES_NITE2
            tuserSkeleton->skeletonState = nite::SKELETON_NONE;
#endif
            tuserSkeleton->uID = i+1;
        }
    }
}

void OpenNI2SkeletonData::storeData(ImageOf<PixelRgb>& img){
    imageFrame = img;
}

void OpenNI2SkeletonData::storeData(ImageOf<PixelMono16>& img){
    depthFrame = img;
}

void OpenNI2SkeletonData::storeData(Bottle& b){
    Bottle *list;
    int userID;
    UserSkeleton *tuserSkeleton;
    if(b.get(0).isString()){
        userID = b.get(1).asInt();
        string vocab = b.get(0).asString().c_str();
        if(vocab.compare("CALIBRATING FOR USER") == 0)
        {
#ifdef OPENNI2_DRIVER_USES_NITE2
            userSkeleton[userID-1].skeletonState = nite::SKELETON_CALIBRATING;
#endif
        }
    }
    
    else if(b.get(0).isList()){
        list = b.get(0).asList();
        userID = list->get(1).asInt();
#ifdef OPENNI2_DRIVER_USES_NITE2
        userSkeleton[userID-1].skeletonState = nite::SKELETON_TRACKED;//USER STATUS
#endif
        tuserSkeleton = &(userSkeleton[userID-1]);
        int jointIndex = 0;
        for(int i = 1; i < b.size(); i+=6){
            list = b.get(i+1).asList();// position elements
            tuserSkeleton->skeletonPointsPos[jointIndex][0] = list->get(0).asDouble();
            tuserSkeleton->skeletonPointsPos[jointIndex][1] = list->get(1).asDouble();
            tuserSkeleton->skeletonPointsPos[jointIndex][2] = list->get(2).asDouble();
            tuserSkeleton->skeletonPosConf[jointIndex] = b.get(i+2).asDouble();
            list = b.get(i+4).asList();// orientation elements
            tuserSkeleton->skeletonPointsOri[jointIndex][0] = list->get(0).asDouble();
            tuserSkeleton->skeletonPointsOri[jointIndex][1] = list->get(1).asDouble();
            tuserSkeleton->skeletonPointsOri[jointIndex][2] = list->get(2).asDouble();
            tuserSkeleton->skeletonPointsOri[jointIndex][3] = list->get(3).asDouble();
            tuserSkeleton->skeletonOriConf[jointIndex] = b.get(i+5).asDouble();
            jointIndex++;
        }
    }
}

Vector* OpenNI2SkeletonData::getOrientation(int userID){
    return userSkeleton[userID-1].skeletonPointsOri;
}

Vector* OpenNI2SkeletonData::getPosition(int userID){
    return userSkeleton[userID-1].skeletonPointsPos;
}

float* OpenNI2SkeletonData::getOrientationConf(int userID){
    return userSkeleton[userID-1].skeletonOriConf;
}

float* OpenNI2SkeletonData::getPositionConf(int userID){
    return userSkeleton[userID-1].skeletonPosConf;
}

#ifdef OPENNI2_DRIVER_USES_NITE2
nite::SkeletonState OpenNI2SkeletonData::getSkeletonState(int userID){
    return userSkeleton[userID-1].skeletonState;
}
#endif

ImageOf<PixelMono16> OpenNI2SkeletonData::getDepthFrame(){
    return depthFrame;
}

ImageOf<PixelRgb> OpenNI2SkeletonData::getImageFrame(){
    return imageFrame;
}

