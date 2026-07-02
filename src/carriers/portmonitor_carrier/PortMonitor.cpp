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

std::pair<std::string, std::string> parseBlocks(const std::string& input)
{
    const std::string sendTag = "+send.portmonitor+";
    const std::string recvTag = "+recv.portmonitor+";

    size_t posSend = input.find(sendTag);
    size_t posRecv = input.find(recvTag);

    std::string middle;   // tra send e recv
    std::string afterRecv; // da recv in poi

    // Estrazione blocco "recv -> fine"
    if (posRecv != std::string::npos)
    {
        afterRecv = input.substr(posRecv + recvTag.size());
    }

    // Estrazione blocco tra send e recv (indipendente dall'ordine)
    if (posSend != std::string::npos && posRecv != std::string::npos)
    {
        size_t start = posSend + sendTag.size();
        size_t end = (posSend < posRecv) ? posRecv : std::string::npos;

        middle = input.substr(start, end - start);
    }
    else if (posSend != std::string::npos && posRecv == std::string::npos)
    {
        // solo send presente: tutto dopo send
        middle = input.substr(posSend + sendTag.size());
    }
    else if (posSend == std::string::npos && posRecv != std::string::npos)
    {
        // solo recv presente: tutto prima di recv
        middle = input.substr(0, posRecv);
    }
    else
    {
        // nessun tag: tutta la stringa è "middle"
        middle = input;
    }

    return {middle, afterRecv};
}

std::string separateString(std::string input)
{
    std::string output;
    size_t start = 0;
    if (start != std::string::npos) {
        output += " (";
        for (size_t i = start; i < (size_t)input.length(); i++) {
            char ch = input[i];
            if (ch == '+') {
                output += ") (";
            } else if (ch == '.') {
                output += " ";
            } else {
                output += ch;
            }
        }
        output += ")";
    }
    return output;
}

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
    //options.fromString(proto.getSenderSpecifier());
    options.put("source", sourceName);
    options.put("destination", portName);
    options.put("sender_side",
             (proto.getContactable()->getName() == sourceName) ? 1 : 0);
    options.put("receiver_side",
             (proto.getContactable()->getName() == portName) ? 1 : 0);

    std::string carrier_options = proto.getRoute().getCarrierName();

    //I want to split a string like this:
    //fast_tcp+send.portmonitor+file.bottle_compression_zlib+type.dll+recv.portmonitor+file.bottle_compression_zlib+type.dll
    //into two strings. For senders side:
    // file.bottle_compression_zlib+type.dll
    // and for receivers side:
    // file.bottle_compression_zlib+type.dll
    auto [file1, file2] = parseBlocks(carrier_options);
    if (proto.getContactable()->getName() == sourceName)
    {
        std::string sendoptions = separateString(file1);
        options.fromString(sendoptions, false);
    }
    else if (proto.getContactable()->getName() == portName)
    {
        std::string recvoptions = separateString(file2);
        options.fromString(recvoptions, false);
    }

    return configureFromProperty(options);
}

