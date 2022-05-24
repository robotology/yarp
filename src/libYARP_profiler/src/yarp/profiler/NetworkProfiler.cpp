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
#include <algorithm>

using namespace yarp::os;
using namespace yarp::profiler;
using namespace yarp::profiler::graph;



NetworkProfiler::ProgressCallback* NetworkProfiler::progCallback = nullptr;

bool NetworkProfiler::getMachinesList(const ports_detail_set& ports, machines_list& l)
{
    l.clear();
    for (auto it = ports.begin(); it != ports.end(); it++)
    {
        std::string ip = it->owner_process.owner_machine.ip;
        if (std::find(l.begin(), l.end(), ip) == l.end())
        {
            l.push_back(ip);
        }
    }
    return true;
}

bool NetworkProfiler::getProcessesList(const ports_detail_set& ports, processes_list& l)
{
    l.clear();
    for (auto it = ports.begin(); it != ports.end(); it++)
    {
        //int pid = it->owner.pid;
        std::string pid = it->owner_process.process_fullname;
        if (std::find(l.begin(), l.end(), pid) == l.end())
        {
            l.push_back(pid);
        }
    }
    return true;
}

bool NetworkProfiler::getPortsDetailedList(ports_detail_set& ports, bool complete)
{
    ports_name_set port_names;
    bool r = getPortsList(port_names, complete);
    if (!r) return false;

    ports_detail_set port_details;
    for (auto it = port_names.begin(); it != port_names.end(); it++)
    {
        PortDetails details;
        r = getPortDetails(it->name, details);
        if (!r) { yError() << "getPortDetails of `" << it->name << "`failed"; }
        ports.push_back(details);
    }
    return r;
}

void NetworkProfiler::filterPortsListByIp(const ports_detail_set& in, ports_detail_set& filtered_out, std::string ip)
{
    filtered_out.clear();
    for (auto it = in.begin(); it != in.end(); it++)
    {
        if (ip != "*") {
            if (it->owner_process.owner_machine.hostname == ip) { filtered_out.push_back(*it); }
        }
    }
}

void NetworkProfiler::filterPortsListByProcess(const ports_detail_set& in, ports_detail_set& filtered_out, std::string process_fullname)
{
    filtered_out.clear();
    for (auto it = in.begin(); it != in.end(); it++)
    {
        if (process_fullname != "*") {
            if (it->owner_process.process_fullname == process_fullname) { filtered_out.push_back(*it); }
        }
    }
}

void NetworkProfiler::filterConnectionListByName(const connections_set& in, connections_set& filtered_out, std::string src_name, std::string dst_name)
{
    filtered_out.clear();
    for (auto it = in.begin(); it != in.end(); it++)
    {
        if (src_name!="*" && dst_name != "*") {
            if (it->src.name == src_name && it->src.name == dst_name) { filtered_out.push_back(*it);}
        }
        else if (src_name == "*") {
            if (it->dst.name == dst_name) { filtered_out.push_back(*it); }
        }
        else if (dst_name == "*") {
            if (it->src.name == src_name) {filtered_out.push_back(*it); }
        }
    }
}

void NetworkProfiler::filterConnectionListByIp(const connections_set& in, connections_set& filtered_out, std::string src_portnumber, std::string dst_portnumber)
{
    filtered_out.clear();
    for (auto it = in.begin(); it != in.end(); it++)
    {
        if (src_portnumber != "*" && dst_portnumber != "*") {
            if (it->src.ip == src_portnumber && it->src.ip == dst_portnumber) { filtered_out.push_back(*it); }
        }
        else if (src_portnumber == "*") {
            if (it->dst.ip == dst_portnumber) { filtered_out.push_back(*it); }
        }
        else if (dst_portnumber == "*") {
            if (it->src.ip == src_portnumber) { filtered_out.push_back(*it); }
        }
    }
}

void NetworkProfiler::filterConnectionListByPortNumber(const connections_set& in, connections_set& filtered_out, std::string src_ip, std::string dst_ip)
{
    filtered_out.clear();
    for (auto it = in.begin(); it != in.end(); it++)
    {
        if (src_ip != "*" && dst_ip != "*") {
            if (it->src.port_number == src_ip && it->src.port_number == dst_ip) { filtered_out.push_back(*it); }
        }
        else if (src_ip == "*") {
            if (it->dst.port_number == dst_ip) { filtered_out.push_back(*it); }
        }
        else if (dst_ip == "*") {
            if (it->src.port_number == src_ip) { filtered_out.push_back(*it); }
        }
    }
}

bool NetworkProfiler::getPortInfo (const std::string& name, const ports_name_set& ports, PortInfo& p)
{
    for (auto it = ports.begin(); it != ports.end(); it++)
    {
        if (name == it->name)
        {
            p.name = it->name;
            p.ip = it->ip;
            p.port_number = it->port_number;
            return true;
        }
    }
    return false;
}

