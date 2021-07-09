/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <string>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/Route.h>
#include <yarp/os/Contactable.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>

#include "PortMonitor.h"
#include "MonitorLogComponent.h"



using namespace yarp::os;


/**
 * Class PortMonitor
 */

// Read connection settings.
bool PortMonitor::configure(yarp::os::ConnectionState& proto)
{
    yCTrace(PORTMONITORCARRIER);

    portName = proto.getRoute().getToName();
    sourceName = proto.getRoute().getFromName();
    group = getPeers().add(portName,this);
    if (!group) {
        yCError(PORTMONITORCARRIER) << "Cannot find group";
        return false;
    }

    Property options;
    options.fromString(proto.getSenderSpecifier());
    options.put("source", sourceName);
    options.put("destination", portName);
    options.put("sender_side",
             (proto.getContactable()->getName() == sourceName) ? 1 : 0);
    options.put("receiver_side",
             (proto.getContactable()->getName() == portName) ? 1 : 0);
    options.put("carrier", proto.getRoute().getCarrierName());
    return configureFromProperty(options);
}

bool PortMonitor::configureFromProperty(yarp::os::Property& options)
{
    yCTrace(PORTMONITORCARRIER);

    delete binder;
    binder = nullptr;

    std::string script = options.check("type", Value("lua")).asString();
    std::string filename = options.check("file", Value("modifier")).asString();
    std::string constraint = options.check("constraint", Value("")).asString();
    // context is used to find the script files
    std::string context = options.check("context", Value("")).asString();

    // check which monitor should be used
    if((binder = MonitorBinding::create(script.c_str())) == nullptr)
    {
         yCError(PORTMONITORCARRIER, R"(Currently only 'lua' script and 'dll' object is supported by portmonitor)");
         return false;
    }

    // set the acceptance constraint
    binder->setAcceptConstraint(constraint.c_str());

    std::string strFile = filename;

    if(script != "dll")
    {
        yarp::os::ResourceFinder rf;
        rf.setDefaultContext(context);
        rf.configure(0, nullptr);
        strFile = rf.findFile(filename);
        if(strFile.empty()) {
            strFile = rf.findFile(filename+".lua");
        }
    }

    // provide some useful information for the monitor object
    // which can be accessed in the create() callback.
    Property info;
    info.clear();
    info.put("filename", strFile);
    info.put("type", script);
    info.put("source", options.find("source").asString());
    info.put("destination", options.find("destination").asString());
    info.put("sender_side",  options.find("sender_side").asInt32());
    info.put("receiver_side",options.find("receiver_side").asInt32());
    info.put("carrier", options.find("carrier").asString());

    PortMonitor::lock();
    bReady =  binder->load(info);
    PortMonitor::unlock();
    return bReady;
}

void PortMonitor::setCarrierParams(const yarp::os::Property& params)
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return;
    }
    PortMonitor::lock();
    binder->setParams(params);
    PortMonitor::unlock();
}

void PortMonitor::getCarrierParams(yarp::os::Property& params) const
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return;
    }
    PortMonitor::lock();
    binder->getParams(params);
    PortMonitor::unlock();
}


yarp::os::ConnectionReader& PortMonitor::modifyIncomingData(yarp::os::ConnectionReader& reader)
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return reader;
    }

    // When we are here,
    // the incoming data should be accessed using localReader.
    // The reader passed to this function is infact empty.
    // first check if we need to call the update callback
    if(!binder->hasUpdate()) {
        localReader->setParentConnectionReader(&reader);
        return *localReader;
    }

    PortMonitor::lock();
    yarp::os::Things thing;
    thing.setConnectionReader(*localReader);
    yarp::os::Things& result = binder->updateData(thing);
    PortMonitor::unlock();
    con.reset();
    if(result.write(con.getWriter())) {
        auto& cReader = con.getReader(reader.getWriter());
        cReader.setParentConnectionReader(&reader);
        if (result.getPortReader() != nullptr) {
            cReader.getWriter()->setReplyHandler(*result.getPortReader());
        }
        return cReader;
    }
    return *localReader;
}

bool PortMonitor::acceptIncomingData(yarp::os::ConnectionReader& reader)
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return false;
    }

    bool result = false;
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
        if(!result) {
            return false;
        }

        // When data is read here using the reader passed to this functions,
        // then it won't be available for modifyIncomingData(). Thus, we write
        // it to a dummy connection and pass it to the modifyOutgoingData() using
        // localReader.
        // localReader points to a connection reader which contains
        // either the original or modified data.
        if(thing.hasBeenRead()) {
            con.reset();
            if(thing.write(con.getWriter())) {
                localReader = &con.getReader(reader.getWriter());
            }
        }
    }

    if(group!=nullptr) {
        getPeers().lock();
        result = group->acceptIncomingData(this);
        getPeers().unlock();
    }
    return result;
}


const yarp::os::PortWriter& PortMonitor::modifyOutgoingData(const yarp::os::PortWriter& writer)
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return writer;
    }

    // If no update callback avoid calling it
    if(!binder->hasUpdate()) {
        return writer;
    }

    PortMonitor::lock();
    thing.reset();
    thing.setPortWriter(const_cast<yarp::os::PortWriter*>(&writer));
    yarp::os::Things& result = binder->updateData(thing);
    PortMonitor::unlock();
    return *result.getPortWriter();
}

bool PortMonitor::acceptOutgoingData(const yarp::os::PortWriter& writer)
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return false;
    }

    // If no accept callback avoid calling it
    if(!binder->hasAccept()) {
        return true;
    }

    PortMonitor::lock();
    yarp::os::Things thing;
    thing.setPortWriter(const_cast<yarp::os::PortWriter*>(&writer));
    bool result = binder->acceptData(thing);
    PortMonitor::unlock();
    return result;
}

yarp::os::PortReader& PortMonitor::modifyReply(yarp::os::PortReader& reader)
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return reader;
    }

    // If no updateReply callback avoid calling it
    if(!binder->hasUpdateReply()) {
        return reader;
    }

    PortMonitor::lock();
    thing.reset();
    thing.setPortReader(&reader);
    yarp::os::Things& result = binder->updateReply(thing);
    PortMonitor::unlock();
    return *result.getPortReader();
}

/**
 * Class PortMonitorGroup
 */

ElectionOf<PortMonitorGroup> *PortMonitor::peers = nullptr;

// Make a singleton manager for finding peer carriers.
ElectionOf<PortMonitorGroup>& PortMonitor::getPeers()
{
    yCTrace(PORTMONITORCARRIER);

    NetworkBase::lock();
    if (peers==nullptr) {
        peers = new ElectionOf<PortMonitorGroup>;
        NetworkBase::unlock();
        yCAssert(PORTMONITORCARRIER, peers);
    } else {
        NetworkBase::unlock();
    }
    return *peers;
}

// Decide whether data should be accepted, for real.
bool PortMonitorGroup::acceptIncomingData(PortMonitor *source)
{
    yCTrace(PORTMONITORCARRIER);

    //bool accept = true;
    for (auto& it : peerSet)
    {
        PortMonitor *peer = it.first;
        if(peer != source)
        {
            peer->lock();
            // TODO: check whether we should return false if
            //       the peer monitor object is not ready!
            if(peer->getBinder()) {
                peer->getBinder()->peerTrigged();
            }
            peer->unlock();
        }
    }
    return source->getBinder()->canAccept();
}
