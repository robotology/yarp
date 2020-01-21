/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#include "RosSlave.h"
#include "RosLookup.h"
#include "TcpRosStream.h"

#include <yarp/os/Vocab.h>

#include <cstdio>
#include <string>


using namespace yarp::os;
using namespace std;

bool verbose = false;

string addPart(string t, string name, int code, Value *val, string orig, string mode="") {
    char buf[5000];
    if (mode=="length") {
        sprintf(buf,"%s %s # suggested length: %d", t.c_str(), name.c_str(), code);
    } else if (mode=="string") {
        sprintf(buf,"%s %s # value seen: \"%s\"", t.c_str(), name.c_str(), orig.c_str());
    } else if (mode=="vocab") {
        char char4 = (code>>24)%256;
        char char3 = (code>>16)%256;
        char char2 = (code>>8)%256;
        char char1 = code%256;
        string r;
        if (char1!=0) {
            r += '\''; r += char1; r += "\'*256^3";
        }
        if (char2!=0) {
            if (r!="") r += "+";
            r += '\''; r += char2; r += "\'*256^2";
        }
        if (char3!=0) {
            if (r!="") r += "+";
            r += '\''; r += char3; r += "\'*256";
        }
        if (char4!=0) {
            if (r!="") r += "+";
            r += '\''; r += char4; r += '\'';
        }
        if (r.length()==0) {
            r = "0";
        }
        sprintf(buf,"%s %s # set to %d (=%s=%s)", t.c_str(), name.c_str(), code, r.c_str(), orig.c_str());
    } else {
        if (val) {
            sprintf(buf,"%s %s # set to %s (%s)", t.c_str(), name.c_str(), val->toString().c_str(), orig.c_str());
        } else {
            sprintf(buf,"%s %s # set to %d (%s)", t.c_str(), name.c_str(), code, orig.c_str());
        }
    }
    return buf;
}

string showFormat(Bottle& b, string root) {
    string r;
    int code = b.getSpecialization();
    r += addPart("int32",root + "_tag",BOTTLE_TAG_LIST+code,nullptr,"BOTTLE_TAG_LIST+code");
    r += "\n";
    bool specialized = (code>0);
    if (code==BOTTLE_TAG_INT32) {
        r += addPart("int32[]",root,b.size(),nullptr,"length","length");
        r += "\n";
        if (b.size()<50) {
            r += " # integers seen: ";
            for (size_t i=0; i<b.size(); i++) {
                char buf[1000];
                sprintf(buf," %d",b.get(i).asInt32());
                r += buf;
            }
            r += "\n";
        }
        return r;
    }
    if (code==BOTTLE_TAG_FLOAT64) {
        r += addPart("float64[]",root,b.size(),nullptr,"length","length");
        r += "\n";
        if (b.size()<50) {
            r += " # floats seen: ";
            for (size_t i=0; i<b.size(); i++) {
                char buf[1000];
                sprintf(buf," %g",b.get(i).asFloat64());
                r += buf;
            }
            r += "\n";
        }
        return r;
    }
    r += addPart("int32",root + "_len",b.size(),nullptr,"elements in list");
    r += "\n";
    for (size_t i=0; i<b.size(); i++) {
        Value& v = b.get(i);
        char tag_name[1000];
        char val_name[1000];
        sprintf(tag_name,"%s%zu_tag", root.c_str(), i);
        sprintf(val_name,"%s%zu", root.c_str(), i);
        if (v.isVocab()) {
            if (!specialized) {
                r += addPart("int32",tag_name,BOTTLE_TAG_VOCAB,nullptr,
                             "BOTTLE_TAG_VOCAB");
                r += "\n";
            }
            r += addPart("int32",val_name,v.asInt32(),nullptr,v.toString(),"vocab");
            r += "\n";
        } else if (v.isInt32()) {
            if (!specialized) {
                r += addPart("int32",tag_name,BOTTLE_TAG_INT32,nullptr,
                             "BOTTLE_TAG_INT32");
                r += "\n";
            }
            r += addPart("int32",val_name,v.asInt32(),&v,v.toString());
            r += "\n";
        } else if (v.isFloat64()) {
            if (!specialized) {
                r += addPart("int32",tag_name,BOTTLE_TAG_FLOAT64,nullptr,
                             "BOTTLE_TAG_FLOAT64");
                r += "\n";
            }
            r += addPart("float64",val_name,v.asInt32(),&v,v.toString());
            r += "\n";
        } else if (v.isList()) {
            r += showFormat(*v.asList(), val_name);
        } else if (v.isBlob()) {
            if (!specialized) {
                r += addPart("int32",tag_name,BOTTLE_TAG_BLOB,nullptr,
                             "BOTTLE_TAG_BLOB");
                r += "\n";
            }
            r += addPart("int8[]",val_name,v.asBlobLength(),nullptr,"length","length");
        } else if (v.isString()) {
            if (!specialized) {
                r += addPart("int32",tag_name,BOTTLE_TAG_STRING,nullptr,
                             "BOTTLE_TAG_STRING");
                r += "\n";
            }
            r += addPart("string",val_name,0,nullptr,v.asString(),"string");
            r += "\n";
        } else {
            r += "IGNORED ";
            r += v.toString();
            r += "\n";
        }
    }
    return r;
}