bool PortMonitor::configureFromProperty(yarp::os::Property& options)
{
    yCTrace(PORTMONITORCARRIER);
    std::string debug_string_options = options.toString();
    YARP_UNUSED(debug_string_options);

    // clear the list of binders, if any, before creating new ones
    for (auto it=binder.begin(); it!=binder.end(); ++it)
    {
        if (*it) {
            delete *it;
            *it=nullptr;
        }
    }
    binder.clear();

    std::vector<std::string> scriptstype;
    std::vector<std::string> filenames;
    std::vector<std::string> constraints;
    std::vector<std::string> contexts;    // context is used to find the script files

    std::string type = options.check("type", Value("")).asString();
    std::string file = options.check("file", Value("")).asString();
    bool sender_side = options.check("sender_side", Value(false)).asBool() == 1;
    // Validate that mandatory parameters are provided
    if (type.empty() || file.empty())
    {
        if (sender_side)
            yCError(PORTMONITORCARRIER) << "Missing required 'type' or 'file' parameter on sender side";
        else
            yCError(PORTMONITORCARRIER) << "Missing required 'type' or 'file' parameter on receiver side";
        return false;
    }

    scriptstype.push_back(type);
    filenames.push_back(file);
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

        std::string type = options.check(typeKey, Value("")).asString();
        std::string file = options.check(fileKey, Value("")).asString();
        // Validate that mandatory parameters are provided
        if (type.empty() || file.empty())
        {
            if (sender_side)
                yCError(PORTMONITORCARRIER) << "Missing required 'type' or 'file' parameter on sender side";
            else
                yCError(PORTMONITORCARRIER) << "Missing required 'type' or 'file' parameter on receiver side";
            return false;
        }

        scriptstype.push_back(type);
        filenames.push_back(file);
        constraints.push_back(options.check(constraintKey, Value("")).asString());
        contexts.push_back(options.check(contextKey, Value("")).asString());
    }

    // Create the monitor binding objects for each script
    for (size_t i=0; i< scriptstype.size(); i++)
    {
        MonitorBinding* mon = MonitorBinding::create(scriptstype[i].c_str());
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
        if(scriptstype[i] != "dll")
        {
            yarp::os::ResourceFinder rf;
            rf.setDefaultContext(contexts[i]);
            rf.configure(0, nullptr);
            strFile = rf.findFile(filenames[i]);
            if(strFile.empty()) {
                strFile = rf.findFile(filenames[i] + ".lua");
            }
            if(strFile.empty()) {
                strFile = rf.findFile(filenames[i] + ".py");
            }
        }

        // set the information for the monitor object
        Property info;
        info.clear();
        info.put("filename", strFile);
        info.put("type", scriptstype[i]);
        info.put("source", options.find("source").asString());
        info.put("destination", options.find("destination").asString());
        info.put("sender_side",  options.find("sender_side").asInt32());
        info.put("receiver_side",options.find("receiver_side").asInt32());

        //here, for multiple portmonitors, only the related parameters should be provided...
        std::string carrier_options = options.find("carrier").asString();
        info.put("carrier", carrier_options);

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

    localReader->setParentConnectionReader(&reader);

    //Call the sequence of portmonitors,
    //feeding the output of one as input to the next one.
    PortMonitor::lock();
    yarp::os::Things thing;
    thing.setConnectionReader(*localReader);
    yarp::os::Things* current = &thing;
    for (size_t i = 0; i < binder.size(); ++i)
    {
        // If no update callback is available, avoid calling it
        if(!binder[i]->hasUpdate())
        {
            continue;
        }
        current = &binder[i]->updateData(*current);
    }
    PortMonitor::unlock();

    con.reset();
    if(current->write(con.getWriter())) {
        auto& cReader = con.getReader(reader.getWriter());
        cReader.setParentConnectionReader(&reader);
        if (current->getPortReader() != nullptr) {
            cReader.getWriter()->setReplyHandler(*current->getPortReader());
        }
        return cReader;
    }
    return *localReader;
}

bool PortMonitor::acceptIncomingData(yarp::os::ConnectionReader& reader)
{
    yCTrace(PORTMONITORCARRIER);

    // TODO: the implementation of this method is not complete yet.
    // It should call the acceptData method of all the portmonitors in the binder vector, not just the first one.

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

    //Call the sequence of portmonitors,
    //feeding the output of one as input to the next one.
    PortMonitor::lock();
    thing.reset();
    thing.setPortWriter(const_cast<yarp::os::PortWriter*>(&writer));
    yarp::os::Things* current = &thing;
    for (size_t i = 0; i < binder.size(); ++i)
    {
        // If no update callback is available, avoid calling it
        if(!binder[i]->hasUpdate())
        {
            continue;
        }
        current = &binder[i]->updateData(*current);
    }
    PortMonitor::unlock();
    return *current->getPortWriter();
}

bool PortMonitor::acceptOutgoingData(const yarp::os::PortWriter& writer)
{
    yCTrace(PORTMONITORCARRIER);

    // TODO: the implementation of this method is not complete yet.
    // It should call the acceptData method of all the portmonitors in the binder vector, not just the first one.

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

    //Call the sequence of portmonitors,
    //feeding the output of one as input to the next one.
    PortMonitor::lock();
    thing.reset();
    thing.setPortReader(&reader);
    yarp::os::Things* current = &thing;
    for (size_t i = 0; i < binder.size(); ++i)
    {
        // If no reply callback is available, avoid calling it
        if(!binder[i]->hasUpdateReply())
        {
            continue;
        }
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
