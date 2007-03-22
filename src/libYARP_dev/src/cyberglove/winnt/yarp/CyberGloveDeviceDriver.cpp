/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include <yarp/os/all.h>
#include <yarp/os/Semaphore.h>

#include <yarp/dev/Drivers.h>

using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;

#include <yarp/CyberGloveDeviceDriver.h>
#include "../dd_orig/include/Serial.h"

#include <iostream>

using namespace std;

class CyberGloveResources {
public:

	CyberGloveResources (void) : _bmutex(1) {
		_bStreamStarted = false;
		_bError = false;
	}

	CSerial _serialPort;
	CyberGloveData _data;

	bool _bStreamStarted;
	bool _bError;

	yarp::os::Semaphore _bmutex;

	void _fillDataStructure(unsigned char *inBytes, CyberGloveData *destStruct);

};

void CyberGloveResources::_fillDataStructure(unsigned char *inBytes, CyberGloveData *destStruct)
{

	// fill data structure with bytes read off the serial port.

	// NOTES:
	//
	// (1) inBytes is an array of 24 bytes, but:
	// (2) inBytes[0] must be skipped (it is the echoed 'G' command) and so
	//     must be inBytes[23] (it is the terminating 0)
	// (3) as a consequence of this, valid data bytes are inBytes[1] to inBytes[22]
	// (4) the manual seems to fail in the description of the data bytes. the
	//     correct order is the one reported below.

	// ---------- thumb
	destStruct->thumb[0] = inBytes[1]; // rotation across palm
	destStruct->thumb[1] = inBytes[2]; // inner phalanx
	destStruct->thumb[2] = inBytes[3]; // outer phalanx
	// ---------- fingers (inner, middle outer phalanx for each finger)
	// index
	destStruct->index[0] = inBytes[5];
	destStruct->index[1] = inBytes[6];
	destStruct->index[2] = inBytes[7];
	// middle
	destStruct->middle[0] = inBytes[8];
	destStruct->middle[1] = inBytes[9];
	destStruct->middle[2] = inBytes[10];
	// ring
	destStruct->ring[0] = inBytes[12];
	destStruct->ring[1] = inBytes[13];
	destStruct->ring[2] = inBytes[14];
	// pinkie
	destStruct->pinkie[0] = inBytes[16];
	destStruct->pinkie[1] = inBytes[17];
	destStruct->pinkie[2] = inBytes[18];
	// ---------- finger-finger abductions
	destStruct->abduction[0] = inBytes[4];  // thumb-index relative abduction
	destStruct->abduction[1] = inBytes[11]; // index-middle relative abduction
	destStruct->abduction[2] = inBytes[15]; // middle-ring abduction
	destStruct->abduction[3] = inBytes[19]; // ring-pinkie abduction
	// ---------- palm arch
	destStruct->palmArch = inBytes[20];
	// ---------- wrist pitch
	destStruct->wristPitch = inBytes[21];
	// ---------- wrist yaw
	destStruct->wristYaw = inBytes[22];

}

inline CyberGloveResources& RES(void *res) { return *(CyberGloveResources *)res; }

CyberGloveDeviceDriver::CyberGloveDeviceDriver()
{

	system_resources = (void *) new CyberGloveResources;
	ACE_ASSERT (system_resources != NULL);

}

CyberGloveDeviceDriver::~CyberGloveDeviceDriver()
{

	if (system_resources != NULL) {
		delete (CyberGloveResources *)system_resources;
	}

	system_resources = NULL;

}

bool CyberGloveDeviceDriver::open(yarp::os::Searchable& config)
{

	CyberGloveOpenParameters par;
	
	CyberGloveResources& d = RES(system_resources);

	par.comPort =	config.find("comPort").asInt();
	par.baudRate =	config.find("baudRate").asInt();

	CSerial::EBaudrate rate;
	LONG lLastError;

	switch ( par.comPort ) {
	case 1:
		lLastError = d._serialPort.Open("COM1");
		break;
	case 2:
		lLastError = d._serialPort.Open("COM2");
		break;
	case 3:
		lLastError = d._serialPort.Open("COM3");
		break;
	case 4:
		lLastError = d._serialPort.Open("COM4");
		break;
	case 5:
		lLastError = d._serialPort.Open("COM5");
		break;
	case 6:
		lLastError = d._serialPort.Open("COM6");
		break;
	case 7:
		lLastError = d._serialPort.Open("COM7");
		break;
	case 8:
		lLastError = d._serialPort.Open("COM8");
		break;
	case 9:
		lLastError = d._serialPort.Open("COM9");
		break;
	default:
		return false;
		break;
	}
	
	if (lLastError != ERROR_SUCCESS) {
		return false;
	}

	switch( par.baudRate ) {
	case 110:
		rate = CSerial::EBaud110;
		break;
	case 300:
		rate = CSerial::EBaud300;
		break;
	case 600:
		rate = CSerial::EBaud600;
		break;
	case 1200:
		rate = CSerial::EBaud1200;
		break;
	case 2400:
		rate = CSerial::EBaud2400;
		break;
	case 4800:
		rate = CSerial::EBaud4800;
		break;
	case 9600:
		rate = CSerial::EBaud9600;
		break;
	case 14400:
		rate = CSerial::EBaud14400;
		break;
	case 19200:
		rate = CSerial::EBaud19200;
		break;
	case 38400:
		rate = CSerial::EBaud38400;
		break;
	case 56000:
		rate = CSerial::EBaud56000;
		break;
	case 57600:
		rate = CSerial::EBaud57600;
		break;
	case 115200:
		rate = CSerial::EBaud115200;
		break;
	default:
		return false;
		break;
	}

	// set up 8,N,1 hardware handshaking communciation
	if ( d._serialPort.Setup(rate,CSerial::EData8,CSerial::EParNone, CSerial::EStop1) != ERROR_SUCCESS ) {
		return false;
	}
	if ( d._serialPort.SetupHandshaking(CSerial::EHandshakeHardware) != ERROR_SUCCESS ) {
		return false;
	}

	// use blocking method, in order not to lose any data
	if ( d._serialPort.SetupReadTimeouts(CSerial::EReadTimeoutBlocking) != ERROR_SUCCESS ) {
		return false;
	}

	// reset the glove and bail out
	return resetGlove();

}

