// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __BROKER__
#define __BROKER__

#include <string>
#include <vector>
#include "ymm-types.h" 

using namespace std; 

//namespace ymm {


/**
 * Class Broker  
 */
class Broker {

public: 
	Broker(void);
	~Broker();
	virtual bool init(void) = 0; //only connector 
	virtual bool init(const char* szcmd, const char* szparam,
	        const char* szhost, const char* szstdio,
	        const char* szworkdir, const char* szenv ) = 0;
	virtual bool run() = 0;
	virtual bool stop() = 0;
	virtual bool kill() = 0;
	virtual bool connect(const char* from, const char* to, 
						const char* carrier) = 0;
	virtual bool disconnect(const char* from, const char* to) = 0;
	virtual bool running(void) = 0;
	virtual bool exists(const char* port) = 0;
	virtual bool connected(const char* from, const char* to) = 0;
	virtual const char* error(void) = 0;
	unsigned int generateID(void);
	
protected:
	static unsigned int UNIQUEID; 
private:

};
 
//}

#endif //__BROKER__
