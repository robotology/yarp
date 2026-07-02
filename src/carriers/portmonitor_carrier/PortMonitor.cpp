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
    std::string debug_string_options = options.toString();

    for (auto it=binder.begin(); it!=binder.end(); ++it)
    {
        if (*it) {
            delete *it;
        }
    }

    std::vector<std::string> scripts;
    std::vector<std::string> filenames;
    std::vector<std::string> constraints;
    std::vector<std::string> contexts;    // context is used to find the script files

    scripts.push_back(options.check("type", Value("lua")).asString());
    filenames.push_back(options.check("file", Value("modifier")).asString());
    constraints.push_back(options.check("constraint", Value("")).asString());
    contexts.push_back(options.check("context", Value("")).asString());

    // Optional support for multiple scripts, e.g. type1, file1, constraint1, context1, type2, file2, constraint2, context2, ...
    for (int i = 1; ; ++i)
    {
        std::string typeKey = "type" + std::to_string(i);
        std::string fileKey = "file" + std::to_string(i);
        std::string constraintKey = "constraint" + std::to_string(i);
        std::string contextKey = "context" + std::to_string(i);

        // if typeN is not provided, we assume that there are no more scripts to load
        if (!options.check(typeKey)) {
            break;
        }

        scripts.push_back(options.check(typeKey, Value("lua")).asString());
        filenames.push_back(options.check(fileKey, Value("modifier")).asString());
        constraints.push_back(options.check(constraintKey, Value("")).asString());
        contexts.push_back(options.check(contextKey, Value("")).asString());
    }

    // Create the monitor binding objects for each script
    for (size_t i=0; i< scripts.size(); i++)
    {
        MonitorBinding* mon = MonitorBinding::create(scripts[i].c_str());
        if(mon == nullptr)
        {
            //Failed to create monitor
            return false;
        }

        // set the acceptance constraint
        mon->setAcceptConstraint(constraints[i].c_str());
        binder.push_back(mon);
    }

    // provide some useful information for the monitor object
    // which can be accessed in the create() callback.
    bReady=true;
    PortMonitor::lock();
    for (size_t i=0; i<binder.size(); i++)
    {
        // sets the context for finding the script file
        std::string strFile = filenames[i];
        if(scripts[i] != "dll")
        {
            yarp::os::ResourceFinder rf;
            rf.setDefaultContext(contexts[i]);
            rf.configure(0, nullptr);
            strFile = rf.findFile(filenames[i]);
            if(strFile.empty()) {
                strFile = rf.findFile(filenames[i] + ".lua");
            }
        }

        // set the information for the monitor object
        Property info;
        info.clear();
        info.put("filename", strFile);
        info.put("type", scripts[i]);
        info.put("source", options.find("source").asString());
        info.put("destination", options.find("destination").asString());
        info.put("sender_side",  options.find("sender_side").asInt32());
        info.put("receiver_side",options.find("receiver_side").asInt32());

        //here, for multiple portmonitors, only the related parameters should be provided...
        info.put("carrier", options.find("carrier").asString());

        // load the monitor object with the provided information
        bReady &= binder[i]->load(info);
    }
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
    for (size_t i=0; i<binder.size(); i++)
    {
        binder[i]->setParams(params);
    }
    PortMonitor::unlock();
}

void PortMonitor::getCarrierParams(yarp::os::Property& params) const
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return;
    }
    PortMonitor::lock();
    for (size_t i=0; i<binder.size(); i++)
    {
        binder[i]->getParams(params);
    }
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
    // The reader passed to this function is indeed empty.
    // first check if we need to call the update callback
    if (!binder[0]->hasUpdate())
    {
        localReader->setParentConnectionReader(&reader);
        return *localReader;
    }

    PortMonitor::lock();
    yarp::os::Things thing;
    thing.setConnectionReader(*localReader);
    yarp::os::Things& result = binder[0]->updateData(thing);
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

    bool result = true;
    localReader = &reader;
    // If no accept callback avoid calling the binder
    if (binder[0]->hasAccept())
    {
        PortMonitor::lock();
        Things thing;
        // set the reference connection reader
        thing.setConnectionReader(reader);
        result &= binder[0]->acceptData(thing);
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
    for (size_t i=0; i<binder.size(); i++)
    {
        if(!binder[i]->hasUpdate())
        {
            return writer;
        }
    }

    //Call the sequence of portmonitors,
    //feeding the output of one as input to the next one.
    PortMonitor::lock();
    thing.reset();
    thing.setPortWriter(const_cast<yarp::os::PortWriter*>(&writer));
    yarp::os::Things* current = &thing;
    for (size_t i = 0; i < binder.size(); ++i) {
        current = &binder[i]->updateData(*current);
    }
    PortMonitor::unlock();
    return *current->getPortWriter();
}

bool PortMonitor::acceptOutgoingData(const yarp::os::PortWriter& writer)
{
    yCTrace(PORTMONITORCARRIER);

    if(!bReady) {
        return false;
    }

    bool result = true;
    // If no accept callback avoid calling it
    if (!binder[0]->hasAccept()) {
        return true;
    }

    PortMonitor::lock();
    yarp::os::Things thing;
    thing.setPortWriter(const_cast<yarp::os::PortWriter*>(&writer));
    result &= binder[0]->acceptData(thing);
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
    for (size_t i=0; i<binder.size(); i++)
    {
        if(!binder[i]->hasUpdateReply())
        {
            return reader;
        }
    }

    //Call the sequence of portmonitors,
    //feeding the output of one as input to the next one.
    PortMonitor::lock();
    thing.reset();
    thing.setPortReader(&reader);
    yarp::os::Things* current = &thing;
    for (size_t i = 0; i < binder.size(); ++i) {
        current = &binder[i]->updateReply(*current);
    }
    PortMonitor::unlock();
    return *current->getPortReader();
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
