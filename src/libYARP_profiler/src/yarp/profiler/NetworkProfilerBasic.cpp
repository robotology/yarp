/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/profiler/NetworkProfilerBasic.h>

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

bool NetworkProfilerBasic::getMachinesList(const ports_detail_set& ports, machines_list& l)
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

bool NetworkProfilerBasic::getProcessesList(const ports_detail_set& ports, processes_list& l)
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

bool NetworkProfilerBasic::getPortsDetailedList(ports_detail_set& ports, bool complete)
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

void NetworkProfilerBasic::filterPortsListByIp(const ports_detail_set& in, ports_detail_set& filtered_out, std::string ip)
{
    filtered_out.clear();
    for (auto it = in.begin(); it != in.end(); it++)
    {
        if (ip != "*") {
            if (it->owner_process.owner_machine.hostname == ip) { filtered_out.push_back(*it); }
        }
    }
}

void NetworkProfilerBasic::filterPortsListByProcess(const ports_detail_set& in, ports_detail_set& filtered_out, std::string process_fullname)
{
    filtered_out.clear();
    for (auto it = in.begin(); it != in.end(); it++)
    {
        if (process_fullname != "*") {
            if (it->owner_process.process_fullname == process_fullname) { filtered_out.push_back(*it); }
        }
    }
}

void NetworkProfilerBasic::filterConnectionListByName(const connections_set& in, connections_set& filtered_out, std::string src_name, std::string dst_name)
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

void NetworkProfilerBasic::filterConnectionListByIp(const connections_set& in, connections_set& filtered_out, std::string src_portnumber, std::string dst_portnumber)
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

void NetworkProfilerBasic::filterConnectionListByPortNumber(const connections_set& in, connections_set& filtered_out, std::string src_ip, std::string dst_ip)
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

bool NetworkProfilerBasic::getPortInfo (const std::string& name, const ports_name_set& ports, PortInfo& p)
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

bool NetworkProfilerBasic::getConnectionsList(connections_set& connections)
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

bool NetworkProfilerBasic::getPortsList(ports_name_set &ports, bool complete) {
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

bool NetworkProfilerBasic::getPortDetails(const std::string& portName, PortDetails& details) {

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

bool NetworkProfilerBasic::yarpClean(float timeout) {

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

std::string NetworkProfilerBasic::PortDetails::toString() const
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
