/*
 * Copyright (C) 2007 Claudio Castellini, Mattia Castelnovi
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include<iostream>

using namespace std;

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include <yarp/E504DeviceDriver.h>

// definitions for serial ports
#include <atlbase.h>
// Definition of ASLControllerPort COM server
#import "../dd_orig/lib/ASLControllerLib.tlb" no_namespace raw_interfaces_only

using namespace yarp::dev;
using namespace std;

inline E504Resources& RES(void* res) { return *(E504Resources*)res; }

E504DeviceDriver::E504DeviceDriver(void) {

	system_resources = (void*) new E504Resources;
	ACE_ASSERT (system_resources != NULL);

}

E504DeviceDriver::~E504DeviceDriver()
{

	if (system_resources != NULL) {
		delete (E504Resources *)system_resources;
	}
	system_resources = NULL;

}

bool E504DeviceDriver::open(yarp::os::Searchable& config)
{

	// gather system resources and parameters
	E504Resources& d = RES(system_resources);

	if ( d.connected ) {
		return false;
	}

	// initialize COM environment
	if (FAILED(CoInitialize(NULL))) {
		return false;
	}

	// create COM object
	if (FAILED(d.E504ControllerPort.CoCreateInstance(__uuidof(ASLControllerPort)))) {
		return false;
	}

	// connect me!!
	if ( SUCCEEDED(d.E504ControllerPort->Connect(config.find("comPort").asInt(), config.find("baudRate").asInt())) ) {
		// initialize control unit:
		d.E504ControllerPort->set_illuminator_power_mode(1);
		d.E504ControllerPort->set_tracking_mode(1);
 		d.E504ControllerPort->set_data_acquisition_mode(1);
	} else {
		return false;
	}

	// gather scene target points - used to evaluate scene POG upon getData()
	d.E504ControllerPort->get_scene_target_point_position (1, &d.sceneUpLeftX, &d.sceneUpLeftY);
	d.E504ControllerPort->get_scene_target_point_position (9, &d.sceneDnRightX, &d.sceneDnRightY);

	// ok, bail out
	d.connected = true;
	return true;	

}

bool E504DeviceDriver::close ()
{

	E504Resources& d = RES(system_resources);
	
	if ( ! d.connected ) {
		return false;
	}

	d.E504ControllerPort->set_illuminator_power_mode(0);
	d.E504ControllerPort->set_data_acquisition_mode(0);
	if (FAILED(d.E504ControllerPort->Disconnect())) {
		return false;
	}
	
	d.connected = false;

	// destroy CComPtr object
	d.E504ControllerPort.Detach();

	// uninitialize COM environment
	CoUninitialize();

	return true;

}


bool E504DeviceDriver::getData (E504Data* data)
{

	E504Resources& d = RES(system_resources);

	if ( ! d.connected ) {
		return false;
	}

	data->targetPoints[0] = (double)d.sceneUpLeftX;
	data->targetPoints[1] = (double)d.sceneUpLeftY;
	data->targetPoints[2] = (double)d.sceneDnRightX;
	data->targetPoints[3] = (double)d.sceneDnRightY;

	ASLController_Results results;
	VARIANT_BOOL valid = VARIANT_FALSE;

	// Read Eye Tracker messages in the buffer
	d.E504ControllerPort->updateComm();
	d.E504ControllerPort->get_results(&results, &valid);
	// Extract all messages from the buffer
	while (valid == VARIANT_TRUE) {
		if (results.pupil_diameter != 0 && results.cr_diameter != 0) {
			data->valid = true;
			data->pupilX = (double)results.scene_x / (double)results.gaze_res_factor;
			data->pupilY = (double)results.scene_y / (double)results.gaze_res_factor;
		} else {
			data->valid = false;
			data->pupilX = 0.0;
			data->pupilY = 0.0;
		}
		d.E504ControllerPort->get_results(&results, &valid);
	}

	return true;

}
