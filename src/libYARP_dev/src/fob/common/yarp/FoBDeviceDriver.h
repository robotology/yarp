/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __FoBDeviceDriverh__
#define __FoBDeviceDriverh__

#include <stdlib.h>
#include <string.h>

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
        class FoBDeviceDriver;
    }
}

struct FoBData {

	FoBData(){ clean(); };
	
	void clean() {
		x = y = z = azimuth = elevation = roll = 0.0;
	};

	double x;
	double y;
	double z;
	double azimuth;
	double elevation;
	double roll;

};

struct FoBOpenParameters {
	FoBOpenParameters() {
		nGroupID = 0;
		comPort = 1;
		baudRate = 115200;
		timeOut = 160;
		measurementRate = 103.3;
		transOpMode = 2;
	};
	int nGroupID;
	unsigned short comPort;
	unsigned int baudRate;
	unsigned int timeOut;
	double measurementRate;
	char transOpMode;
	// TODO: manage flocks with multiple birds...
	// bool standalone;
	// int nDevices;
};

class yarp::dev::FoBDeviceDriver : public DeviceDriver {
private:
	FoBDeviceDriver(const FoBDeviceDriver&);
	void operator=(const FoBDeviceDriver&);

public:
	FoBDeviceDriver();
	virtual ~FoBDeviceDriver();

	// overload open, close
	virtual bool open(yarp::os::Searchable& config);
	virtual bool close();

	virtual int getData(void *ret);
	virtual int startStreaming();
	virtual int stopStreaming ();

protected:
	void *system_resources;

};

#endif
