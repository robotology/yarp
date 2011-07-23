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

class GenericYarpDriver
{
public:
	GenericYarpDriver();
	GenericYarpDriver(string receivingPortName, string sendingPortName);
	~GenericYarpDriver(void);
	BufferedPort<Bottle> *getReceivePort();
	BufferedPort<Bottle> *getSendPort();
	virtual void onRead(Bottle &bot) = 0;
	virtual bool updateInterface() = 0;
	virtual bool shellRespond(const Bottle& command, Bottle& reply) = 0;
	virtual bool close() = 0;
	void setupPorts(string receivingPortName, string sendingPortName);
protected:
	BufferedPort<Bottle> *_sendingPort;
	BufferedPort<Bottle> *_receivingPort;
};
