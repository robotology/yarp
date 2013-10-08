// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Log.h>
#include <yarp/os/ConstString.h>

#include "PortMonitor.h"


using namespace yarp::os;


/**
 * Class PortMonitor
 */

// Read connection settings.
bool PortMonitor::configure(yarp::os::ConnectionState& proto) 
{    
    //printf("configure\n");
    portName = proto.getRoute().getToName();
    sourceName = proto.getRoute().getFromName();
    
    Property options;
    options.fromString(proto.getSenderSpecifier().c_str());

    ConstString script = options.check("script", Value("lua")).asString();
    if((binder = MonitorBinding::create(script.c_str())) == NULL)
    {
         YARP_LOG_ERROR("Currently only \'lua\' scripting is supported by portmonitor");
         return false;
    }
   
    ConstString filename = options.check("file", Value("modifier")).asString();
    return (bReady =  binder->loadScript(filename.c_str()));
}

void PortMonitor::setCarrierParams(const yarp::os::Property& params) 
{
    if(binder == NULL)
        return;
    binder->setParams(params);
}

void PortMonitor::getCarrierParams(yarp::os::Property& params) 
{
    if(binder == NULL)
        return;
    binder->getParams(params);
}


yarp::os::ConnectionReader& PortMonitor::modifyIncomingData(yarp::os::ConnectionReader& reader) 
{
     if(binder == NULL)
        return reader;

    return binder->updateData(reader);
    //return *local;
}

YARP_SSIZE_T PortMonitor::read(const yarp::os::Bytes& b) 
{
    return -1;
}