void usage(const char *action,
           const char *msg,
           const char *example = nullptr,
           const char *explanation = nullptr) {
    printf("\n  yarpros %s\n", action);
    printf("     %s\n", msg);
    if (example!=nullptr) {
        printf("       $ yarpros %s\n", example);
    }
    if (explanation!=nullptr) {
        printf("       # %s\n", explanation);
    }
}

void show_usage() {
    printf("Welcome to yarpros.  Here are the most useful commands available:\n");
    usage("sniff out <port>","suggest .msg for output from <port> ","sniff out /grabber");
    usage("sniff in <port>","suggest .msg for input to <port> ","sniff in /grabber");
    usage("type <name>","(MOVED to yarpidl_rosmsg) generate YARP header files from <name>.msg","type PointCloud2");
    usage("help","show this help",nullptr);

    printf("\nYARP clients can use the ROS name server. If you'd prefer to stick\n");
    printf("with the native YARP name server, the following commands are useful:\n");
    usage("roscore","register port /roscore to refer to ROS_MASTER_URI","roscore");
    usage("roscore <hostname> <port number>","manually register port /roscore to point to the ros master","roscore 192.168.0.1 11311");
    usage("pub[lisher] <node> <topic>","register a ROS publisher <node>/<topic> pair as a port called <node><topic>","publisher /talker /chatter","this registers a port called /talker/chatter");
    usage("pub[lisher] <port> <node> <topic>","register a ROS publisher <node>/<topic> pair as a port called <port>","publisher /talker /talker /chatter");
    usage("sub[scriber] <node> <topic>","register a ROS subscriber <node>/<topic> pair as a port called <node><topic>","subscriber /listener /chatter","this registers a port called /listener/chatter");
    usage("sub[scriber] <yarp> <node> <topic>","register a ROS subscriber <node>/<topic> pair as a port called <port>","subscriber /listener /listener /chatter");
    usage("service <yarp> <node> <service>","register a ROS service <node>/<service> pair as a port called <port>","service /adder /add_two_ints_server /add_two_ints");
    usage("node <name>","register a ROS node name with YARP","node /talker");

    printf("\nHere are some general options:\n");
    usage("--verbose","give verbose output for debugging",nullptr);
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
    std::string ip = Contact::convertHostToIp(hostname);
    Bottle req;
    req.addString("register");
    req.addString(name);
    req.addString(carrier);
    req.addString(ip);
    req.addInt32(portnum);
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
    if (std::string(argv[1])=="help" ||
        std::string(argv[1])=="--help") {
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
    std::string tag = cmd.get(0).asString();

    // Process the command
    if (tag=="roscore") {
        if (cmd.size()>1) {
            if (!(cmd.get(1).isString()&&cmd.get(2).isInt32())) {
                fprintf(stderr,"wrong syntax, run with no arguments for help\n");
                return 1;
            }
            Bottle reply;
            register_port("/roscore", "xmlrpc",
                          cmd.get(1).asString().c_str(), cmd.get(2).asInt32(),
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
                          c.getHost().c_str(), c.getPort(),
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
        std::string yarp_port = cmd.get(1+offset).asString();
        std::string ros_port = cmd.get(2+offset).asString();
        std::string topic = cmd.get(3+offset).asString();
        if (cmd.size()==3) {
            yarp_port = ros_port + topic;
        }
        RosLookup lookup(verbose);
        if (verbose) printf("  * looking up ros node %s\n", ros_port.c_str());
        bool ok = lookup.lookupCore(ros_port);
        if (!ok) return 1;
        if (verbose) printf("  * found ros node %s\n", ros_port.c_str());
        if (verbose) printf("  * looking up topic %s\n", topic.c_str());
        ok = lookup.lookupTopic(topic);
        if (!ok) return 1;
        if (verbose) printf("  * found topic %s\n", topic.c_str());
        string carrier = "tcpros+role.pub+topic.";
        if (service) {
            carrier = "rossrv+service.";
        }
        register_port(yarp_port.c_str(),
                      (carrier+topic).c_str(),
                      lookup.hostname.c_str(),
                      lookup.portnum,
                      reply);
        printf("%s\n", reply.toString().c_str());
        return 0;
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
        std::string yarp_port = cmd.get(1+offset).asString();
        std::string ros_port = cmd.get(2+offset).asString();
        std::string topic = cmd.get(3+offset).asString();
        if (cmd.size()==3) {
            yarp_port = ros_port + topic;
        }
        RosLookup lookup(verbose);
        if (verbose) printf("  * looking up ros node %s\n", ros_port.c_str());
        bool ok = lookup.lookupCore(ros_port);
        if (!ok) return 1;
        if (verbose) printf("  * found ros node %s\n", ros_port.c_str());
        ok = register_port(yarp_port.c_str(),
                      (string("tcpros+role.sub+topic.")+topic).c_str(),
                      lookup.hostname.c_str(),
                      lookup.portnum,
                      reply);
        printf("%s\n", reply.toString().c_str());
        return ok?0:1;
    } else if (tag=="type") {
        fprintf(stderr, "MOVED: 'yarpros type' is now 'yarpidl_rosmsg'\n");
        return 1;
    } else if (tag=="sniff") {
        if (cmd.size()<2) {
            fprintf(stderr,"Show the format of a YARP bottle-compatible message in ROS syntax.\n");
            return 1;
        }
        std::string dir = cmd.get(1).asString();
        bool in = false;
        if (dir=="in") in = true;
        else if (dir=="out") in = false;
        else {
            fprintf(stderr,"Please specify one of 'in' or 'out'.\n");
            return 1;
        }
        std::string pname = cmd.get(2).asString();
        Port p;
        if (!p.open("...")) return 1;
        if (in) {
            if (!Network::connect(pname,p.getName(),"tcp+log.in")) return 1;
        } else {
            if (!Network::connect(pname,p.getName())) return 1;
        }
        Bottle b;
        p.read(b);
        string r;
        if (in&&b.get(0).asVocab()==yarp::os::createVocab('r','p','c')&&b.get(1).isList()) {

            r = showFormat(*b.get(1).asList(),"v");
        } else {
            r = showFormat(b,"v");
        }
        printf("Got message:\n%s\n",r.c_str());
        return 0;
    } else {
        fprintf(stderr,"unknown command, run with no arguments for help\n");
        return 1;
    }
    return 0;
}
