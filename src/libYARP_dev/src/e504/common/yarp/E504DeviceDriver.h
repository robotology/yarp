/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __E504DeviceDriverh__
#define __E504DeviceDriverh__

#include<iostream>

using namespace std;

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

struct E504Data {
	
	E504Data() { clean(); };
	
	void clean() {
		valid = false;
		pupilX = pupilY = 0.0;
		targetPoints[0] = 0.0;
		targetPoints[1] = 0.0;
		targetPoints[2] = 0.0;
		targetPoints[3] = 0.0;
	};

	bool valid;
	double pupilX;
	double pupilY;
	double targetPoints[4];

};

// definitions for serial ports
#include <atlbase.h>
// Definition of ASLControllerPort COM server
#import "../winnt/dd_orig/lib/ASLControllerLib.tlb" no_namespace raw_interfaces_only

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
        class E504DeviceDriver;
    }
}

class E504Resources
{
public:
	E504Resources () {
		connected = false;
		sceneUpLeftX = sceneUpLeftY = sceneDnRightX = sceneDnRightY = 0;
	};

	bool connected;
	long sceneUpLeftX, sceneUpLeftY, sceneDnRightX, sceneDnRightY;

	CComPtr<IASLControllerPort> E504ControllerPort;

};

struct E504OpenParameters {
	E504OpenParameters() {
		comPort = 1;
		baudRate = 57600;
	};

	unsigned short comPort;
	unsigned int baudRate;

};

class yarp::dev::E504DeviceDriver : public DeviceDriver {

private:
	E504DeviceDriver(const E504DeviceDriver&);
	void operator=(const E504DeviceDriver&);

public:
	E504DeviceDriver();
	virtual ~E504DeviceDriver();

	// overload open, close
	virtual bool open(yarp::os::Searchable& config);
	virtual bool close();
	virtual bool getData(E504Data*);

protected:
	void *system_resources;

};

#endif