bool NetworkProfiler::getConnectionsList(connections_set& connections)
{
    //get the list of all the ports
    ports_name_set ports;
    getPortsList(ports);

    //for each port, get the list of its connections...
    for (auto it = ports.begin(); it != ports.end(); it++)
    {
        PortDetails info;
        getPortDetails(it->name, info);
        ConnectionDetails conn;
        /*
        //I will not process incoming connections since probably everything in the system 
        //is determined just looking at the outgoing connections
        for (auto it2 = info.inputs.begin(); it2 != info.inputs.end(); it2++)
        {
            PortInfo p; getPortInfo(it2->name, ports, p);
            conn.src.name = it2->name;
            conn.src.ip = p.ip;
            conn.src.port_number = p.port_number;
            conn.dst.name = it->name;
            conn.dst.ip = it->ip;
            conn.dst.port_number = it->port_number;
            conn.carrier = it2->carrier;
        }*/
        //process all the outgoing connections and put them into a vector
        for (auto it2 = info.outputs.begin(); it2 != info.outputs.end(); it2++)
        {
            PortInfo p; getPortInfo(it2->port_name, ports, p);
            conn.src.name = it->name;
            conn.src.ip = it->ip;
            conn.src.port_number= it->port_number;
            conn.dst.name = it2->port_name;
            conn.dst.ip = p.ip;
            conn.dst.port_number = p.port_number;
            conn.carrier = it2->carrier;

            //add the connection to the list if it is valid and it is unique
            //(no other connections with same src/dst/protocol)
            if (conn.isValid() &&
                std::find(connections.begin(), connections.end(), conn) == connections.end())
            {
                connections.push_back(conn);
            }
        }
    }
    return true;
}

bool NetworkProfiler::getPortsList(ports_name_set &ports, bool complete) {
    ports.clear();

    ContactStyle style;
    style.quiet = true;
    style.timeout = 3.0;
    std::string nameserver = NetworkBase::getNameServerName();
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
                if (c.getCarrier() != "mcast")
                {
                      PortInfo portd;
                      portd.name = entry->find("name").asString();
                      portd.ip = entry->find("ip").asString();
                      portd.port_number = std::to_string(entry->find("port_number").asInt32());
                      ports.push_back(portd);
                }
            }
        }
    }

    return true;
}

bool NetworkProfiler::getPortDetails(const std::string& portName, PortDetails& details) {

    details.info.name = portName;
    Port ping;
    ping.open("...");
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
        ConnectedPortInfo cnn;
        cnn.port_name = reply.get(i).asString();
        Bottle reply2;
        cmd.clear();
        cmd.addString("list"); cmd.addString("out"); cmd.addString(cnn.port_name);
        if (!ping.write(cmd, reply2)) {
            yWarning()<<"Cannot write (list out"<<cnn.port_name <<") to"<<portName;
        } else {
            cnn.carrier = reply2.find("carrier").asString();
        }
        details.outputs.push_back(cnn);
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
        ConnectedPortInfo cnn;
        cnn.port_name = reply.get(i).asString();
        if (cnn.port_name != ping.getName()) {
            details.inputs.push_back(cnn);
        }
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
    if (!process) {
        yWarning()<<"Cannot find 'process' property of port "<<portName;
    } else {
        std::string process_str = process->toString();
        details.owner_process.process_name = process->find("name").asString();
        details.owner_process.arguments = process->find("arguments").asString();
        details.owner_process.pid = process->find("pid").asInt32();
        details.owner_process.priority = process->find("priority").asInt32();
        details.owner_process.policy = process->find("policy").asInt32();
        details.owner_process.process_fullname = details.owner_process.process_name + "(" + std::to_string(details.owner_process.pid) + ")";
    }

    Property* platform = reply.find("platform").asDict();
    if (!platform) {
        yWarning()<<"Cannot find 'platform' property of port "<<portName;
    } else {
        std::string platform_str = platform->toString();
        details.owner_process.owner_machine.os = platform->find("os").asString();
        details.owner_process.owner_machine.hostname = platform->find("hostname").asString();
        details.owner_process.owner_machine.ip = platform->find("hostname").asString();
    }

    ping.close();
    return true;
}


