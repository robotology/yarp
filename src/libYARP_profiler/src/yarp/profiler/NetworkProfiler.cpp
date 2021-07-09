/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/profiler/NetworkProfiler.h>

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/Port.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/Carrier.h>
#include <yarp/companion/impl/Companion.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::profiler;
using namespace yarp::profiler::graph;



NetworkProfiler::ProgressCallback* NetworkProfiler::progCallback = nullptr;

bool NetworkProfiler::yarpNameList(ports_name_set &ports, bool complete) {
    ports.clear();

    ContactStyle style;
    style.quiet = true;
    style.timeout = 3.0;
    string nameserver = NetworkBase::getNameServerName();
    Bottle msg, reply;
    msg.addString("bot");
    msg.addString("list");
    if(!NetworkBase::write(Contact(nameserver), msg, reply, style)) {
        yError() << "Cannot write to yarp name server";
        return false;
    }

    if(reply.size() == 0) {
        yError() << "Empty reply from yarp name server";
        return false;
    }

    for (size_t i=1; i<reply.size(); i++) {
        Bottle *entry = reply.get(i).asList();
        if(entry != nullptr) {
            bool shouldTake = false;
            std::string portname = entry->check("name", Value("")).asString();
            if(complete)
            {
                shouldTake = portname != "";
            }
            else
            {
                shouldTake = portname != "" && portname != "fallback" && portname != nameserver;
            }
            if (shouldTake) {
                Contact c = Contact::fromConfig(*entry);
                if(c.getCarrier() != "mcast")
                    ports.push_back(*entry);
            }
        }
    }

    return true;
}

bool NetworkProfiler::getPortDetails(const string& portName, PortDetails& info) {

    info.name = portName;
    Port ping;
    ping.open("/yarpviz");
    ping.setAdminMode(true);
    ping.setTimeout(1.0);
    if(!NetworkBase::connect(ping.getName(), portName)) {
        yWarning()<<"Cannot connect to"<<portName;
        ping.close();
        return false;
    }

    // Getting output connections list
    Bottle cmd, reply;
    cmd.addString("list"); cmd.addString("out");
    if(!ping.write(cmd, reply)) {
        yError()<<"Cannot write (list out) to"<<portName;
        ping.close();
        return false;
    }
    for(size_t i=0; i<reply.size(); i++) {
        ConnectionInfo cnn;
        cnn.name = reply.get(i).asString();
        Bottle reply2;
        cmd.clear();
        cmd.addString("list"); cmd.addString("out"); cmd.addString(cnn.name);
        if(!ping.write(cmd, reply2))
            yWarning()<<"Cannot write (list out"<<cnn.name<<") to"<<portName;
        else
            cnn.carrier =  reply2.find("carrier").asString();
        info.outputs.push_back(cnn);
    }

    // Getting input connections list
    cmd.clear(); reply.clear();
    cmd.addString("list"); cmd.addString("in");
    if(!ping.write(cmd, reply)) {
        yError()<<"Cannot write (list in) to"<<portName;
        ping.close();
        return false;
    }
    for(size_t i=0; i<reply.size(); i++) {
        ConnectionInfo cnn;
        cnn.name = reply.get(i).asString();
        if(cnn.name != ping.getName())
            info.inputs.push_back(cnn);
    }

    // Getting owner info
    cmd.clear(); reply.clear();
    cmd.addString("prop"); cmd.addString("get"); cmd.addString(portName);
    if(!ping.write(cmd, reply)) {
        yError()<<"Cannot write (prop get"<<portName<<") to"<<portName;
        ping.close();
        return false;
    }

    Property* process = reply.find("process").asDict();
    if(!process)
        yWarning()<<"Cannot find 'process' property of port "<<portName;
    else {
        info.owner.name = process->find("name").asString();
        info.owner.arguments = process->find("arguments").asString();
        info.owner.pid = process->find("pid").asInt32();
        info.owner.priority = process->find("priority").asInt32();
        info.owner.policy = process->find("policy").asInt32();
    }

    Property* platform = reply.find("platform").asDict();
    if(!platform)
        yWarning()<<"Cannot find 'platform' property of port "<<portName;
    else {
        info.owner.os = platform->find("os").asString();
        info.owner.hostname = platform->find("hostname").asString();
    }

    ping.close();
    return true;
}


