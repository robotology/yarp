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
#include "RosType.h"
#include "RosTypeCodeGenYarp.h"

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

    printf("Here are some general options:\n");
    usage("--verbose","give verbose output for debugging",NULL);
}

bool announce_port(const char *name,
                   PortReader& reply) {
    Bottle req;
    req.addString("announce");
    req.addString(name);
    return Network::write(Network::getNameServerContact(),
                          req,
                          reply);
}

bool register_port(const char *name,
                   const char *carrier,
                   const char *hostname,
                   int portnum,
                   PortReader& reply) {
    ConstString ip = Contact::convertHostToIp(hostname);
    Bottle req;
    req.addString("register");
    req.addString(name);
    req.addString(carrier);
    req.addString(ip);
    req.addInt(portnum);
    bool ok = Network::write(Network::getNameServerContact(),
                             req,
                             reply);
    if (ok) {
        Bottle reply2;
        announce_port(name,reply2);
    }
    return ok;
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
    } else if (tag=="type") {
        RosTypeSearch env;
        RosType t;
        if (cmd.size()!=2) {
            fprintf(stderr,"This command is not ready to be used.\n");
            fprintf(stderr,"It reads a ROS .msg file, and then... does nothing with it.\n");
            return 1;
        }
        ConstString tname = cmd.get(1).asString();
        t.read(tname,env);
        t.show();
        RosTypeCodeGenYarp gen;
        RosTypeCodeGenState state;
        t.emitType(gen,state);
        printf("\n");
    } else {
        fprintf(stderr,"unknown command, run with no arguments for help\n");
        return 1;
    }  
    return 0;
}

