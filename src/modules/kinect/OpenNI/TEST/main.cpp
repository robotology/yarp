/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/all.h>
//#include "../IKinectDevice/IKinectDeviceDriver.h"
#include <yarp/dev/IKinectDeviceDriver.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/RFModule.h>
#include <yarp/sig/all.h>
//#include "../KinectYarpDeviceServerLib/KinectDeviceDriverServer.h"
//#include "../KinectYarpDeviceClientLib/KinectDeviceDriverClient.h"
#include "GLWindow.h"
#include <string>

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;

class KinectThread : public Thread, public RFModule{
	IKinectDeviceDriver *_kinect;
	GLWindow *_glWindow;
	BufferedPort<ImageOf<PixelInt> > imgPort;
public:
	KinectThread(IKinectDeviceDriver *kinect, GLWindow *glWindow){
		_kinect = kinect;
		_glWindow = glWindow;
		imgPort.open("/depthImg");
	}

	void run(){
		ResourceFinder dummyResource;
		this->runModule(dummyResource);
	};
	double getPeriod(){return 0.0;};
	bool updateModule(){
		static int userLastStatus[MAX_USERS] = {-1};

		for(int userID = 0; userID < MAX_USERS; userID++){
			if(userLastStatus[userID] != _kinect->getSkeletonState(userID)){
				userLastStatus[userID] = _kinect->getSkeletonState(userID);
				printf("USER %d HAS STATUS %d\n",userID,userLastStatus[userID]);
			}
			Matrix skeletonOrientation[TOTAL_JOINTS];
			Vector skeletonPosition[TOTAL_JOINTS];
			double confPosition[TOTAL_JOINTS], confOrientation[TOTAL_JOINTS];
			if(!_kinect->getSkeletonOrientation(skeletonOrientation,confOrientation,userID)) continue;
			_kinect->getSkeletonPosition(skeletonPosition,confPosition,userID);
			_glWindow->setData(userID,userLastStatus[userID],skeletonOrientation, confPosition,skeletonPosition,confOrientation);
			_glWindow->redraw();
		}

		ImageOf<PixelInt> img = _kinect->getDepthMap();
		imgPort.prepare() = img;
		imgPort.write();
		return true;
	};
	bool configure(yarp::os::ResourceFinder &rf){
		return true;
	};
};

int main(int argc, char *argv[]){

	Network yarp;
	if(!yarp.checkNetwork()){
		printf("Yarp network failed!\n");
		return -1;
	}

	/*DriverCreator *kinect_factoryClient = new DriverCreatorOf<yarp::dev::KinectDeviceDriverClient>("KinectDeviceClient","","");
	DriverCreator *kinect_factoryServer = new DriverCreatorOf<yarp::dev::KinectDeviceDriverServer>("KinectDeviceServer","","");
	Drivers::factory().add(kinect_factoryClient);
	Drivers::factory().add(kinect_factoryServer);*/

	//Property config("(device KinectDeviceClient) (remotePortPrefix /kinect) (localPortPrefix /kinectSkeletonClient) (userDetection)");
	//Property config("(device KinectDeviceLocal) (portPrefix /kinectSkeletonServer) (openPorts)");
	Property config("(device KinectDeviceLocal) (portPrefix /kinectSkeletonServer) (userDetection)");
	//Property config("(device KinectDeviceLocal) (portPrefix /kinectSkeletonServer)");
	PolyDriver dd(config);
	IKinectDeviceDriver *grabber;
	dd.view(grabber);
	if (grabber==NULL) { return 0; } // failed

	GLWindow *glWindow = new GLWindow(argc,argv);

	KinectThread kinectThread(grabber,glWindow);
	kinectThread.start();

	glWindow->runGLWindow();

	return 1;
}