bool NetworkProfiler::creatNetworkGraph(ports_detail_set details, yarp::profiler::graph::Graph& graph) {

    // adding the ports and processor nodes
    if(NetworkProfiler::progCallback)
        NetworkProfiler::progCallback->onProgress(0);

    ports_detail_iterator itr;
    unsigned int itr_count = 0;
    for(itr = details.begin(); itr!=details.end(); itr++) {
        PortDetails info = (*itr);

        // port node
        PortVertex* port = new PortVertex(info.name);
        if(!info.inputs.size() && !info.outputs.size())
            port->property.put("orphan", true);
        graph.insert(*port);

        //process node (owner)
        ProcessVertex* process = new ProcessVertex(info.owner.pid, info.owner.hostname);
        //prop.clear();
        process->property.put("name", info.owner.name);
        process->property.put("arguments", info.owner.arguments);
        process->property.put("hostname", info.owner.hostname);
        process->property.put("priority", info.owner.priority);
        process->property.put("policy", info.owner.policy);
        process->property.put("os", info.owner.os);
        process->property.put("hidden", false);
        auto itrVert=graph.insert(*process);
        // create connection between ports and its process
        if(dynamic_cast<ProcessVertex*> (*itrVert))
            port->setOwner((ProcessVertex*)(*itrVert));



        //machine node (owner of the process)
        MachineVertex* machine = new MachineVertex(info.owner.os, info.owner.hostname);
        graph.insert(*machine);
        //todo do the same done for the process.
        process->setOwner(machine);

        if(!info.inputs.size() && !info.outputs.size())
            graph.insertEdge(*process, *port, Property("(type ownership) (dir unknown)"));

        // calculate progress
        if(NetworkProfiler::progCallback) {
            NetworkProfiler::progCallback->onProgress((unsigned int) (++itr_count/((float)(details.size()*2)) * 100.0) );
        }
    }


    // create connection between ports
    for(itr = details.begin(); itr!=details.end(); itr++) {
        PortDetails info = (*itr);
        // find the current port vertex in the graph
        pvertex_iterator vi1 = graph.find(PortVertex(info.name));
        yAssert(vi1 != graph.vertices().end());
        for(auto cnn : info.outputs) {
            pvertex_iterator vi2 = graph.find(PortVertex(cnn.name));
            if(vi2 != graph.vertices().end()) {
                //yInfo()<<"connecting "<<(*vi1)->property.find("name").asString()<<"->"<<(*vi2)->property.find("name").asString();
                Property edge_prop("(type connection)");
                edge_prop.put("carrier", cnn.carrier);
                graph.insertEdge(vi1, vi2, edge_prop);
            }
            else
                yWarning()<<"Found a nonexistent port ("<<cnn.name<<")"<<"in the output list of"<<(*vi1)->property.find("name").asString();
        }
        // calculate progress
        if(NetworkProfiler::progCallback) {
            NetworkProfiler::progCallback->onProgress((unsigned int) (++itr_count/((float)(details.size()*2)) * 100.0) );
        }
    }
    if(NetworkProfiler::progCallback)
        NetworkProfiler::progCallback->onProgress(100); // is it really needed? :p
    return true;
}

bool NetworkProfiler::yarpClean(float timeout) {

    if (timeout <= 0)
        timeout = -1;

    stringstream sstream;
    sstream<<timeout;
    char* argv[2];
    argv[0] = (char*) "--timeout";
    argv[1] = (char*) sstream.str().c_str();
    yarp::companion::impl::Companion::getInstance().cmdClean(2,argv);
    return true;
}

