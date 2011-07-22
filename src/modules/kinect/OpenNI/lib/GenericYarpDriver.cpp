// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "GenericYarpDriver.h"

GenericYarpDriver::GenericYarpDriver(){}

GenericYarpDriver::GenericYarpDriver(string receivingPortName, string sendingPortName)
{
	setupPorts(receivingPortName,sendingPortName);
}

GenericYarpDriver::~GenericYarpDriver(void)
{
}

BufferedPort<Bottle> *GenericYarpDriver::getReceivePort(){
	return _receivingPort;
}

BufferedPort<Bottle> *GenericYarpDriver::getSendPort(){
	return _sendingPort;
}

void GenericYarpDriver::setupPorts(string receivingPortName, string sendingPortName){
	_sendingPort = new BufferedPort<Bottle>();
	_sendingPort->open(sendingPortName.c_str());
	_receivingPort = new BufferedPort<Bottle>();
	_receivingPort->open(receivingPortName.c_str());
}
