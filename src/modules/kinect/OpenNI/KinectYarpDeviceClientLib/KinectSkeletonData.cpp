/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "KinectSkeletonData.h"

KinectSkeletonData::KinectSkeletonData(void)
{
	initUserSkeletons();
}

KinectSkeletonData::~KinectSkeletonData(void)
{
}

void KinectSkeletonData::initUserSkeletons(){
	_userSkeleton = new UserSkeleton[MAX_USERS];
	UserSkeleton *userSkeleton;
	for(int i = 0; i < MAX_USERS; i++){
		userSkeleton = &(_userSkeleton[i]);
		for(int j = 0; j < TOTAL_JOINTS; j++){
			userSkeleton->skeletonPointsOri[j].resize(3,3);
			userSkeleton->skeletonPointsOri[j].zero();
			userSkeleton->skeletonPointsPos[j].resize(3);
			userSkeleton->skeletonPointsPos[j].zero();
			userSkeleton->skeletonPosConf[j] = -1;
			userSkeleton->skeletonOriConf[j] = -1;
			userSkeleton->skeletonState = NO_USER;
		}
	}
}

void KinectSkeletonData::storeData(ImageOf<PixelRgb>& img){
	//std::cout<<"onRead(ImageOf<PixelRgb>)"<<std::endl;
	_imageMap = img;
}

void KinectSkeletonData::storeData(ImageOf<PixelInt>& img){
	//std::cout<<"onRead(ImageOf<PixelInt>)"<<std::endl;
	_depthMap = img;
}

void KinectSkeletonData::storeData(Bottle& b){
	Bottle *list;
	int userID;
	UserSkeleton *userSkeleton;
	if(b.get(0).isString()){
		userID = b.get(1).asInt();
		string vocab = b.get(0).asString().c_str();
		if(vocab.compare(USER_CALIBRATING_MSG) == 0){
			_userSkeleton[userID].skeletonState = CALIBRATING;
		}else if(vocab.compare(USER_DETECTED_MSG) == 0){
			_userSkeleton[userID].skeletonState = USER_DETECTED;
		}else if(vocab.compare(USER_LOST_MSG) == 0){
			_userSkeleton[userID].skeletonState = NO_USER;
		}
	}else if(b.get(0).isList()){
		list = b.get(0).asList();
		userID = list->get(1).asInt();
		_userSkeleton[userID].skeletonState = SKELETON_TRACKING;//USER STATUS
		userSkeleton = &(_userSkeleton[userID]);
		for(int i = 1; i < b.size(); i+=6){
			int jointIndex = (i-1)/6;
			list = b.get(i+1).asList();//possition elements
			userSkeleton->skeletonPointsPos[jointIndex][0] = list->get(0).asDouble();
			userSkeleton->skeletonPointsPos[jointIndex][1] = list->get(1).asDouble();
			userSkeleton->skeletonPointsPos[jointIndex][2] = list->get(2).asDouble();
			userSkeleton->skeletonPosConf[jointIndex] = b.get(i+2).asDouble();
			list = b.get(i+4).asList();//orientation elements
			userSkeleton->skeletonPointsOri[jointIndex][0][0] = list->get(0).asDouble();
			userSkeleton->skeletonPointsOri[jointIndex][0][1] = list->get(1).asDouble();
			userSkeleton->skeletonPointsOri[jointIndex][0][2] = list->get(2).asDouble();
			userSkeleton->skeletonPointsOri[jointIndex][1][0] = list->get(3).asDouble();
			userSkeleton->skeletonPointsOri[jointIndex][1][1] = list->get(4).asDouble();
			userSkeleton->skeletonPointsOri[jointIndex][1][2] = list->get(5).asDouble();
			userSkeleton->skeletonPointsOri[jointIndex][2][0] = list->get(6).asDouble();
			userSkeleton->skeletonPointsOri[jointIndex][2][1] = list->get(7).asDouble();
			userSkeleton->skeletonPointsOri[jointIndex][2][2] = list->get(8).asDouble();
			userSkeleton->skeletonOriConf[jointIndex] = b.get(i+5).asDouble();
		}
	}
	//std::cout<<"onRead(Bottle)"<<std::endl;
}

Matrix* KinectSkeletonData::getOrientation(int userID){
	return _userSkeleton[userID].skeletonPointsOri;
}

Vector* KinectSkeletonData::getPosition(int userID){
	return _userSkeleton[userID].skeletonPointsPos;
}

double* KinectSkeletonData::getOrientationConf(int userID){
	return _userSkeleton[userID].skeletonOriConf;
}

double* KinectSkeletonData::getPositionConf(int userID){
	return _userSkeleton[userID].skeletonPosConf;
}

int KinectSkeletonData::getSkeletonState(int userID){
	return _userSkeleton[userID].skeletonState;
}

ImageOf<PixelInt> KinectSkeletonData::getDepthMap(){
	return _depthMap;
}

ImageOf<PixelRgb> KinectSkeletonData::getImageMap(){
	return _imageMap;
}