bool NetworkProfiler::creatSimpleModuleGraph(yarp::profiler::graph::Graph& graph, yarp::profiler::graph::Graph& subgraph) {
    subgraph.clear();
    pvertex_const_iterator itr;
    const pvertex_set& vertices = graph.vertices();
    //insert machines
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {

        if(!dynamic_cast<MachineVertex*>(*itr))
                continue;
        else
        {
            auto* mv1 = dynamic_cast<MachineVertex*>(*itr);
            if (mv1)
            {
                MachineVertex* mv2 = new MachineVertex(mv1->property.find("os").asString(),
                                                       mv1->property.find("hostname").asString());
                mv2->property = mv1->property;
                subgraph.insert(*mv2);
            }
        }
    }

    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        if(!dynamic_cast<ProcessVertex*>(*itr))
            continue;
        auto* pv1 = dynamic_cast<ProcessVertex*>(*itr);
        if (pv1)
        {
            ProcessVertex* pv2 = new ProcessVertex(pv1->property.find("pid").asInt32(),
                                                   pv1->property.find("hostname").asString());
            pv2->property = pv1->property;
            subgraph.insert(*pv2);
        }
    }
    // insert edges
    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        if(!dynamic_cast<ProcessVertex*>(*itr))
            continue;
        Vertex* v1 = (*itr);
        const edge_set& outs = v1->outEdges();
        edge_const_iterator eitr;
        for(eitr = outs.begin(); eitr!=outs.end(); eitr++) {
            const Edge& e = (*eitr);
            const Vertex& p1 = e.second();

            const edge_set& pouts = p1.outEdges();
            edge_const_iterator peitr;
            for(peitr = pouts.begin(); peitr!=pouts.end(); peitr++) {
                const Vertex& p2 = (*peitr).second();
                Property prop((*peitr).property);
                string label = p1.property.find("name").asString();
                label.append(" - ").append(p2.property.find("name").asString());
                prop.put("label", label);
                subgraph.insertEdge(*v1, p2.outEdges()[0].second(), prop);
            }
        }
    }
    return true;
}

std::string NetworkProfiler::packetPrioToString(yarp::os::QosStyle::PacketPriorityLevel level) {
    std::string name;
    switch(level) {
    case yarp::os::QosStyle::PacketPriorityNormal : {
        name = "NORMAL";
        break;
    }
    case yarp::os::QosStyle::PacketPriorityLow : {
        name = "LOW";
        break;
    }
    case yarp::os::QosStyle::PacketPriorityHigh : {
        name = "HIGH";
        break;
    }
    case yarp::os::QosStyle::PacketPriorityCritical : {
        name = "CRITIC";
        break;
    }
    case yarp::os::QosStyle::PacketPriorityInvalid : {
        name = "INVALID";
        break;
    }
    default: {
        name = "UNDEFINED";
    }
    };
    return name;
}

yarp::os::QosStyle::PacketPriorityLevel NetworkProfiler::packetStringToPrio(std::string level) {
    if(level=="NORMAL") return yarp::os::QosStyle::PacketPriorityNormal;
    if(level=="LOW") return yarp::os::QosStyle::PacketPriorityLow;
    if(level=="HIGH") return yarp::os::QosStyle::PacketPriorityHigh;
    if(level=="CRITIC") return yarp::os::QosStyle::PacketPriorityCritical;
    if(level=="INVALID") return yarp::os::QosStyle::PacketPriorityInvalid;
    return yarp::os::QosStyle::PacketPriorityInvalid;
}

bool NetworkProfiler::updateConnectionQosStatus(yarp::profiler::graph::Graph& graph) {
    // adding all process nodes and subgraphs
    pvertex_const_iterator itr;
    const pvertex_set& vertices = graph.vertices();

    for(itr = vertices.begin(); itr!=vertices.end(); itr++) {
        const Vertex &v1 = (**itr);
        for(const auto& i : v1.outEdges()) {
            Edge& edge = (Edge&) i;
            const Vertex &v2 = edge.second();
            if(!v1.property.check("hidden") && !v2.property.check("hidden")) {
                if(edge.property.find("type").asString() == "connection") {
                    //yInfo()<<v1.property.find("name").asString()<<"->"<<v2.property.find("name").asString()<<label;
                    yarp::os::QosStyle fromStyle, toStyle;
                    if(yarp::os::NetworkBase::getConnectionQos(v1.property.find("name").asString(),
                                                               v2.property.find("name").asString(), fromStyle, toStyle)) {
                        // source
                        edge.property.put("FromThreadPriority", fromStyle.getThreadPriority());
                        edge.property.put("FromThreadPolicy", fromStyle.getThreadPolicy());
                        edge.property.put("FromPacketPriority", fromStyle.getPacketPriorityAsLevel());
                        edge.property.put("ToThreadPriority", toStyle.getThreadPriority());
                        edge.property.put("ToThreadPolicy", toStyle.getThreadPolicy());
                        edge.property.put("ToPacketPriority", toStyle.getPacketPriorityAsLevel());
                    }
                    else
                        yWarning()<<"Cannot retrieve Qos property of"<<v1.property.find("name").asString()<<"->"<<v2.property.find("name").asString();
                }
            }
        }
    }
    return true;
}

