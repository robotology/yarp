// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/SharedLibraryClass.h>
#include <yarp/os/SharedLibrary.h>

#include "MonitorSharedLib.h"
#include "AbstractMonitorObject.h"

using namespace yarp::os;
using namespace std;


/**
 * Class MonitorSharedLib
 */
MonitorSharedLib::MonitorSharedLib(void) : bHasAcceptCallback(false), 
                               bHasUpdateCallback(false)
{
    /**
     * Registring extra functions for lua :
     *  - PortMonitor.setEvent()
     *  - PortMonitor.unsetEvent()
     *  - PortMonitor.setConstraint()
     *  - PortMonitor.getConstraint()
     */ 
}

MonitorSharedLib::~MonitorSharedLib()
{
}

bool MonitorSharedLib::load(const char* script_file)
{
    SharedLibraryClassFactory<AbstractMonitorObject> monitorFactory("simple","AbstractMonitorObject_there");
	if(!monitorFactory.isValid()) {
		//printf("Library problem: (%s)\n", Vocab::decode(helloFactory2.getStatus()).c_str());
        YARP_LOG_ERROR("Cannot load shared library");
        YARP_LOG_ERROR(script_file);
		return false;
	}

    return true;
}

bool MonitorSharedLib::setParams(const yarp::os::Property& params)
{
    //if(getLocalFunction("setparam"))
    //
    return true;
}

bool MonitorSharedLib::getParams(yarp::os::Property& params)
{
    //if(getLocalFunction("getparam"))
    return true;
}


bool MonitorSharedLib::acceptData(yarp::os::Things& thing)
{
    //if(getLocalFunction("accept"))
    
    return true;
}


yarp::os::Things& MonitorSharedLib::updateData(yarp::os::Things& thing)
{
    //if(getLocalFunction("update"))
    return thing;
}

bool MonitorSharedLib::peerTrigged(void)
{
    //if(getLocalFunction("trig"))
    return true;
}

bool MonitorSharedLib::canAccept(void)
{
    if(constraint == "")
        return true;

    //MonitorEventRecord& record = MonitorEventRecord::getInstance();   
    //string strConstraint = constraint;
    return true;
}


