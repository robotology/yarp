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

bool verbose = false;

static Contact roscoreFromEnv() {
    ConstString addr = NetworkBase::getEnvironment("ROS_MASTER_URI");
    Contact c = Contact::fromString(addr.c_str());
    if (c.isValid()) {
        c = c.addCarrier("xmlrpc");
    }
    return c;
}

static ConstString roscore() {
    static ConstString addr = "/roscore";
    static bool checkedEnv = false;
    if (!checkedEnv) {
        Contact c = roscoreFromEnv();
        if (c.isValid()) {
            addr = c.toString();
        }
        checkedEnv = true;
    }
    //printf("ROSCORE %s\n", addr.c_str());
    return addr;
}

#define ROSCORE_PORT roscore()

bool rpc(const char *target,
         const char *carrier,
         PortWriter& writer,
         PortReader& reader) {
    Contact c = NetworkBase::queryName(target);
    if (verbose) printf("contact %s for %s\n", c.toString().c_str(), target);
    if (!c.isValid()) return false;
    ContactStyle style;
    style.quiet = false;
    style.timeout = 4;
    style.carrier = carrier;
    if (verbose) printf("RPC to %s\n", c.toString().c_str());
    bool ok = Network::write(c,writer,reader,style);
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
        if (verbose) printf("slave got request %s\n", cmd.toString().c_str());
        reply.addInt(1);
        reply.addString("");
        Bottle& lst = reply.addList();
        lst.addString("TCPROS");
        lst.addString(hostname.c_str());
        lst.addInt(portnum);
        ConnectionWriter *writer = reader.getWriter();
        if (writer==NULL) { return false; }
        if (verbose) printf("replying with %s\n", reply.toString().c_str());
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
    if (verbose) printf("%s\n", reply.toString().c_str());
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
    //printf("Sending [%s] to %s\n", req.toString().c_str(),toString().c_str());
    Contact c = Contact::fromString(toString().c_str());
    rpc(toString().c_str(),"xmlrpc",req,reply);
    if (reply.get(0).asInt()!=1) {
        fprintf(stderr,"Failure looking up topic %s: %s\n", name, reply.toString().c_str());
        return false;
    }
    Bottle *pref = reply.get(2).asList();
    if (pref==NULL) {
        fprintf(stderr,"Failure looking up topic %s: expected list of protocols\n", name);
        return false;
    }
    if (pref->get(0).asString()!="TCPROS") {
        fprintf(stderr,"Failure looking up topic %s: unsupported protocol %s\n", name,
               pref->get(0).asString().c_str());
        return false;
    }
    Value hostname2 = pref->get(1);
    Value portnum2 = pref->get(2);
    hostname = hostname2.asString().c_str();
    portnum = portnum2.asInt();
    protocol = "tcpros";
    if (verbose) {
        printf("topic %s available at %s:%d\n", name, hostname.c_str(), portnum);
    }
    return true;
}


void usage(const char *action,
           const char *msg,
           const char *example = NULL) {
    printf("\n  yarpros %s\n", action);
    printf("     %s\n", msg);
    if (example!=NULL) {
        printf("     example: yarpros %s\n", example);
    }
}

void show_usage() {
    printf("Hello, good evening, and welcome to yarpros\n");
    printf("Here are some things you can do:\n");
    usage("roscore","set yarp port /roscore to equal ROS_MASTER_URI","roscore");
    usage("roscore <hostname> <port number>","manually set yarp port /roscore to point to the ros master","roscore 192.168.0.1 11311");
    usage("import <name>","import a ROS name into YARP","import /talker");
    usage("import <yarpname> <nodename> <topicname>","import a ROS node/topic pair as a port","import /talker /talker /chatter");
    usage("read <yarpname> <nodename> <topicname>","read to a YARP port from a ROS node's contribution to a topic","read /read /talker /chatter");
    usage("write <yarpname> <nodename> <topicname>","write from a YARP port to a ROS node's subscription to a topic","write /write /listener /chatter");
    usage("rpc <yarpname> <nodename> <servicename>","write/read from a YARP port to a ROS node's named service","rpc /rpc /add_two_ints_server /add_two_ints");
    printf("Here are some general options:\n");
    usage("--verbose","give verbose output for debugging",NULL);
}

