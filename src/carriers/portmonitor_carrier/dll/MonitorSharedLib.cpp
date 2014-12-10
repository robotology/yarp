// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
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
    monitor.close();
}

bool MonitorSharedLib::load(const yarp::os::Property& options)
{
    string filename = options.find("filename").asString();
    monitorFactory.open(filename.c_str(), "MonitorObject_there");
	if(!monitorFactory.isValid()) {
        string msg = string("Cannot load shared library ") + filename  + string(" (");
        msg += Vocab::decode(monitorFactory.getStatus()) + string(")");
        YARP_LOG_ERROR(msg.c_str());
		return false;
	}

    monitorFactory.addRef();

    monitor.open(monitorFactory);    
    if(!monitor.isValid()) {
        YARP_LOG_ERROR("Cannot create an instance of MonitorObject");
        return false;
    }

    return monitor->create(options);
}

bool MonitorSharedLib::setParams(const Property &params)
{
    return monitor->setparam(params);
}

bool MonitorSharedLib::getParams(yarp::os::Property& params)
{
    return monitor->getparam(params);
}

bool MonitorSharedLib::acceptData(yarp::os::Things& thing)
{
    return monitor->accept(thing);
}


yarp::os::Things& MonitorSharedLib::updateData(Things &thing)
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


