// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
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

    ConstString script = options.check("type", Value("lua")).asString();
    ConstString filename = options.check("file", Value("modifier")).asString();
    ConstString constraint = options.check("constraint", Value("")).asString();
    // context is used to find the script files
    ConstString context = options.check("context", Value("")).asString();

    // check which monitor should be used
    if((binder = MonitorBinding::create(script.c_str())) == NULL)
    {
         YARP_LOG_ERROR("Currently only \'lua\' script and \'dll\' object is supported by portmonitor");
         return false;
    }

    // set the acceptance constraint
    binder->setAcceptConstraint(constraint.c_str());

    ConstString strFile = filename;

    if(script != "dll")
    {
        yarp::os::ResourceFinder rf;
        rf.setDefaultContext(context.c_str());
        rf.configure(0, NULL);
        strFile = rf.findFile(filename.c_str());
        if(strFile == "")
            strFile = rf.findFile(filename+".lua");
    }

    // provide some useful information for the monitor object
    // which can be accessed in the create() callback.
    Property info;
    info.clear();
    info.put("filename", strFile);
    info.put("type", script);
    info.put("sender_side",
             (proto.getContactable()->getName() == sourceName));
    info.put("receiver_side",
             (proto.getContactable()->getName() == portName));
    info.put("source", sourceName);
    info.put("destination", portName);
    info.put("carrier", proto.getRoute().getCarrierName());

    PortMonitor::lock();
    bReady =  binder->load(info);
    PortMonitor::unlock();
    return bReady;
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

    // When we are here,
    // the incoming data should be accessed using localReader.
    // The reader passed to this function is infact empty.
    // first check if we need to call the update callback
    if(!binder->hasUpdate())
        return *localReader;

    PortMonitor::lock();
    yarp::os::Things thing;
    thing.setConnectionReader(*localReader);
    yarp::os::Things& result = binder->updateData(thing);
    PortMonitor::unlock();
    con.reset();
    if(result.write(con.getWriter()))
        return con.getReader();
    return *localReader;
}

bool PortMonitor::acceptIncomingData(yarp::os::ConnectionReader& reader)
{
    if(!bReady) return false;

    bool result;
    localReader = &reader;
    // If no accept callback avoid calling the binder
    if(binder->hasAccept())
    {
        PortMonitor::lock();
        Things thing;
        // set the reference connection reader
        thing.setConnectionReader(reader);
        result = binder->acceptData(thing);
        PortMonitor::unlock();
        if(!result)
            return false;

        // When data is read here using the reader passed to this functions,
        // then it wont be available for modifyIncomingData(). Thus, we write
        // it to a dumy connection and pass it to the modifyOutgoingData() using
        // localReader.
        // localReader points to a connection reader which contains
        // either the original or modified data.
        if(thing.hasBeenRead()) {
            con.reset();
            if(thing.write(con.getWriter()))
                localReader = &con.getReader();
        }
    }

    getPeers().lock();
    yAssert(group);
    result = group->acceptIncomingData(this);
    getPeers().unlock();
    return result;
}


yarp::os::PortWriter& PortMonitor::modifyOutgoingData(yarp::os::PortWriter& writer)
{
    if(!bReady) return writer;

    // If no update callback avoid calling it
    if(!binder->hasUpdate())
        return writer;

    PortMonitor::lock();
    thing.reset();
    thing.setPortWriter(&writer);
    yarp::os::Things& result = binder->updateData(thing);
    PortMonitor::unlock();
    return *result.getPortWriter();
}

bool PortMonitor::acceptOutgoingData(yarp::os::PortWriter& writer)
{
    if(!bReady) return false;

    // If no accept callback avoid calling it
    if(!binder->hasAccept())
        return true;

    PortMonitor::lock();
    yarp::os::Things thing;
    thing.setPortWriter(&writer);
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
        yAssert(peers);
    } else {
        NetworkBase::unlock();
    }
    return *peers;
}

// Decide whether data should be accepted, for real.
bool PortMonitorGroup::acceptIncomingData(PortMonitor *source)
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
