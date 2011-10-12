// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __Executable__
#define __Executable__

#include <string>
#include <vector>
#include "ymm-types.h" 
#include "broker.h"
#include "module.h"
#include "application.h"

#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>

//using namespace yarp::os;
using namespace std; 

//namespace ymm {


#define DEF_PERIOD		100  //ms
#define WDOG_PERIOD		5000 //ms

typedef enum __RSTATE {
	SUSPENDED,
	READY,
	CONNECTING,
	RUNNING,
	DYING, 
	DEAD
} RSTATE;


class MEvent{

public: 
	MEvent() {}
	virtual ~MEvent() {}
	virtual void onExecutableStart(void* which) {};
	virtual void onExecutableStop(void* which) {};
	virtual void onExecutableDied(void* which) {};
	virtual void onExecutableFailed(void* which) {};
	virtual void onCnnStablished(void* which) {};
	virtual void onCnnFailed(void* which) {};
protected:


private:

};


/**
 * Class Executable  
 */
class Executable : public yarp::os::RateThread{

public: 
	Executable(Broker* _broker, MEvent* _event, bool bWatchDog=true);
	virtual ~Executable();
	bool threadInit();
	void afterStart(bool s);
	void run();
	void threadRelease();

	bool start(void);
	void stop(void);
	void kill(void);

	void setID(int id) { theID = id;}
	void setCommand(const char* val) { if(val) strCommand = val; }
	void setParam(const char* val) { if(val) strParam = val; }
	void setHost(const char* val) { if(val) strHost = val; }
	void setStdio(const char* val) { if(val) strStdio = val; }
	void setWorkDir(const char* val) { if(val) strWorkdir = val; } 
	void setEnv(const char* val) {if(val) strEnv = val; }

	void addConnection(Connection &cnn) { connections.push_back(cnn); }
	RSTATE state(void) { return getState();}
	Broker* getBroker(void) { return broker; }
	MEvent* getEvent(void) { return event; }
	const char* getCommand(void) { return strCommand.c_str(); }
	const char* getParam(void) { return strParam.c_str(); }
	const char* getHost(void) { return strHost.c_str(); }
	const char* getStdio(void) { return strStdio.c_str(); }
	const char* getWorkDir(void) { return strWorkdir.c_str(); }
	const char* getEnv(void) { return strEnv.c_str(); }
	int getID(void) { return theID; }
	CnnContainer& getConnections(void) { return connections;}

	void enableAutoConnect(void) { bAutoConnect = true; }
	void disableAutoConnect(void) { bAutoConnect = false; }

private:
	bool bAutoConnect;
	string strCommand;
	string strParam;
	string strHost;
	string strStdio;
	string strWorkdir;	
	string strEnv;
	int theID;
	
	RSTATE status; 
	bool bWatchDog;
	Broker* broker;
	MEvent* event;
	CnnContainer connections;
	
	ErrorLogger* logger;
	yarp::os::Semaphore safeState;
	
	void setState(RSTATE st);
	RSTATE getState(bool update=true);
	bool runModule(void);
	bool stopModule(void);
	void watchModule(void);
	bool checkPriorityPorts(void);
	bool checkNormalPorts(void);
	bool connectAllPorts(void);
	
};


typedef vector<Executable*> ExecutablePContainer;
typedef vector<Executable*>::iterator ExecutablePIterator;

 
 
//}

#endif //__Executable__
