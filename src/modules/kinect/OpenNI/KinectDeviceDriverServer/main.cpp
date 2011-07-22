// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include <stdio.h>

#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/dev/drivers.h>

#include "../lib/PortCtrlMod.h"
#include "../KinectYarpDeviceServerLib/KinectDeviceDriverServer.h"

using namespace yarp::dev;
using namespace yarp::os;

int main(int argc, char *argv[]){

	Network yarp;
	if(!yarp.checkNetwork()){
		printf("Yarp network failed!\n");
		return -1;
	}

	PortCtrlMod portMod;

	Property p;
	p.fromCommand(argc,argv);
	ResourceFinder rf;
	//rf.setVerbose();
	//rf.setDefaultContext("../../../../../../../Users/doart3/Documents/Work/ISR/Software/FinalInteractionSoftware/InterfaceYarpDriver/KinectDeviceDriverServer");
	//rf.setDefaultConfigFile("kinectDeviceDriverServer.ini");
	//rf.configure("ICUB_ROOT", argc, argv);

	if(!portMod.configure(rf)){
		printf("ERROR: Bad configuration of the RFModule.");
		return -1;
	}

	string xmlPath;
	string portPrefix;

	if(p.check("portPrefix")) portPrefix = p.find("portPrefix").asString();
	else {
		printf("\t- Error: portPrefix element not found in INI file.\n");
		portMod.close();
		return 0;
	}
	bool userDetection = p.check("userDetection");
	KinectDeviceDriverServer *kinect = new KinectDeviceDriverServer(portPrefix,userDetection);
	portMod.setInterfaceDriver(kinect);

	portMod.runModule();

	return 0;

}