bool CyberGloveDeviceDriver::close ()
{

	CyberGloveResources& d = RES(system_resources);

	if ( d._serialPort.Close() != ERROR_SUCCESS ) {
		return false;
	}

	return true;

}

void CyberGloveDeviceDriver::run ()
{

	CyberGloveResources& d = RES(system_resources);
	
	// gather streamed data
	unsigned char response[24];
	while ( ! isStopping() ) {
		d._serialPort.Read(response,24);
		d._bmutex.wait();
		d._fillDataStructure(response, &(d._data));
		d._bmutex.post();
	}

}

bool CyberGloveDeviceDriver::startStreaming ()
{

	CyberGloveResources& d = RES(system_resources);
	
	// send streaming command
	unsigned char command1[1] = { 'S' };
	d._serialPort.Write(command1,1);

	// start thread and set flag accordingly
	d._bStreamStarted = true;
	start();

	return true;

}

bool CyberGloveDeviceDriver::stopStreaming ()
{

	CyberGloveResources& d = RES(system_resources);

	// FIXUP: the "terminate streaming" command, ctrl-c, is sent and processed
	// correctly, but it is not clear how to gather the response string
	// (there is still garbage in the serial buffer before the response).
	// so we ignore the response check and return success.
	// lines which won't work are commented out.

	// stop stream gathering thread and set flag accordingly
	stop();
	d._bStreamStarted = false;
	
	// send stream termination command (control-c, ASCII code 3)
	// expect to receive echo of termination command [3,NUL]
	unsigned char command[1] = { 3 };
//	unsigned char response[2];
//	unsigned char expected[2] = { 3, 0 };
	d._serialPort.Purge();
	d._serialPort.Write(command,1);
//	d._serialPort.Read(response,2);
	d._serialPort.Purge();
//	if ( memcmp(response,expected,2) != 0 ) {
//		return false;
//	}

	return true;

}

bool CyberGloveDeviceDriver::getData (CyberGloveData* data)
{

	CyberGloveResources& d = RES(system_resources);

	if (d._bStreamStarted) {
		// if data is requested for while streaming is active, must
		// ensure we respect the semaphore
		d._bmutex.wait();
		*data = d._data;
		d._bmutex.post();
	} else {
		// otherwise, send a G command and read values
		unsigned char command[1] = { 'G' };
		unsigned char response[24];
		d._serialPort.Purge();
		d._serialPort.Write(command,1);
		d._serialPort.Read(response,24);
		d._serialPort.Purge();
		if ( response[23] != 0 ) {
			return false;
		}
		d._fillDataStructure(response, data);
	}

	if (d._bError) {
		return false;
	} else {
		return true;
	}

}

bool CyberGloveDeviceDriver::getSwitch(int* status)
{

	CyberGloveResources& d = RES(system_resources);

	if (d._bStreamStarted) {
		return false;
	}

	// send switch status query command and gather the result
	unsigned char command[2] = { '?', 'W' };
	unsigned char response[4];
	d._serialPort.Purge();
	d._serialPort.Write(command,2);
	d._serialPort.Read(response,4);
	d._serialPort.Purge();
	*status = response[2];

	return true;

}

bool CyberGloveDeviceDriver::getLed(int* status)
{

	CyberGloveResources& d = RES(system_resources);

	if (d._bStreamStarted) {
		return false;
	}

	unsigned char command[2] = { '?', 'L' };
	unsigned char response[4];
	d._serialPort.Purge();
	d._serialPort.Write(command,2);
	d._serialPort.Read(response,4);
	d._serialPort.Purge();
	*status = response[2];

	return true;

}

bool CyberGloveDeviceDriver::setLed(int* status)
{

	CyberGloveResources& d = RES(system_resources);

	if (d._bStreamStarted) {
		return false;
	}

	unsigned char command[2] = { 'L', 0 }; command[1] = *status;
	unsigned char response[2];
	d._serialPort.Purge();
	d._serialPort.Write(command,2);
	d._serialPort.Read(response,2);
	d._serialPort.Purge();

	return true;

}
	
bool CyberGloveDeviceDriver::resetGlove()
{

	CyberGloveResources& d = RES(system_resources);

	// send "restart firmware" command: ASCII 18
	// expect to receive confirmation sequence [18,CR,LF,NUL]
	unsigned char command[1] = { 18 };
	unsigned char response[4];
	unsigned char expected[4] = { 18, 13, 10, 0 };
	d._serialPort.Purge();
	d._serialPort.Write(command,1);
	d._serialPort.Read(response,4);
	d._serialPort.Purge();
	if ( memcmp(response,expected,4) != 0 ) {
		return false;
	}

	return true;

}
