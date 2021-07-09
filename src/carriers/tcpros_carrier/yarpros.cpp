/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "RosSlave.h"
#include "RosLookup.h"
#include "TcpRosStream.h"

#include <yarp/os/Vocab.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <cstdio>
#include <string>

using namespace yarp::os;
using namespace std;


namespace {
void print_callback(yarp::os::Log::LogType type,
                    const char* msg,
                    const char* file,
                    const unsigned int line,
                    const char* func,
                    double systemtime,
                    double networktime,
                    double externaltime,
                    const char* comp_name)
{
    YARP_UNUSED(type);
    YARP_UNUSED(file);
    YARP_UNUSED(line);
    YARP_UNUSED(func);
    YARP_UNUSED(systemtime);
    YARP_UNUSED(networktime);
    YARP_UNUSED(externaltime);
    YARP_UNUSED(comp_name);
    static const char* err_str = "[ERROR] ";
    static const char* warn_str = "[WARNING] ";
    static const char* no_str = "";
    printf("%s%s\n",
           ((type == yarp::os::Log::ErrorType) ? err_str : ((type == yarp::os::Log::WarningType) ? warn_str : no_str)),
           msg);
}

YARP_LOG_COMPONENT(YARPROS,
                   "yarp.carrier.tcpros.yarpros",
                   yarp::os::Log::InfoType,
                   yarp::os::Log::LogTypeReserved,
                   print_callback,
                   nullptr)
}


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
        if (v.isVocab32()) {
            if (!specialized) {
                r += addPart("int32",tag_name,BOTTLE_TAG_VOCAB32,nullptr,
                             "BOTTLE_TAG_VOCAB32");
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
    yCInfo(YARPROS, "\n  yarpros %s", action);
    yCInfo(YARPROS, "     %s\n", msg);
    if (example!=nullptr) {
        yCInfo(YARPROS, "       $ yarpros %s", example);
    }
    if (explanation!=nullptr) {
        yCInfo(YARPROS, "       # %s", explanation);
    }
}

void show_usage() {
    yCInfo(YARPROS, "Welcome to yarpros.  Here are the most useful commands available:");
    usage("sniff out <port>","suggest .msg for output from <port> ","sniff out /grabber");
    usage("sniff in <port>","suggest .msg for input to <port> ","sniff in /grabber");
    usage("type <name>","(MOVED to yarpidl_rosmsg) generate YARP header files from <name>.msg","type PointCloud2");
    usage("help","show this help",nullptr);

    yCInfo(YARPROS);
    yCInfo(YARPROS, "YARP clients can use the ROS name server. If you'd prefer to stick");
    yCInfo(YARPROS, "with the native YARP name server, the following commands are useful:");
    usage("roscore","register port /roscore to refer to ROS_MASTER_URI","roscore");
    usage("roscore <hostname> <port number>","manually register port /roscore to point to the ros master","roscore 192.168.0.1 11311");
    usage("pub[lisher] <node> <topic>","register a ROS publisher <node>/<topic> pair as a port called <node><topic>","publisher /talker /chatter","this registers a port called /talker/chatter");
    usage("pub[lisher] <port> <node> <topic>","register a ROS publisher <node>/<topic> pair as a port called <port>","publisher /talker /talker /chatter");
    usage("sub[scriber] <node> <topic>","register a ROS subscriber <node>/<topic> pair as a port called <node><topic>","subscriber /listener /chatter","this registers a port called /listener/chatter");
    usage("sub[scriber] <yarp> <node> <topic>","register a ROS subscriber <node>/<topic> pair as a port called <port>","subscriber /listener /listener /chatter");
    usage("service <yarp> <node> <service>","register a ROS service <node>/<service> pair as a port called <port>","service /adder /add_two_ints_server /add_two_ints");
    usage("node <name>","register a ROS node name with YARP","node /talker");
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

    if (options.check("verbose")) {
        yCWarning(YARPROS, "The 'verbose' option is deprecated");
    }

    // Get the command tag
    std::string tag = cmd.get(0).asString();

    // Process the command
    if (tag=="roscore") {
        if (cmd.size()>1) {
            if (!(cmd.get(1).isString()&&cmd.get(2).isInt32())) {
                yCError(YARPROS, "wrong syntax, run with no arguments for help");
                return 1;
            }
            Bottle reply;
            register_port("/roscore", "xmlrpc",
                          cmd.get(1).asString().c_str(), cmd.get(2).asInt32(),
                          reply);
            yCInfo(YARPROS, "%s", reply.toString().c_str());
        } else {
            Bottle reply;
            Contact c = RosLookup::getRosCoreAddressFromEnv();
            if (!c.isValid()) {
                yCError(YARPROS, "cannot find roscore, is ROS_MASTER_URI set?");
                return 1;
            }
            register_port("/roscore", "xmlrpc",
                          c.getHost().c_str(), c.getPort(),
                          reply);
            yCInfo(YARPROS, "%s", reply.toString().c_str());
        }
        return 0;
    } else if (tag=="node") {
        Bottle req, reply;
        if (cmd.size()!=2) {
            yCError(YARPROS, "wrong syntax, run with no arguments for help");
            return 1;
        }
        if (!cmd.get(1).isString()) {
            yCError(YARPROS, "wrong syntax, run with no arguments for help");
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
            yCInfo(YARPROS, "%s",reply.toString().c_str());
        }
        return ok?0:1;
    } else if (tag=="publisher"||tag=="pub"||tag=="service"||tag=="srv") {
        bool service = (tag=="service"||tag=="srv");
        Bottle req, reply;
        if (cmd.size()!=3 && cmd.size()!=4) {
            yCError(YARPROS, "wrong syntax, run with no arguments for help");
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
        RosLookup lookup;
        yCDebug(YARPROS, "  * looking up ros node %s", ros_port.c_str());
        bool ok = lookup.lookupCore(ros_port);
        if (!ok) return 1;
        yCDebug(YARPROS, "  * found ros node %s", ros_port.c_str());
        yCDebug(YARPROS, "  * looking up topic %s", topic.c_str());
        ok = lookup.lookupTopic(topic);
        if (!ok) return 1;
        yCDebug(YARPROS, "  * found topic %s", topic.c_str());
        string carrier = "tcpros+role.pub+topic.";
        if (service) {
            carrier = "rossrv+service.";
        }
        register_port(yarp_port.c_str(),
                      (carrier+topic).c_str(),
                      lookup.hostname.c_str(),
                      lookup.portnum,
                      reply);
        yCInfo(YARPROS, "%s", reply.toString().c_str());
        return 0;
    } else if (tag=="subscriber"||tag=="sub") {
        Bottle req, reply;
        if (cmd.size()!=3 && cmd.size()!=4) {
            yCError(YARPROS, "wrong syntax, run with no arguments for help");
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
        RosLookup lookup;
        yCDebug(YARPROS, "  * looking up ros node %s", ros_port.c_str());
        bool ok = lookup.lookupCore(ros_port);
        if (!ok) return 1;
        yCDebug(YARPROS, "  * found ros node %s", ros_port.c_str());
        ok = register_port(yarp_port.c_str(),
                      (string("tcpros+role.sub+topic.")+topic).c_str(),
                      lookup.hostname.c_str(),
                      lookup.portnum,
                      reply);
        yCInfo(YARPROS, "%s", reply.toString().c_str());
        return ok?0:1;
    } else if (tag=="type") {
        yCError(YARPROS, "MOVED: 'yarpros type' is now 'yarpidl_rosmsg'");
        return 1;
    } else if (tag=="sniff") {
        if (cmd.size()<2) {
            yCError(YARPROS, "Show the format of a YARP bottle-compatible message in ROS syntax.");
            return 1;
        }
        std::string dir = cmd.get(1).asString();
        bool in = false;
        if (dir=="in") in = true;
        else if (dir=="out") in = false;
        else {
            yCError(YARPROS, "Please specify one of 'in' or 'out'.");
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
        if (in&&b.get(0).asVocab32()==yarp::os::createVocab32('r','p','c')&&b.get(1).isList()) {

            r = showFormat(*b.get(1).asList(),"v");
        } else {
            r = showFormat(b,"v");
        }
        yCInfo(YARPROS, "Got message: [%s]",r.c_str());
        return 0;
    } else {
        yCError(YARPROS, "unknown command, run with no arguments for help");
        return 1;
    }
    return 0;
}
