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

using namespace std;
using namespace yarp::os;

/**
* Generic Yarp class that initializes two ports and cycle. Needs PortCtrlMod to be initialized.
*/
class GenericYarpDriver
{
public:
	GenericYarpDriver();
	GenericYarpDriver(string receivingPortName, string sendingPortName);
	~GenericYarpDriver(void);
	BufferedPort<Bottle> *getReceivePort();
	BufferedPort<Bottle> *getSendPort();
	/**
	* Callback from a input port (receivingPortName) event
	*
	* @param bottle from the port
	*/
	virtual void onRead(Bottle &bot) = 0;
	/**
	* Infinite cycle
	*
	* @return false stops the cycle
	*/
	virtual bool updateInterface() = 0;
	/**
	* Gets a bottle from the command line
	* 
	* @param command from the command line
	* @param reply to be filled with data to be sent to the std::cout
	*/
	virtual bool shellRespond(const Bottle& command, Bottle& reply) = 0;
	/**
	* Executed when the ctr-c event is caught
	*/
	virtual bool close() = 0;
	/**
	* create the input e output ports
	*/
	void setupPorts(string receivingPortName, string sendingPortName);
protected:
	BufferedPort<Bottle> *_sendingPort;
	BufferedPort<Bottle> *_receivingPort;
};
