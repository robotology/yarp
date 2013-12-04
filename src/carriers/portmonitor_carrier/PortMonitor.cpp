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
    group = getPeers().add(portName,this);
    if (!group) return false;
   
    Property options;
    options.fromString(proto.getSenderSpecifier().c_str());

    if(binder) delete binder;
    binder = NULL;        
    // check which monitor should be used
    ConstString script = options.check("script", Value("lua")).asString();
    if((binder = MonitorBinding::create(script.c_str())) == NULL)
    {
         YARP_LOG_ERROR("Currently only \'lua\' scripting is supported by portmonitor");
         return false;
    }
   
    // check the acceptance constraint
    ConstString constraint = options.check("constraint", Value("")).asString();
    binder->setAcceptConstraint(constraint.c_str());

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
        PortMonitor::lock();
        bReady =  binder->loadScript(strFile.c_str());
        PortMonitor::unlock();
        return bReady;

    }
    return (bReady = false);
}

void PortMonitor::setCarrierParams(const yarp::os::Property& params) 
{
    if(!bReady) return;
    PortMonitor::lock();
    binder->setParams(params);
    PortMonitor::unlock();
}

void PortMonitor::getCarrierParams(yarp::os::Property& params) 
{
    if(!bReady) return;
    PortMonitor::lock();
    binder->getParams(params);
    PortMonitor::unlock();
}


yarp::os::ConnectionReader& PortMonitor::modifyIncomingData(yarp::os::ConnectionReader& reader) 
{
    if(!bReady) return reader;

    PortMonitor::lock();
    KnownThings thing;
    thing.read(reader);
    yarp::os::KnownThings& result = binder->updateData(thing);    
    PortMonitor::unlock();
    con.reset();
    if(result.write(con.getWriter()))
        return con.getReader();
    return reader;
}

bool PortMonitor::acceptIncomingData(yarp::os::ConnectionReader& reader) 
{       
    if(!bReady) return false;
    
    PortMonitor::lock();
    KnownThings thing;
    thing.read(reader);
    bool result = binder->acceptData(thing);
    PortMonitor::unlock();
    if(!result)
        return false;

    getPeers().lock();
    YARP_ASSERT(group);
    result = group->acceptIncomingData(reader,this);
    getPeers().unlock();
    return result;
}


yarp::os::Portable& PortMonitor::modifyOutgoingData(yarp::os::Portable& portable)
{
    if(!bReady) return portable;
    
    //portable.write(con.getWriter());
    //Bottle b;
    //b.read(con.getReader());
    //Bottle* b = dynamic_cast<Bottle*>(&portable);
    //printf("%s\n", b->toString().c_str());
    //return portable;

    PortMonitor::lock();
    thing.reset();
    thing.setReference(&portable);
    yarp::os::KnownThings& result = binder->updateData(thing);    
    PortMonitor::unlock();
    return *result.getReference();
}

bool PortMonitor::acceptOutgoingData(yarp::os::Portable& portable)
{
    if(!bReady) return false;
    
    PortMonitor::lock();
    KnownThings thing;
    thing.setReference(&portable);
    bool result = binder->acceptData(thing);
    PortMonitor::unlock();
    return result;
}


/**
 * Class PortMonitorGroup
 */


ElectionOf<PortMonitorGroup> *PortMonitor::peers = NULL;

// Make a singleton manager for finding peer carriers.
ElectionOf<PortMonitorGroup>& PortMonitor::getPeers() {
    NetworkBase::lock();
    if (peers==NULL) {
        peers = new ElectionOf<PortMonitorGroup>;
        NetworkBase::unlock();
        YARP_ASSERT(peers);
    } else {
        NetworkBase::unlock();
    }
    return *peers;
}

// Decide whether data should be accepted, for real.
bool PortMonitorGroup::acceptIncomingData(yarp::os::ConnectionReader& reader,
                                       PortMonitor *source) 
{
    
    //bool accept = true;
    for (PeerRecord<PortMonitor>::iterator it = peerSet.begin(); it!=peerSet.end(); it++)
    {
        PortMonitor *peer = it->first;
        if(peer != source)
        {
            peer->lock();
            // TODO: check whether we should return false if 
            //       the peer monitor object is not ready!
            if(peer->getBinder())
                peer->getBinder()->peerTrigged();
            peer->unlock();
        }            
    }
    return source->getBinder()->canAccept();
}


