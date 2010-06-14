// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <string>

using namespace yarp::os;
using namespace std;

#define ROSCORE_PORT "/roscore"

bool rpc(const char *target,
         const char *carrier,
         PortWriter& writer,
         PortReader& reader) {
    Port p;
    Network::setVerbosity(-1);
    p.open("...");
    bool ok = Network::connect(p.getName(),target,carrier);
    if (ok) {
        ok = p.write(writer,reader);
    }
    p.close();
    Network::setVerbosity(0);
    return ok;
}

// temporary slave
class RosSlave : public PortReader {
private:
    Port slave;
    string hostname;
    int portnum;
    Semaphore done;
public:
    RosSlave() : done(0) { 
    }

    void start(const char *hostname, int portnum) {
        this->hostname = hostname;
        this->portnum = portnum;
        slave.setReader(*this);
        slave.open("...");
    }

    void stop() {
        double delay = 0.1;
        while (!done.check()) {
            if (delay>1) {
                fprintf(stderr, "ROS may already be trying to reach this port\n");
                break;
            }
            Time::delay(delay);
            delay *= 2;
        }
        slave.close();
    }

    Contact where() {
        return slave.where();
    }

    virtual bool read(ConnectionReader& reader) {
        Bottle cmd, reply;
        bool ok = cmd.read(reader);
        if (!ok) return false;
        printf("slave got request %s\n", cmd.toString().c_str());
        reply.addInt(1);
        reply.addString("");
        Bottle& lst = reply.addList();
        lst.addString("TCPROS");
        lst.addString(hostname.c_str());
        lst.addInt(portnum);
        ConnectionWriter *writer = reader.getWriter();
        if (writer==NULL) { return false; }
        printf("replying with %s\n", reply.toString().c_str());
        reply.write(*writer);
        done.post();
        return true;
    }
};

class RosLookup {
public:
    bool valid;
    string hostname;
    int portnum;
    string protocol;

    RosLookup() {
        valid = false;
    }

    bool lookupCore(const char *name);

    bool lookupTopic(const char *name);

    string toString() {
        char buf[1000];
        sprintf(buf,"/%s:%d/", hostname.c_str(), portnum);
        return buf;
    }

};


bool RosLookup::lookupCore(const char *name) {
    Bottle req, reply;
    req.addString("lookupNode");
    req.addString("dummy_id");
    req.addString(name);
    rpc(ROSCORE_PORT, "xmlrpc", req, reply);
    if (reply.get(0).asInt()!=1) {
        fprintf(stderr, "Failure: %s\n", reply.toString().c_str());
        return false;
    }
    string url = reply.get(2).asString().c_str();
    size_t break1 = url.find_first_of("://",0);
    if (break1==string::npos) {
        fprintf(stderr, "url not understood: %s\n", url.c_str());
        return false;
    }
    size_t break2 = url.find_first_of(":",break1+3);
    if (break2==string::npos) {
        fprintf(stderr, "url not understood: %s\n", url.c_str());
        return false;
    }
    size_t break3 = url.find_first_of("/",break2+1);
    if (break3==string::npos) {
        fprintf(stderr, "url not understood: %s\n", url.c_str());
        return false;
    }
    hostname = url.substr(break1+3,break2-break1-3);
    Value vportnum;
    vportnum.fromString(url.substr(break2+1,break3-break2-1).c_str());
    portnum = vportnum.asInt();
    printf("%s\n", reply.toString().c_str());
    valid = (portnum!=0);
    rpc(ROSCORE_PORT, "xmlrpc", req, reply);
    return valid;
}


bool RosLookup::lookupTopic(const char *name) {
    if (!valid) {
        fprintf(stderr, "Need a node\n");
        return false;
    }
    Bottle req, reply;
    req.addString("requestTopic");
    req.addString("dummy_id");
    req.addString(name);
    Bottle& lst = req.addList();
    Bottle& sublst = lst.addList();
    sublst.addString("TCPROS");
    rpc(toString().c_str(),"xmlrpc",req,reply);
    if (reply.get(0).asInt()!=1) {
        printf("Failure looking up topic %s: %s\n", name, reply.toString().c_str());
        return false;
    }
    Bottle *pref = reply.get(2).asList();
    if (pref==NULL) {
        printf("Failure looking up topic %s: expected list of protocols\n", name);
        return false;
    }
    if (pref->get(0).asString()!="TCPROS") {
        printf("Failure looking up topic %s: unsupported protocol %s\n", name,
               pref->get(0).asString().c_str());
        return false;
    }
    Value hostname2 = pref->get(1);
    Value portnum2 = pref->get(2);
    hostname = hostname2.asString().c_str();
    portnum = portnum2.asInt();
    protocol = "tcpros";
    printf("topic %s available at %s:%d\n", name, hostname.c_str(), portnum);
    return true;
}


