// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Log.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/ResourceFinder.h>

#include "PortMonitor.h"


using namespace yarp::os;


/**
 * Class PortMonitor
 */

// Read connection settings.
bool PortMonitor::configure(yarp::os::ConnectionState& proto) 
{       
    portName = proto.getRoute().getToName();
    sourceName = proto.getRoute().getFromName();
    
    Property options;
    options.fromString(proto.getSenderSpecifier().c_str());

    if(binder) delete binder;
    binder = NULL;        
    ConstString script = options.check("script", Value("lua")).asString();
    if((binder = MonitorBinding::create(script.c_str())) == NULL)
    {
         YARP_LOG_ERROR("Currently only \'lua\' scripting is supported by portmonitor");
         return false;
    }
   
    ConstString context = options.check("context", Value("")).asString();
    ConstString filename = options.check("file", Value("modifier")).asString();
    yarp::os::ResourceFinder rf;
    //rf.setDefaultConfigFile(filename);
    rf.setDefaultContext(context.c_str());
    rf.configure(0, NULL);
    ConstString strFile = rf.findFile(filename.c_str());
    if(strFile == "")
    {
        strFile = rf.findFile(filename+".lua");
            return (bReady =  binder->loadScript(strFile.c_str()));
    }
    return (bReady = false);
}

void PortMonitor::setCarrierParams(const yarp::os::Property& params) 
{
    if(!bReady) return;
    binder->setParams(params);
}

void PortMonitor::getCarrierParams(yarp::os::Property& params) 
{
    if(!bReady) return;
    binder->getParams(params);
}


yarp::os::ConnectionReader& PortMonitor::modifyIncomingData(yarp::os::ConnectionReader& reader) 
{
    if(!bReady) return reader;

    return binder->updateData(reader);
}

bool PortMonitor::acceptIncomingData(yarp::os::ConnectionReader& reader) 
{
    if(!bReady) return false;

    return binder->acceptData(reader);
}



YARP_SSIZE_T PortMonitor::read(const yarp::os::Bytes& b) 
{
    return -1;
}

