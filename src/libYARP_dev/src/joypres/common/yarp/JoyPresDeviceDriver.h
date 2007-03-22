/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __JoyPresDeviceDriverh__
#define __JoyPresDeviceDriverh__

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
        class JoyPresDeviceDriver;
    }
}

struct JoyPresData {
	JoyPresData() { clean(); };
	void clean() {
		channelA = channelB = channelC = channelD = 0;
	};

	int channelA;
	int channelB;
	int channelC;
	int channelD;

};

class yarp::dev::JoyPresDeviceDriver : public DeviceDriver {
private:
	JoyPresDeviceDriver(const JoyPresDeviceDriver&);
	void operator=(const JoyPresDeviceDriver&);
public:
    JoyPresDeviceDriver() ;
	virtual ~JoyPresDeviceDriver();

    virtual bool open(yarp::os::Searchable& config) ;
    virtual bool close() ;
	virtual bool getData(JoyPresData*);

protected:
	 void *system_resources;
};

#endif