bool NetworkProfiler::attachPortmonitorPlugin(std::string portName, yarp::os::Property pluginProp) {

    //e.g.,  atch in "(context yarpviz) (file portrate)"
    yarp::os::Bottle cmd, reply;
    cmd.addString("atch");
    cmd.addString("in");
    cmd.addString(pluginProp.toString());
    //Property& prop = cmd.addDict();
    //prop.fromString(pluginProp.toString());
    //yInfo()<<cmd.toString();
    Contact srcCon = Contact::fromString(portName);
    bool ret = yarp::os::NetworkBase::write(srcCon, cmd, reply, true, true, 2.0);
    if(!ret) {
        yError()<<"Cannot write to"<<portName;
        return false;
    }
    if(reply.get(0).asString() != "ok") {
             yError()<<reply.toString();
        return false;
    }

    return true;

}

bool NetworkProfiler::detachPortmonitorPlugin(std::string portName) {
    //e.g.,  dtch in
    yarp::os::Bottle cmd, reply;
    cmd.addString("dtch");
    cmd.addString("in");
    Contact srcCon = Contact::fromString(portName);
    bool ret = yarp::os::NetworkBase::write(srcCon, cmd, reply, true, true, 2.0);
    if(!ret) {
        yError()<<"Cannot write to"<<portName;
        return false;
    }
    if(reply.get(0).asString() != "ok") {
             yError()<<reply.toString();
        return false;
    }
    return true;
}

bool NetworkProfiler::setPortmonitorParams(std::string portName, yarp::os::Property& param) {
    //e.g.,  set in "/view" (log_raw 1)"
    yarp::os::Bottle cmd, reply;
    cmd.addString("set");
    cmd.addString("in");
    cmd.addString(portName.c_str());
    Bottle tmp;
    tmp.fromString(param.toString());
    cmd.add(tmp.get(0));
    Contact srcCon = Contact::fromString(portName);
    bool ret = yarp::os::NetworkBase::write(srcCon, cmd, reply, true, true, 2.0);
    if(!ret) {
        yError()<<"Cannot write to"<<portName;
        return false;
    }
    if(reply.size() > 1) {
        if(reply.get(0).isString() && reply.get(0).asString() == "fail") {
            yError()<<reply.toString();
            return false;
        }
        else if(reply.get(0).isInt32() && reply.get(0).asInt32() == -1) {
            yError()<<reply.toString();
            return false;
        }
    }
    return true;
}

bool NetworkProfiler::getPortmonitorParams(std::string portName, yarp::os::Bottle& param) {
    //e.g.,  get in /portname"
    yarp::os::Bottle cmd;
    cmd.addString("get");
    cmd.addString("in");
    cmd.addString(portName.c_str());
    Contact srcCon = Contact::fromString(portName);
    bool ret = yarp::os::NetworkBase::write(srcCon, cmd, param, true, true, 2.0);
    if(!ret) {
        yError()<<"Cannot write to"<<portName;
        return false;
    }
    if(param.size() > 1) {
        if(param.get(0).isString() && param.get(0).asString() == "fail") {
            yError()<<param.toString();
            return false;
        }
        else if(param.get(0).isInt32() && param.get(0).asInt32() == -1) {
            yError()<<param.toString();
            return false;
        }
    }
    return true;
}