bool NetworkProfiler::creatNetworkGraph(ports_detail_set details_set, yarp::profiler::graph::Graph& graph) {

    // adding the ports and processor nodes
    if (NetworkProfiler::progCallback) {
        NetworkProfiler::progCallback->onProgress(0);
    }

    ports_detail_iterator itr;
    unsigned int itr_count = 0;
    for(itr = details_set.begin(); itr!= details_set.end(); itr++) {
        PortDetails details = (*itr);

        // port node
        PortVertex* port = new PortVertex(details.info.name);
        if (!details.inputs.size() && !details.outputs.size()) {
            port->property.put("orphan", true);
        }
        graph.insert(*port);

        //process node (owner)
        ProcessVertex* process = new ProcessVertex(details.owner_process.pid, details.owner_process.owner_machine.hostname);
        //prop.clear();
        process->property.put("name", details.owner_process.process_name);
        process->property.put("arguments", details.owner_process.arguments);
        process->property.put("hostname", details.owner_process.owner_machine.hostname);
        process->property.put("priority", details.owner_process.priority);
        process->property.put("policy", details.owner_process.policy);
        process->property.put("os", details.owner_process.owner_machine.os);
        process->property.put("hidden", false);
        auto itrVert=graph.insert(*process);
        // create connection between ports and its process
        if (dynamic_cast<ProcessVertex*>(*itrVert)) {
            port->setOwner((ProcessVertex*)(*itrVert));
        }


        //machine node (owner of the process)
        MachineVertex* machine = new MachineVertex(details.owner_process.owner_machine.os, details.owner_process.owner_machine.hostname);
        graph.insert(*machine);
        //todo do the same done for the process.
        process->setOwner(machine);

        if (!details.inputs.size() && !details.outputs.size()) {
            graph.insertEdge(*process, *port, Property("(type ownership) (dir unknown)"));
        }

        // calculate progress
        if(NetworkProfiler::progCallback) {
            NetworkProfiler::progCallback->onProgress((unsigned int) (++itr_count/((float)(details_set.size()*2)) * 100.0) );
        }
    }


    // create connection between ports
    for(itr = details_set.begin(); itr!= details_set.end(); itr++) {
        PortDetails details = (*itr);
        // find the current port vertex in the graph
        pvertex_iterator vi1 = graph.find(PortVertex(details.info.name));
        yAssert(vi1 != graph.vertices().end());
        for(auto cnn : details.outputs) {
            pvertex_iterator vi2 = graph.find(PortVertex(cnn.port_name));
            if(vi2 != graph.vertices().end()) {
                //yInfo()<<"connecting "<<(*vi1)->property.find("name").asString()<<"->"<<(*vi2)->property.find("name").asString();
                Property edge_prop("(type connection)");
                edge_prop.put("carrier", cnn.carrier);
                graph.insertEdge(vi1, vi2, edge_prop);
            } else {
                yWarning() << "Found a nonexistent port (" << cnn.port_name << ")"
                           << "in the output list of" << (*vi1)->property.find("name").asString();
            }
        }
        // calculate progress
        if(NetworkProfiler::progCallback) {
            NetworkProfiler::progCallback->onProgress((unsigned int) (++itr_count/((float)(details_set.size()*2)) * 100.0) );
        }
    }
    if (NetworkProfiler::progCallback) {
        NetworkProfiler::progCallback->onProgress(100); // is it really needed? :p
    }
    return true;
}

bool NetworkProfiler::yarpClean(float timeout) {

    if (timeout <= 0) {
        timeout = -1;
    }

    std::stringstream sstream;
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

        if (!dynamic_cast<MachineVertex*>(*itr)) {
            continue;
        } else {
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
        if (!dynamic_cast<ProcessVertex*>(*itr)) {
            continue;
        }
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
        if (!dynamic_cast<ProcessVertex*>(*itr)) {
            continue;
        }
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
                std::string label = p1.property.find("name").asString();
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
    if (level == "NORMAL") {
        return yarp::os::QosStyle::PacketPriorityNormal;
    }
    if (level == "LOW") {
        return yarp::os::QosStyle::PacketPriorityLow;
    }
    if (level == "HIGH") {
        return yarp::os::QosStyle::PacketPriorityHigh;
    }
    if (level == "CRITIC") {
        return yarp::os::QosStyle::PacketPriorityCritical;
    }
    if (level == "INVALID") {
        return yarp::os::QosStyle::PacketPriorityInvalid;
    }
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
                    } else {
                        yWarning() << "Cannot retrieve Qos property of" << v1.property.find("name").asString() << "->" << v2.property.find("name").asString();
                    }
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

std::string NetworkProfiler::PortDetails::toString() const
{
    std::ostringstream str;
    str << "port name: " << info.name << std::endl;
    str << "port ip: " << info.ip << std::endl;
    str << "port port number: " << info.port_number << std::endl;
    str << "outputs:" << std::endl;
    std::vector<ConnectedPortInfo>::const_iterator itr;
    for (itr = outputs.begin(); itr != outputs.end(); itr++) {
        str << "   + " << (*itr).port_name << " (" << (*itr).carrier << ")" << std::endl;
    }
    str << "inputs:" << std::endl;
    for (itr = inputs.begin(); itr != inputs.end(); itr++) {
        str << "   + " << (*itr).port_name << " (" << (*itr).carrier << ")" << std::endl;
    }
    str << "owner:" << std::endl;
    str << "   + name:      " << owner_process.process_name << std::endl;
    str << "   + arguments: " << owner_process.arguments << std::endl;
    str << "   + hostname:  " << owner_process.owner_machine.hostname << std::endl;
    str << "   + priority:  " << owner_process.priority << std::endl;
    str << "   + policy:    " << owner_process.policy << std::endl;
    str << "   + os:        " << owner_process.owner_machine.os << std::endl;
    str << "   + pid:       " << owner_process.pid << std::endl;
    return str.str();
}