void usage(const char *action,
           const char *msg,
           const char *example = NULL) {
    printf("\n  yarpros %s\n", action);
    printf("  -- %s\n", msg);
    if (example!=NULL) {
        printf("  -- example: yarpros %s\n", example);
    }
}

bool register_port(const char *name,
                   const char *hostname,
                   int portnum,
                   PortReader& reply) {
    Bottle req;
    req.addString("register");
    req.addString(name);
    req.addString("tcp");
    req.addString(hostname);
    req.addInt(portnum);
    return Network::write(Network::getNameServerContact(),
                          req,
                          reply);
}

int main(int argc, char *argv[]) {
    if (argc<=1) {
        printf("Hello, good evening, and welcome to yarpros\n");
        printf("Here are some things you can do:\n");
        usage("roscore <hostname> <port number>","tell yarp how to reach the ros master","roscore 192.168.0.1 11311");
        usage("import <name>","import a ROS name into YARP","import /talker");
        usage("read <yarpname> <nodename> <topicname>","read to a YARP port from a ROS node's contribution to a topic","read /read /talker /chatter");
        usage("write <yarpname> <nodename> <topicname>","write from a YARP port to a ROS node's subscription to a topic","write /write /listener /chatter");
        usage("rpc <yarpname> <nodename> <servicename>","write/read from a YARP port to a ROS node's named service","rpc /rpc /add_two_ints_server /add_two_ints");
        return 0;
    }

    Network yarp;

    Bottle cmd;
    for (int i=1; i<argc; i++) {
        Value v;
        v.fromString(argv[i]);
        cmd.add(v);
    }
  
    ConstString tag = cmd.get(0).asString();

    if (tag=="roscore") {
        if (!(cmd.get(1).isString()&&cmd.get(2).isInt())) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        Bottle reply;
        register_port(ROSCORE_PORT, cmd.get(1).asString(), cmd.get(2).asInt(), 
                      reply);
        printf("%s\n", reply.toString().c_str());
        return 0;
    } else if (tag=="import") {
        if (!cmd.get(1).isString()) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        Bottle req, reply;
        RosLookup lookup;
        bool ok = lookup.lookupCore(cmd.get(1).asString());
        if (ok) {
            register_port(cmd.get(1).asString().c_str(),
                          lookup.hostname.c_str(),
                          lookup.portnum,
                          reply);
            printf("%s\n",reply.toString().c_str());
        }
        return ok?0:1;
    } else if (tag=="read") {
        if (!cmd.size()==4) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        ConstString yarp_port = cmd.get(1).asString();
        ConstString ros_port = cmd.get(2).asString();
        ConstString topic = cmd.get(3).asString();
        RosLookup lookup;
        bool ok = lookup.lookupCore(ros_port.c_str());
        if (!ok) return 1;
        ok = lookup.lookupTopic(topic.c_str());
        if (!ok) return 1;
        yarp.connect(yarp_port.c_str(),
                     lookup.toString().c_str(),
                     (string("tcpros+topic.")+topic.c_str()).c_str());
        return ok?0:1;
    } else if (tag=="write") {
        if (!cmd.size()==4) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        ConstString yarp_port = cmd.get(1).asString();
        ConstString ros_port = cmd.get(2).asString();
        ConstString topic = cmd.get(3).asString();
        RosLookup lookup;
        bool ok = lookup.lookupCore(ros_port.c_str());
        if (!ok) return 1;
        Contact addr_writer = yarp.queryName(yarp_port);
        if (!addr_writer.isValid()) {
            fprintf(stderr,"cannot find yarp port %s\n", yarp_port.c_str());
            return 1;
        }
        RosSlave slave;
        printf("Starting temporary slave\n");
        slave.start(addr_writer.getHost(),addr_writer.getPort());
        Contact addr_slave = slave.where();
        Bottle cmd, reply;
        cmd.addString("publisherUpdate");
        cmd.addString("dummy_id");
        cmd.addString(topic);
        Bottle& lst = cmd.addList();
        char buf[1000];
        sprintf(buf,"http://%s:%d/", addr_slave.getHost().c_str(), 
                addr_slave.getPort());
        lst.addString(buf);
        rpc(lookup.toString().c_str(),"xmlrpc",cmd,reply);
        printf("%s\n",reply.toString().c_str());
        slave.stop();
        return 0;
    } else if (tag=="rpc") {
        if (!cmd.size()==4) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        ConstString yarp_port = cmd.get(1).asString();
        ConstString ros_port = cmd.get(2).asString();
        ConstString service = cmd.get(3).asString();
        RosLookup lookup;
        bool ok = lookup.lookupCore(ros_port.c_str());
        if (!ok) return 1;
        ok = lookup.lookupTopic(service.c_str());
        if (!ok) return 1;
        yarp.connect(yarp_port.c_str(),
                     lookup.toString().c_str(),
                     (string("tcpros+service.")+service.c_str()).c_str());
        return ok?0:1;
    } else {
        fprintf(stderr,"unknown command, run with no arguments for help\n");
        return 1;
    }
  
    return 0;
}

