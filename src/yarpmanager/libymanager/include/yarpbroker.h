// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __YARPBROKER__
#define __YARPBROKER__

#include <string>
#include <sstream>
#include <iostream>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>

#include "broker.h" 

using namespace std; 
//using namespace yarp::os;
 
//namespace ymm {


/**
 * Class Broker  
 */
class YarpBroker: public Broker{

public: 
	YarpBroker();
	virtual ~YarpBroker();
	virtual bool init();
	virtual bool init(const char* szcmd, const char* szparam,
	        const char* szhost, const char* szstdio,
	        const char* szworkdir, const char* szenv );
	virtual bool run();
	virtual bool stop();
	virtual bool kill();
	virtual bool connect(const char* from, const char* to, 
						const char* carrier);
	virtual bool disconnect(const char* from, const char* to);
	virtual bool running(void);
	virtual bool exists(const char* port);
	virtual bool connected(const char* from, const char* to);
	virtual const char* error(void);
	
	
protected:

private:
	string strCmd;
	string strParam;
	string strHost;
	string strStdio;
	string strWorkdir;
	string strTag;
	string strEnv;
	unsigned int ID;
	yarp::os::Property command;
	string strError;	
	bool bOnlyConnector;
	bool bInitialized;
	
	bool timeout(double base, double timeout);
	yarp::os::Property& runProperty(void);		
	int requestServer(yarp::os::Property& config);
	yarp::os::Bottle SendMsg(yarp::os::Bottle& msg, yarp::os::ConstString target);

};
 
//}

#endif //__YARPBROKER__
