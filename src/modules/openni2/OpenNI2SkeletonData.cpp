// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
            tuserSkeleton->skeletonState = nite::SKELETON_NONE;
            tuserSkeleton->uID = 0;
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
        userSkeleton[userID].uID = userID;
        string vocab = b.get(0).asString().c_str();
        if(vocab.compare("CALIBRATING FOR USER") == 0){
            userSkeleton[userID].skeletonState = nite::SKELETON_CALIBRATING;
        }
        else if(vocab.compare("CALIBRATION ERROR FOR USER") == 0){
            userSkeleton[userID].skeletonState = nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE;}
        else if(vocab.compare("LOST SKELETON FOR USER") == 0){
            userSkeleton[userID].skeletonState = nite::SKELETON_NONE;
        }
    }else if(b.get(0).isList()){
        list = b.get(0).asList();
        userID = list->get(1).asInt();
        userSkeleton[userID].uID = userID;
        userSkeleton[userID].skeletonState = nite::SKELETON_TRACKED;//USER STATUS
        tuserSkeleton = &(userSkeleton[userID]);
        for(int i = 1; i < b.size(); i+=6){
            int jointIndex = (i-1)/6;
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
        }
    }
}

Vector* OpenNI2SkeletonData::getOrientation(int userID){
    return userSkeleton[userID].skeletonPointsOri;
}

Vector* OpenNI2SkeletonData::getPosition(int userID){
    return userSkeleton[userID].skeletonPointsPos;
}

double* OpenNI2SkeletonData::getOrientationConf(int userID){
    return userSkeleton[userID].skeletonOriConf;
}

double* OpenNI2SkeletonData::getPositionConf(int userID){
    return userSkeleton[userID].skeletonPosConf;
}

int OpenNI2SkeletonData::getSkeletonState(int userID){
    return userSkeleton[userID].skeletonState;
}

ImageOf<PixelMono16> OpenNI2SkeletonData::getDepthFrame(){
    return depthFrame;
}

ImageOf<PixelRgb> OpenNI2SkeletonData::getImageFrame(){
    return imageFrame;
}

