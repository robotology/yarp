/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "RosLookup.h"

#include <stdlib.h>

using namespace yarp::os;

static bool rpc(const Contact& c,
                const char *carrier,
                Bottle& writer,
                Bottle& reader,
                bool verbose) {
    ContactStyle style;
    style.quiet = false;
    style.timeout = 4;
    style.carrier = carrier;
    if (verbose) {
        printf("  > sending to [%s] %s\n", c.toString().c_str(),
               writer.toString().c_str());
    }
    bool ok = Network::write(c,writer,reader,style);
    return ok;
}

bool RosLookup::lookupCore(const ConstString& name) {
    Bottle req, reply;
    req.addString("lookupNode");
    req.addString("dummy_id");
    req.addString(name);
    rpc(getRosCoreAddress(), "xmlrpc", req, reply, verbose);
    if (reply.get(0).asInt()!=1) {
        fprintf(stderr, "Failure: %s\n", reply.toString().c_str());
        return false;
    }
    ConstString url = reply.get(2).asString();
    ConstString::size_type break1 = url.find("://",0);
    if (break1==ConstString::npos) {
        fprintf(stderr, "url not understood: %s\n", url.c_str());
        return false;
    }
    ConstString::size_type break2 = url.find(":",break1+3);
    if (break2==ConstString::npos) {
        fprintf(stderr, "url not understood: %s\n", url.c_str());
        return false;
    }
    ConstString::size_type break3 = url.find("/",break2+1);
    if (break3==ConstString::npos) {
        fprintf(stderr, "url not understood: %s\n", url.c_str());
        return false;
    }
    hostname = url.substr(break1+3,break2-break1-3);
    Value vportnum;
    vportnum.fromString(url.substr(break2+1,break3-break2-1).c_str());
    portnum = vportnum.asInt();
    if (verbose) printf("%s\n", reply.toString().c_str());
    valid = (portnum!=0);
    rpc(getRosCoreAddress(), "xmlrpc", req, reply, verbose);
    return valid;
}


bool RosLookup::lookupTopic(const ConstString& name) {
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
    rpc(c,"xmlrpc",req,reply, verbose);
    if (reply.get(0).asInt()!=1) {
        fprintf(stderr,"Failure looking up topic %s: %s\n", name.c_str(), reply.toString().c_str());
        return false;
    }
    Bottle *pref = reply.get(2).asList();
    if (pref==NULL) {
        fprintf(stderr,"Failure looking up topic %s: expected list of protocols\n", name.c_str());
        return false;
    }
    if (pref->get(0).asString()!="TCPROS") {
        fprintf(stderr,"Failure looking up topic %s: unsupported protocol %s\n", name.c_str(),
               pref->get(0).asString().c_str());
        return false;
    }
    Value hostname2 = pref->get(1);
    Value portnum2 = pref->get(2);
    hostname = hostname2.asString().c_str();
    portnum = portnum2.asInt();
    protocol = "tcpros";
    if (verbose) {
        printf("topic %s available at %s:%d\n", name.c_str(), 
               hostname.c_str(), portnum);
    }
    return true;
}

yarp::os::Contact RosLookup::getRosCoreAddressFromEnv() {
    ConstString addr = NetworkBase::getEnvironment("ROS_MASTER_URI");
    Contact c = Contact::fromString(addr.c_str());
    if (c.isValid()) {
        c.setCarrier("xmlrpc");
    }
    return c;
}

yarp::os::Contact RosLookup::getRosCoreAddress() {
    static bool checkedEnv = false;
    static Contact addr;
    if (!checkedEnv) {
        Contact c = getRosCoreAddressFromEnv();
        addr = c;
        checkedEnv = true;
    }
    if (!addr.isValid()) {
        addr = NetworkBase::queryName("/roscore");
    }
    if (!addr.isValid()) {
        fprintf(stderr,"cannot find roscore, is ROS_MASTER_URI set?\n");
        ::exit(1);
    }
    return addr;
}


