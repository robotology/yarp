/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __CyberGloveDeviceDriverh__
#define __CyberGloveDeviceDriverh__

#include <yarp/dev/DeviceDriver.h>

struct CyberGloveData {

	CyberGloveData(){ clean(); };

	void clean() {
		thumb[0]=0;  thumb[1]=0;  thumb[2]=0;
		index[0]=0;  index[1]=0;  index[2]=0;
		middle[0]=0; middle[1]=0; middle[2]=0;
		ring[0]=0;   ring[1]=0;   ring[2]=0;
		pinkie[0]=0; pinkie[1]=0; pinkie[2]=0;
		abduction[0]=0; abduction[1]=0; abduction[2]=0; abduction[3]=0;
		palmArch=0;	wristPitch=0; wristYaw=0;
	};

	int thumb[3];	// [rotation, inner, outer]
	int index[3];	// [inner, middle, outer phalanx]
	int middle[3];	// [inner, middle, outer phalanx]
	int ring[3];	// [inner, middle, outer phalanx]
	int pinkie[3];	// [inner, middle, outer phalanx]
	int abduction[4];	// [thumb-index, index-middle, middle-ring, ring-pinkie relative abductions]
	int palmArch;	// palm arch
	int wristPitch;	// wrist pitch
	int wristYaw;	// wrist yaw

};

namespace yarp {
    namespace dev {
        class CyberGloveDeviceDriver;
    }
}

struct CyberGloveOpenParameters {

	unsigned short comPort;
	unsigned int baudRate;

};

class yarp::dev::CyberGloveDeviceDriver : public DeviceDriver,  public yarp::os::Thread {
private:
	CyberGloveDeviceDriver(const CyberGloveDeviceDriver&);
	void operator=(const CyberGloveDeviceDriver&);

public:
	CyberGloveDeviceDriver();
	virtual ~CyberGloveDeviceDriver();

	virtual bool open(yarp::os::Searchable& config);
	virtual bool close();
	virtual void run ();

	virtual bool startStreaming();
	virtual bool stopStreaming();

	virtual bool getData(CyberGloveData*);
	virtual bool getSwitch(int*);
	virtual bool getLed(int*);
	virtual bool setLed(int*);
	virtual bool resetGlove();	

protected:
	void *system_resources;

};

#endif
