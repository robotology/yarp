// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/all.h>
#include "RosSlave.h"
#include "RosLookup.h"

#include <string>


using namespace yarp::os;
using namespace std;

bool verbose = false;

void usage(const char *action,
           const char *msg,
           const char *example = NULL,
           const char *explanation = NULL) {
    printf("\n  yarpros %s\n", action);
    printf("     %s\n", msg);
    if (example!=NULL) {
        printf("       $ yarpros %s\n", example);
    }
    if (explanation!=NULL) {
        printf("       # %s\n", explanation);
    }
}

void show_usage() {
    printf("Welcome to yarpros.\n");
    usage("roscore","register port /roscore to refer to ROS_MASTER_URI","roscore");
    usage("roscore <hostname> <port number>","manually register port /roscore to point to the ros master","roscore 192.168.0.1 11311");
    usage("pub[lisher] <node> <topic>","register a ROS publisher <node>/<topic> pair as a port called <node><topic>","publisher /talker /chatter","this registers a port called /talker/chatter");
    usage("pub[lisher] <port> <node> <topic>","register a ROS publisher <node>/<topic> pair as a port called <port>","publisher /talker /talker /chatter");
    usage("sub[scriber] <node> <topic>","register a ROS subscriber <node>/<topic> pair as a port called <node><topic>","subscriber /listener /chatter","this registers a port called /listener/chatter");
    usage("sub[scriber] <yarp> <node> <topic>","register a ROS subscriber <node>/<topic> pair as a port called <port>","subscriber /listener /listener /chatter");
    usage("service <yarp> <node> <service>","register a ROS service <node>/<service> pair as a port called <port>","service /adder /add_two_ints_server /add_two_ints");
    usage("node <name>","register a ROS node name with YARP","node /talker");

    // READ and WRITE not needed any more - can be done via registration
    //usage("read <yarpname> <nodename> <topicname>","read to a YARP port from a ROS node's contribution to a topic","read /read /talker /chatter");
    //usage("write <yarpname> <nodename> <topicname>","write from a YARP port to a ROS node's subscription to a topic","write /write /listener /chatter");

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
        if (argv[i][0]!='-') {
            cmd.add(v);
        }
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
            Contact c = RosLookup::getRosCoreAddressFromEnv();
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
    } else if (tag=="node") {
        Bottle req, reply;
        if (cmd.size()!=2) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        if (!cmd.get(1).isString()) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        RosLookup lookup(verbose);
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
    } else if (tag=="publisher"||tag=="pub"||tag=="service"||tag=="srv") {
        bool service = (tag=="service"||tag=="srv");
        Bottle req, reply;
        if (cmd.size()!=3 && cmd.size()!=4) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        int offset = 0;
        if (cmd.size()==3) {
            offset = -1;
        }
        ConstString yarp_port = cmd.get(1+offset).asString();
        ConstString ros_port = cmd.get(2+offset).asString();
        ConstString topic = cmd.get(3+offset).asString();
        if (cmd.size()==3) {
            yarp_port = ros_port + topic;
        }
        RosLookup lookup(verbose);
        if (verbose) printf("  * looking up ros node %s\n", ros_port.c_str());
        bool ok = lookup.lookupCore(ros_port.c_str());
        if (!ok) return 1;
        if (verbose) printf("  * found ros node %s\n", ros_port.c_str());
        if (verbose) printf("  * looking up topic %s\n", topic.c_str());
        ok = lookup.lookupTopic(topic.c_str());
        if (!ok) return 1;
        if (verbose) printf("  * found topic %s\n", topic.c_str());
        string carrier = "tcpros+role.pub+topic.";
        if (service) {
            carrier = "rossrv+service.";
        }
        register_port(yarp_port.c_str(),
                      (carrier+topic.c_str()).c_str(),
                      lookup.hostname.c_str(),
                      lookup.portnum,
                      reply);
        printf("%s\n", reply.toString().c_str());
        return ok?0:1;        
    } else if (tag=="subscriber"||tag=="sub") {
        Bottle req, reply;
        if (cmd.size()!=3 && cmd.size()!=4) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        int offset = 0;
        if (cmd.size()==3) {
            offset = -1;
        }
        ConstString yarp_port = cmd.get(1+offset).asString();
        ConstString ros_port = cmd.get(2+offset).asString();
        ConstString topic = cmd.get(3+offset).asString();
        if (cmd.size()==3) {
            yarp_port = ros_port + topic;
        }
        RosLookup lookup(verbose);
        if (verbose) printf("  * looking up ros node %s\n", ros_port.c_str());
        bool ok = lookup.lookupCore(ros_port.c_str());
        if (!ok) return 1;
        if (verbose) printf("  * found ros node %s\n", ros_port.c_str());
        register_port(yarp_port.c_str(),
                      (string("tcpros+role.sub+topic.")+topic.c_str()).c_str(),
                      lookup.hostname.c_str(),
                      lookup.portnum,
                      reply);
        printf("%s\n", reply.toString().c_str());
        return ok?0:1;
    } else {
        fprintf(stderr,"unknown command, run with no arguments for help\n");
        return 1;
    }

    /*
    else if (tag=="read") {
        if (!cmd.size()==4) {
            fprintf(stderr,"wrong syntax, run with no arguments for help\n");
            return 1;
        }
        ConstString yarp_port = cmd.get(1).asString();
        ConstString ros_port = cmd.get(2).asString();
        ConstString topic = cmd.get(3).asString();
        RosLookup lookup(verbose);
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
        RosLookup lookup(verbose);
        bool ok = lookup.lookupCore(ros_port.c_str());
        if (!ok) return 1;
        Contact addr_writer = yarp.queryName(yarp_port);
        if (!addr_writer.isValid()) {
            fprintf(stderr,"cannot find yarp port %s\n", yarp_port.c_str());
            return 1;
        }
        RosSlave slave(verbose);
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
        rpc(lookup.toContact("xmlrpc"),"xmlrpc",cmd,reply);
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
        RosLookup lookup(verbose);
        bool ok = lookup.lookupCore(ros_port.c_str());
        if (!ok) return 1;
        ok = lookup.lookupTopic(service.c_str());
        if (!ok) return 1;
        yarp.connect(yarp_port.c_str(),
                     lookup.toString().c_str(),
                     (string("tcpros+service.")+service.c_str()).c_str());
        return ok?0:1;
    }
    */
  
    return 0;
}

