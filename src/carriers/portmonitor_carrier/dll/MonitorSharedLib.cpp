// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>

#include "MonitorSharedLib.h"


using namespace yarp::os;
using namespace std;


/**
 * Class MonitorSharedLib
 */
MonitorSharedLib::MonitorSharedLib(void) 
{
}

MonitorSharedLib::~MonitorSharedLib()
{
    if(monitor.isValid())
        monitor->destroy();
}

bool MonitorSharedLib::load(const char* script_file)
{
    monitorFactory.open(script_file, "MonitorObject_there");
	if(!monitorFactory.isValid()) {
        string msg = string("Cannot load shared library ") + script_file  + string(" (");
        msg += Vocab::decode(monitorFactory.getStatus()) + string(")");
        YARP_LOG_ERROR(msg.c_str());
		return false;
	}

    monitor.open(monitorFactory);    
    if(!monitor.isValid()) {
        YARP_LOG_ERROR("Cannot create an instance of MonitorObject");
        return false;
    }

    return monitor->create();
}

bool MonitorSharedLib::setParams(const yarp::os::Property& params)
{
    monitor->setparam(params);
    return true;
}

bool MonitorSharedLib::getParams(yarp::os::Property& params)
{
    monitor->getparam(params);
    return true;
}

bool MonitorSharedLib::hasAccept() 
{
    return monitor->hasAccept();
}

bool MonitorSharedLib::acceptData(yarp::os::Things& thing)
{
    return monitor->accept(thing);
}

bool MonitorSharedLib::hasUpdate()
{
    return monitor->hasUpdate();
}


yarp::os::Things& MonitorSharedLib::updateData(yarp::os::Things& thing)
{
    return monitor->update(thing);
}

bool MonitorSharedLib::peerTrigged(void)
{
    monitor->trig();
    return true;
}

bool MonitorSharedLib::canAccept(void)
{
    if(constraint == "")
        return true;
     //TODO: constraint checking should be implemented here!
     return true;
}