bool register_port(const char *name,
                   const char *carrier,
                   const char *hostname,
                   int portnum,
                   PortReader& reply) {
    Bottle req;
    req.addString("register");
    req.addString(name);
    req.addString(carrier);
    req.addString(hostname);
    req.addInt(portnum);
    return Network::write(Network::getNameServerContact(),
                          req,
                          reply);
}

int main(int argc, char *argv[]) {
    if (argc<=1) {
        show_usage();
        return 0;
    }

    Network yarp;

    // Read in the command sequence
    Bottle cmd;
    for (int i=1; i<argc; i++) {
        Value v;
        v.fromString(argv[i]);
        cmd.add(v);
    }

    // Check for flags
    Property options;
    options.fromCommand(argc,argv);
    verbose = false;
    if (options.check("verbose")) {
        verbose = true;
    }
  
    // Get the command tag
    ConstString tag = cmd.get(0).asString();

    // Process the command
    if (tag=="roscore") {
        if (cmd.size()>1) {
            if (!(cmd.get(1).isString()&&cmd.get(2).isInt())) {
                fprintf(stderr,"wrong syntax, run with no arguments for help\n");
                return 1;
            }
            Bottle reply;
            register_port("/roscore", "xmlrpc",
                          cmd.get(1).asString(), cmd.get(2).asInt(), 
                          reply);
            printf("%s\n", reply.toString().c_str());
        } else {
            Bottle reply;
            Contact c = roscoreFromEnv();
            if (!c.isValid()) {
                fprintf(stderr,"cannot find roscore, is ROS_MASTER_URI set?\n");
                return 1;
            }
            register_port("/roscore", "xmlrpc",
                          c.getHost(), c.getPort(), 
                          reply);
            printf("%s\n", reply.toString().c_str());
        }
        return 0;
    } else if (tag=="import") {
        Bottle req, reply;
        if (cmd.size()==2) {
            if (!cmd.get(1).isString()) {
                fprintf(stderr,"wrong syntax, run with no arguments for help\n");
                return 1;
            }
            RosLookup lookup;
            bool ok = lookup.lookupCore(cmd.get(1).asString());
            if (ok) {
                register_port(cmd.get(1).asString().c_str(),
                              "xmlrpc",
                              lookup.hostname.c_str(),
                              lookup.portnum,
                              reply);
                printf("%s\n",reply.toString().c_str());
            }
            return ok?0:1;
        } else if (cmd.size()==4) {
            ConstString yarp_port = cmd.get(1).asString();
            ConstString ros_port = cmd.get(2).asString();
            ConstString topic = cmd.get(3).asString();
            RosLookup lookup;
            if (verbose) printf("  * looking up ros node %s\n", ros_port.c_str());
            bool ok = lookup.lookupCore(ros_port.c_str());
            if (!ok) return 1;
            if (verbose) printf("  * found ros node %s\n", ros_port.c_str());
            if (verbose) printf("  * looking up topic %s\n", topic.c_str());
            ok = lookup.lookupTopic(topic.c_str());
            if (!ok) return 1;
            if (verbose) printf("  * found topic %s\n", topic.c_str());
            register_port(yarp_port.c_str(),
                          (string("tcpros+topic.")+topic.c_str()).c_str(),
                          lookup.hostname.c_str(),
                          lookup.portnum,
                          reply);
            return ok?0:1;
            
        } else {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
    } else if (tag=="read") {
        if (!cmd.size()==4) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        ConstString yarp_port = cmd.get(1).asString();
        ConstString ros_port = cmd.get(2).asString();
        ConstString topic = cmd.get(3).asString();
        RosLookup lookup;
        if (verbose) printf("  * looking up ros node %s\n", ros_port.c_str());
        bool ok = lookup.lookupCore(ros_port.c_str());
        if (!ok) return 1;
        if (verbose) printf("  * found ros node %s\n", ros_port.c_str());
        if (verbose) printf("  * looking up topic %s\n", topic.c_str());
        ok = lookup.lookupTopic(topic.c_str());
        if (!ok) return 1;
        if (verbose) printf("  * found topic %s\n", topic.c_str());
        ConstString src = lookup.toString().c_str();
        ConstString dest = yarp_port;
        ConstString carrier = (string("tcpros+topic.")+topic.c_str()).c_str();
        printf("yarp connect %s %s %s\n",
               src.c_str(), dest.c_str(), carrier.c_str());
        yarp.connect(src.c_str(),dest.c_str(),carrier.c_str());
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
        if (verbose) printf("Starting temporary slave\n");
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

