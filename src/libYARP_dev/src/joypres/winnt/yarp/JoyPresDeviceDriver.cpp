	/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <iostream>

#include <yarp/JoyPresDeviceDriver.h>

#include "../dd_orig/include/Joystick.h"

using namespace yarp::dev;
using namespace std;

class JoyPresResources {
public:

	CJoystick _sensor;
	JoyData _data;
	int _sensN;

};

inline JoyPresResources& RES(void *res) { return *(JoyPresResources *)res; }

JoyPresDeviceDriver::JoyPresDeviceDriver()
{

	system_resources = (void *) new JoyPresResources;

}

JoyPresDeviceDriver::~JoyPresDeviceDriver()
{
	
	if (system_resources != NULL)
		delete (JoyPresResources *)system_resources;

	system_resources = NULL;

}

bool JoyPresDeviceDriver::open(yarp::os::Searchable& config)
{
	
	JoyPresResources& d = RES(system_resources);

	d._data.x = 0;
	d._data.y = 0;
	d._data.z = 0;
	d._data.u = 0;
	d._data.buttons = 0;

    return true;

}

bool JoyPresDeviceDriver::close()
{ 

    return true;

}

bool JoyPresDeviceDriver::getData(JoyPresData* data)
{
	
	JoyPresResources& d = RES(system_resources);
	
	bool bRes = d._sensor.getRawData(&d._data);

	data->channelA = d._data.x;
	data->channelB = d._data.y;
	data->channelC = d._data.z;
	data->channelD = d._data.u;

	if (bRes)
		return true;
	else
		return false;

}
