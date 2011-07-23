// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#include <string>

#include <yarp/os/all.h>

#include "GenericYarpDriver.h"

using namespace std;

using namespace yarp::os;

/**
* This class is used to initialize the GenericYarpDriver
*/
class PortCtrlMod: public RFModule, public TypedReaderCallback<Bottle>
{
public:
	PortCtrlMod(bool useCallback = true);
	virtual double getPeriod();
	virtual bool updateModule();
	virtual bool respond(const Bottle& command, Bottle& reply);
	virtual bool configure(ResourceFinder &rf);
	virtual bool interruptModule();
	virtual bool close();
	virtual void onRead(Bottle &b);
	/**
	* Initializes the GenericYarpDriver
	*/
	void setInterfaceDriver(GenericYarpDriver *interfaceDriver);
private:
	BufferedPort<Bottle> *_sendingPort;
	BufferedPort<Bottle> *_receivingPort;
	GenericYarpDriver *_interfaceDriver;
	bool _isInterfaceSet;
	bool _useCallback;
};
