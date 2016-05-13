#include "NetworkProfiler.h"

#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/Port.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/Carrier.h>


using namespace std;
using namespace yarp::os;
using namespace yarp::graph;


NetworkProfiler::ProgressCallback* NetworkProfiler::progCallback = NULL;

bool NetworkProfiler::yarpNameList(ports_name_set &ports) {
    ports.clear();

    ContactStyle style;
    style.quiet = true;
    style.timeout = 3.0;
    string nameserver = NetworkBase::getNameServerName();
    Bottle msg, reply;
    msg.addString("bot");
    msg.addString("list");
    if(!NetworkBase::write(Contact::byName(nameserver), msg, reply, style)) {
        yError() << "Cannot write to yarp name server";
        return false;
    }

    if(reply.size() == 0) {
        yError() << "Empty reply from yarp name server";
        return false;
    }

    for (int i=1; i<reply.size(); i++) {
        Bottle *entry = reply.get(i).asList();
        if(entry != NULL) {
            ConstString portname = entry->check("name", Value("")).asString();
            if (portname != "" && portname != "fallback" && portname != nameserver) {
                Contact c = Contact::byConfig(*entry);
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
        return false;
    }
    for(int i=0; i<reply.size(); i++) {
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
        return false;
    }
    for(int i=0; i<reply.size(); i++) {
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
        return false;
    }

    Property* process = reply.find("process").asDict();
    if(!process)
        yWarning()<<"Cannot find 'process' property of port "<<portName;
    else {
        info.owner.name = process->find("name").asString();
        info.owner.arguments = process->find("arguments").asString();
        info.owner.pid = process->find("pid").asInt();
        info.owner.priority = process->find("priority").asInt();
        info.owner.policy = process->find("policy").asInt();
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


bool NetworkProfiler::creatNetworkGraph(ports_detail_set details, yarp::graph::Graph& graph) {
    ports_detail_iterator itr = details.begin();

    // adding the ports and processor nodes

    unsigned int itr_count = 0;

    if(NetworkProfiler::progCallback)
        NetworkProfiler::progCallback->onProgress(0);

    for(; itr!=details.end(); itr++) {
        PortDetails info = (*itr);

        // port node
        PortVertex* port = new PortVertex(info.name);
        if(!info.inputs.size() && !info.outputs.size())
            port->property.put("orphan", true);
        graph.insert(*port);

        //processor node (owner)
        ProcessVertex* owner = new ProcessVertex(info.owner.pid, info.owner.hostname);
        //prop.clear();
        owner->property.put("name", info.owner.name);
        owner->property.put("arguments", info.owner.arguments);
        owner->property.put("hostname", info.owner.hostname);
        owner->property.put("priority", info.owner.priority);
        owner->property.put("policy", info.owner.policy);
        owner->property.put("os", info.owner.os);
        graph.insert(*owner);

        // create connection between ports and its owner
        if(info.inputs.size()) {
            graph.insertEdge(*port, *owner, Property("(type ownership) (dir out)"));
            port->property.put("dir", "in");            
        }
        if(info.outputs.size()) {
            graph.insertEdge(*owner, *port, Property("(type ownership) (dir in)"));
            port->property.put("dir", "out");        
        }
        port->setOwner(owner);

        // TODO: shall we add unconnected ports?!!
        if(!info.inputs.size() && !info.outputs.size())
            graph.insertEdge(*owner, *port, Property("(type ownership) (dir unknown)"));

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
        for(int i=0; i<info.outputs.size(); i++) {
            ConnectionInfo cnn = info.outputs[i];
            pvertex_iterator vi2 = graph.find(PortVertex(cnn.name));
            yAssert(vi2 != graph.vertices().end());
            //yInfo()<<"connecting "<<(*vi1)->property.find("name").asString()<<"->"<<(*vi2)->property.find("name").asString();
            Property edge_prop("(type connection)");
            edge_prop.put("carrier", cnn.carrier);
            graph.insertEdge(vi1, vi2, edge_prop);
        }
        // calculate progress
        if(NetworkProfiler::progCallback) {
            NetworkProfiler::progCallback->onProgress((unsigned int) (++itr_count/((float)(details.size()*2)) * 100.0) );
        }
    }

    NetworkProfiler::progCallback->onProgress(100); // is it really needed? :p
    return true;
}

bool NetworkProfiler::yarpClean(float timeout) {

    if (timeout <= 0)
        timeout = -1;

    NetworkProfiler::ports_name_set ports;
    NetworkProfiler::yarpNameList(ports);
    for(int i=0; i<ports.size(); i++) {
        std::string portname = ports[i].find("name").asString();
        Contact addr = Contact::byConfig(ports[i]);
        if (addr.isValid()) {
            if (timeout>=0)
                addr.setTimeout(timeout);
            /**
             * TODO: fix the yarp clean
             */

            /*
            OutputProtocol *out = Carriers::connect(addr);
            if (out == NULL)
                NetworkBase::unregisterName(portname);
            else
                delete out;
            */
        }
    }

    string serverName = NetworkBase::getNameServerName();
    Bottle cmd("gc"), reply;
    NetworkBase::write(serverName, cmd, reply);
    return true;
}
