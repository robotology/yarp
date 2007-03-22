/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <iostream>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include <yarp/FoBDeviceDriver.h>
#include "../dd_orig/include/Bird.h"

using namespace yarp::dev;
using namespace std;

class FoBResources {
public:
	FoBResources (void) {
		groupID = 0;
		standalone = true;
		nDevices = 1;
		posScaling = 0;
		xMapping = 1;
		yMapping = -1;
		zMapping = -1;
		streamingStarted = false;
		connected = false;
	};

	int groupID;
	bool standalone;
	int nDevices;
	double posScaling;
	int xMapping;
	int yMapping;
	int zMapping;
	bool streamingStarted;
	bool connected;

};

inline FoBResources& RES(void *res) { return *(FoBResources*)res; }

FoBDeviceDriver::FoBDeviceDriver() 
{

	system_resources = (void*) new FoBResources;

}

FoBDeviceDriver::~FoBDeviceDriver()
{

	if (system_resources != NULL)
		delete (FoBResources*)system_resources;

	system_resources = NULL;

}


bool FoBDeviceDriver::open(yarp::os::Searchable& config)
{

	FoBResources& d = RES(system_resources);

	unsigned short comPort = config.find("comPort").asInt();

	d.groupID = config.find("groupID").asInt();
	if ( ! birdRS232WakeUp(d.groupID, d.standalone, d.nDevices, &comPort,
                           config.find("baudRate").asInt(),config.find("timeOut").asInt(),
						   config.find("timeOut").asInt()) ) {
		return false;
	}
	
	// set measurement rate
	BIRDSYSTEMCONFIG birdSysConfig;
	if ( ! birdGetSystemConfig(d.groupID,&birdSysConfig) )
		return false;
	birdSysConfig.dMeasurementRate = config.find("measurementRate").asInt();
	if ( ! birdSetSystemConfig(d.groupID,&birdSysConfig) )
		return false;

	// set transmitter operation mode
	BYTE pbuffer[3] = { 'P', 18, config.find("transOpMode").asInt() };
	if ( ! birdRS232SendCommand(d.groupID, 0, (void*)pbuffer, 3) )
		return false;

	// gather scaling; set connected flag to true
	BIRDDEVICECONFIG birdDevConfig;
	if ( ! birdGetDeviceConfig(d.groupID,0,&birdDevConfig) )
		return false;
	d.posScaling = birdDevConfig.wScaling;
	d.connected = true;

	// ok, bail out
	return true;	

}

bool FoBDeviceDriver::close (void)
{

	FoBResources& d = RES(system_resources);
	
	if (d.streamingStarted)
		stopStreaming();

	if (d.connected)
		birdShutDown(d.groupID);
	
	d.connected = false;

	return true;

}

int FoBDeviceDriver::getData (void *cmd)
{

	FoBResources& d = RES(system_resources);

	if (!d.connected)
		return false;

	BIRDFRAME dataFrame;
	BIRDREADING *birdReading;
	
	if (!d.streamingStarted) {
		birdStartSingleFrame(d.groupID);
		birdGetFrame(d.groupID,&dataFrame);
	} else {
		birdGetMostRecentFrame(d.groupID,&dataFrame);
	}
	
	birdReading = &dataFrame.reading[0];

	struct FoBData * data = (struct FoBData *)cmd;
	data->x = birdReading->position.nX * d.posScaling / 32767.0 * d.xMapping;
	data->y = birdReading->position.nY * d.posScaling / 32767.0 * d.yMapping;
	data->z = birdReading->position.nZ * d.posScaling / 32767.0 * d.zMapping;
	data->azimuth = birdReading->angles.nAzimuth * 180.0 / 32767.0;
	data->elevation = birdReading->angles.nElevation * 180.0 / 32767.0;
	data->roll = birdReading->angles.nRoll * 180.0 / 32767.0;

	return true;

}

int FoBDeviceDriver::startStreaming ()
{

	FoBResources& d = RES(system_resources);
	int ret;
	
	ret = birdStartFrameStream(d.groupID);
	
	if (!ret)
		return false;
	
	d.streamingStarted = true;
	
	return true;

}

int FoBDeviceDriver::stopStreaming ()
{
	
	FoBResources& d = RES(system_resources);
	
	d.streamingStarted = false;
	
	birdStopFrameStream(d.groupID);

	return true;

}
