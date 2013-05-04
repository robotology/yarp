// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/RosNameSpace.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/DummyConnector.h>

using namespace yarp::os;
using namespace yarp::os::impl;

RosNameSpace::RosNameSpace(const Contact& contact) {
    this->contact = contact;
}

RosNameSpace::~RosNameSpace() {
}

Contact RosNameSpace::getNameServerContact() const {
    return contact;
}

Contact RosNameSpace::queryName(const char *name) {
    ConstString full = name;
    ConstString node = full;
    ConstString srv = "";
    int srv_idx = full.find("#");
    if (srv_idx!=ConstString::npos) {
        node = full.substr(0,srv_idx);
        srv = full.substr(srv_idx+1,full.length());
    }

    Bottle cmd,reply;
    cmd.addString("lookupNode");
    cmd.addString("dummy_id");
    cmd.addString(toRosNodeName(node));
    NetworkBase::write(getNameServerContact(),
                       cmd, reply);
    //printf("query: sent %s, got %s\n", cmd.toString().c_str(), reply.toString().c_str());
    Contact contact;
    if (reply.get(0).asInt()!=1) {
        return contact;
    }
    contact = Contact::fromString(reply.get(2).asString());
    contact = contact.addCarrier("xmlrpc");
    contact = contact.addName(name);

    if (srv == "") return contact;

    // we need to go a step further and find a service

    contact = contact.addName("");
    printf("Working with %s\n", contact.toString().c_str());
    Bottle req;
    req.addString("requestTopic");
    req.addString("dummy_id");
    req.addString(srv);
    Bottle& lst = req.addList();
    Bottle& sublst = lst.addList();
    sublst.addString("TCPROS");
    if (!NetworkBase::write(contact,req,reply,false,true)) {
        fprintf(stderr,"Failure looking up service %s: %s\n", srv.c_str(), reply.toString().c_str());
        return Contact();
    }
    Bottle *pref = reply.get(2).asList();
    if (pref==NULL) {
        fprintf(stderr,"Failure looking up service %s: expected list of protocols\n", srv.c_str());
        return Contact();
    }
    if (pref->get(0).asString()!="TCPROS") {
        fprintf(stderr,"Failure looking up service %s: unsupported protocol %s\n", srv.c_str(),
                pref->get(0).asString().c_str());
        return Contact();
    }
    Value hostname2 = pref->get(1);
    Value portnum2 = pref->get(2);
    contact = contact.addSocket((ConstString("rossrv+service.")+srv + "+raw.2").c_str(),
                                hostname2.asString().c_str(),
                                portnum2.asInt());

    printf("GOT %s\n", contact.toString().c_str());

    return contact;
}

Contact RosNameSpace::registerName(const char *name) {
    fprintf(stderr,"ROS name server does not do 'raw' registrations.\n");
    fprintf(stderr,"Use [Buffered]Port::open to get complete registrations.\n");
    exit(1);

    return Contact();
}

Contact RosNameSpace::registerContact(const Contact& contact) {
    ConstString full = contact.getName();
    ConstString name = full;
    int pub_idx = name.find("+#");
    int sub_idx = name.find("-#");

    ConstString node = "";
    ConstString pub = "";
    ConstString sub = "";
    if (pub_idx!=ConstString::npos) {
        node = name.substr(0,pub_idx);
        pub = name.substr(pub_idx+2,name.length());
        YARP_SPRINTF1(Logger::get(),debug,"Publish to %s",pub.c_str());
    }
    if (sub_idx!=ConstString::npos) {
        node = name.substr(0,sub_idx);
        sub = name.substr(sub_idx+2,name.length());
        YARP_SPRINTF1(Logger::get(),debug,"Subscribe to %s",sub.c_str());
    }
    if (node=="") {
        node = name;
    }
    YARP_SPRINTF4(Logger::get(),debug,"Name [%s] Node [%s] sub [%s] pub [%s]",
                  name.c_str(), node.c_str(), sub.c_str(), pub.c_str());

    {
        Bottle cmd, reply;
        // for ROS, we fake port name registrations by
        // registering them as nodes that publish to an arbitrary
        // topic
        cmd.clear();
        cmd.addString("registerPublisher");
        cmd.addString(toRosNodeName(node));
        //cmd.addString(toRosNodeName(contact.getName().c_str()));
        cmd.addString("/yarp/registration");
        cmd.addString("*");
        Contact c = rosify(contact);
        cmd.addString(c.toString());
        //printf("Writing to %s\n",getNameServerContact().toString().c_str());
        bool ok = NetworkBase::write(getNameServerContact(),
                                     cmd, reply);
        if (!ok) return Contact();
    }

    if (pub!="") {
        NetworkBase::connect(node, ConstString("topic:/") + pub);
    }
    if (sub!="") {
        NetworkBase::connect(ConstString("topic:/") + sub, node);
    }

    /*
    // There's a mismatch in how topics work in YARP and ROS.
    // For YARP, pubs/subs to topics are usually managed
    // externally, whereas ROS expects them to be set up
    // in source code.  Hence, YARP ports need a way to
    // remember topic pub/subs across restarts, since
    // the name server doesn't expect to manage them.
    //
    // Workaround: when unregistering, we don't "clean
    // up" pub/subs for YARP ports, and when reregistering,
    // we capture and reaffirm those old pub/subs (otherwise
    // roscore deletes them)

    Bottle cmd, reply, state;
    cmd.addString("getSystemState");
    cmd.addString(contact.getName().c_str());
    bool ok = NetworkBase::write(getNameServerContact(),
                                 cmd, state);
    if (!ok) {
        return Contact();
    }
    YARP_SPRINTF2(Logger::get(),debug,
                  "ROS registration: sent %s, got %s",
                  cmd.toString().c_str(),
                  state.toString().c_str());
    // for ROS, we fake port name registrations by
    // registering them as nodes that publish to an arbitrary
    // topic
    cmd.clear();
    cmd.addString("registerPublisher");
    cmd.addString(contact.getName().c_str());
    cmd.addString("/yarp/registration");
    cmd.addString("*");
    Contact c = rosify(contact);
    cmd.addString(c.toString());
    //printf("Writing to %s\n",getNameServerContact().toString().c_str());
    ok = NetworkBase::write(getNameServerContact(),
                            cmd, reply);
    if (!ok) return Contact();
    YARP_SPRINTF2(Logger::get(),debug,
                  "ROS registration: sent %s, got %s",
                  cmd.toString().c_str(),
                  reply.toString().c_str());

    // recover pubs/subs
    Bottle *lst = state.get(2).asList();
    if (lst!=NULL) {
        Bottle *pubs = lst->get(0).asList();
        Bottle *subs = lst->get(1).asList();
        if (pubs!=NULL && subs!=NULL) {
            YARP_SPRINTF1(Logger::get(),debug,
                          "  pubs: %s",
                          pubs->toString().c_str());
            YARP_SPRINTF1(Logger::get(),debug,
                          "  subs: %s",
                          subs->toString().c_str());
            for (int i=0; i<pubs->size();i++) {
                Bottle *pub = pubs->get(i).asList();
                if (pub==NULL) continue;
                ConstString key = pub->get(0).asString();
                //if (key=="yarp") continue;
                Bottle *item = pub->get(1).asList();
                if (item==NULL) continue;
                for (int j=0; j<item->size(); j++) {
                    if (item->get(j).asString()==contact.getName()) {
                        YARP_SPRINTF1(Logger::get(),debug,
                                      "  pub match: %s",
                                      key.c_str());
                        NetworkBase::connect(contact.getName(),ConstString("topic:/") + key);
                    }
                }
            }
            for (int i=0; i<subs->size();i++) {
                Bottle *sub = subs->get(i).asList();
                if (sub==NULL) continue;
                ConstString key = sub->get(0).asString();
                //if (key=="yarp") continue;
                Bottle *item = sub->get(1).asList();
                if (item==NULL) continue;
                for (int j=0; j<item->size(); j++) {
                    if (item->get(j).asString()==contact.getName()) {
                        YARP_SPRINTF1(Logger::get(),debug,
                                      "  sub match: %s",
                                      key.c_str());
                        NetworkBase::connect(ConstString("topic:/") + key,contact.getName());
                    }
                }
            }
        }
    }
    */

    return contact.addName(node);
}

Contact RosNameSpace::unregisterName(const char *rname) {
    ConstString full = rname;
    ConstString name = full;
    int pub_idx = name.find("+#");
    int sub_idx = name.find("-#");

    ConstString node = "";
    ConstString pub = "";
    ConstString sub = "";
    if (pub_idx!=ConstString::npos) {
        node = name.substr(0,pub_idx);
        pub = name.substr(pub_idx+2,name.length());
        //printf("Publish to %s\n",pub.c_str());
    }
    if (sub_idx!=ConstString::npos) {
        node = name.substr(0,sub_idx);
        sub = name.substr(sub_idx+2,name.length());
        //printf("Subscribe to %s\n",sub.c_str());
    }
    if (node=="") {
        node = name;
    }
    YARP_SPRINTF3(Logger::get(),debug,"Name [%s] sub [%s] pub [%s]\n",
                  name.c_str(), sub.c_str(), pub.c_str());

    if (pub!="") {
        NetworkBase::disconnect(full,ConstString("topic:/") + pub);
    }
    if (sub!="") {
        NetworkBase::disconnect(ConstString("topic:/") + sub, full);
    }

    Contact contact = NetworkBase::queryName(rname);
    Bottle cmd,reply;
    cmd.addString("unregisterPublisher");
    cmd.addString(rname);
    cmd.addString("/yarp/registration");
    Contact c = Contact::bySocket("http",contact.getHost().c_str(),
                                  contact.getPort());
    cmd.addString(c.toString());
    //printf("Writing to %s\n",getNameServerContact().toString().c_str());
    bool ok = NetworkBase::write(getNameServerContact(),
                                 cmd, reply);
    if (!ok) return Contact();
    //printf("unregistration: sent %s, got %s\n", cmd.toString().c_str(), reply.toString().c_str());

    return Contact();
}

Contact RosNameSpace::unregisterContact(const Contact& contact) {
    Bottle cmd,reply;
    cmd.addString("unregisterSubscriber");
    cmd.addString(contact.getName());
    cmd.addString("/yarp/registration");
    Contact c = Contact::bySocket("http",contact.getHost().c_str(),
                                  contact.getPort());
    cmd.addString(c.toString());
    //printf("Writing to %s\n",getNameServerContact().toString().c_str());
    bool ok = NetworkBase::write(getNameServerContact(),
                                 cmd, reply);
    if (!ok) return Contact();
    //printf("unregistration: sent %s, got %s\n", cmd.toString().c_str(), reply.toString().c_str());
    return Contact();
}

bool RosNameSpace::setProperty(const char *name,
                               const char *key,
                               const Value& value) {
    return false;
}

Value *RosNameSpace::getProperty(const char *name, const char *key) {
        return NULL;
}

bool RosNameSpace::connectPortToTopic(const Contact& src,
                                      const Contact& dest,
                                      ContactStyle style) {
    Bottle cmd;
    cmd.addString("registerPublisher");
    cmd.addString(toRosNodeName(src.getName()));
    cmd.addString(dest.getName());
    cmd.addString("*");
    cmd.addString(rosify(src).toString());

    return connectTopic(cmd,false,src,dest,style,false);
}

bool RosNameSpace::connectTopicToPort(const Contact& src,
                                      const Contact& dest,
                                      ContactStyle style) {
    Bottle cmd;
    cmd.addString("registerSubscriber");
    cmd.addString(toRosNodeName(dest.getName()));
    cmd.addString(src.getName());
    cmd.addString("*");
    cmd.addString(rosify(dest).toString());

    return connectTopic(cmd,true,src,dest,style,true);
}

bool RosNameSpace::disconnectPortFromTopic(const Contact& src,
                                           const Contact& dest,
                                           ContactStyle style) {
    Bottle cmd;
    cmd.addString("unregisterPublisher");
    cmd.addString(toRosNodeName(src.getName()));
    cmd.addString(dest.getName());
    cmd.addString(rosify(src).toString());
    return connectTopic(cmd,false,src,dest,style,false);
}

bool RosNameSpace::disconnectTopicFromPort(const Contact& src,
                                           const Contact& dest,
                                           ContactStyle style) {
    Bottle cmd;
    cmd.addString("unregisterSubscriber");
    cmd.addString(toRosNodeName(dest.getName()));
    cmd.addString(src.getName());
    cmd.addString(rosify(dest).toString());
    return connectTopic(cmd,true,src,dest,style,false);
}

bool RosNameSpace::connectPortToPortPersistently(const Contact& src,
                                                 const Contact& dest,
                                                 ContactStyle style) {
    return false;
}

bool RosNameSpace::disconnectPortToPortPersistently(const Contact& src,
                                                    const Contact& dest,
                                                    ContactStyle style) {
    return false;
}

bool RosNameSpace::connectTopic(Bottle& cmd,
                                bool srcIsTopic,
                                const Contact& src,
                                const Contact& dest,
                                ContactStyle style,
                                bool activeRegistration) {
    Bottle reply;
    Contact dynamicSrc = src;
    Contact dynamicDest = dest;
    if (style.carrier!="") {
        if (srcIsTopic) {
            dynamicDest = dynamicDest.addCarrier(style.carrier);
        } else {
            dynamicSrc = dynamicSrc.addCarrier(style.carrier);
        }
    }
    Contact base = getNameServerContact();
    bool ok = NetworkBase::write(base,
                                    cmd,
                                    reply);
    bool fail = (reply.check("faultCode",Value(0)).asInt()!=0)||!ok;
    if (fail) {
        if (!style.quiet) {
            fprintf(stderr,"Failure: name server did not accept connection to topic.\n");
            if (reply.check("faultString")) {
                fprintf(stderr,"Cause: %s\n", reply.check("faultString",Value("")).asString().c_str());
            }
        }
    }
    if (!fail) {
        if (activeRegistration) {
            Bottle *lst = reply.get(2).asList();
            Bottle cmd2;
            if (lst!=NULL) {
                cmd2.addString("publisherUpdate");
                cmd2.addString("/yarp");
                cmd2.addString(dynamicSrc.getName());
                cmd2.addList() = *lst;
                //printf("ACTIVE REGISTRATION: %s\n", cmd2.toString().c_str());
                NetworkBase::write(dynamicDest,
                                    cmd2,
                                    reply,
                                    true);
            }
        }
    }
    return !fail;
}

bool RosNameSpace::localOnly() const {
    return false;
}

bool RosNameSpace::usesCentralServer() const {
    return true;
}

bool RosNameSpace::serverAllocatesPortNumbers() const {
    return false;
}

bool RosNameSpace::connectionHasNameOfEndpoints() const {
    return false;
}

Contact RosNameSpace::detectNameServer(bool useDetectedServer,
                                       bool& scanNeeded,
                                       bool& serverUsed) {
    NameConfig nc;
    nc.fromFile();
    Contact c = nc.getAddress().toContact();
    scanNeeded = false;
    serverUsed = false;

    if (!c.isValid()) {
        scanNeeded = true;
        fprintf(stderr,"Checking for ROS_MASTER_URI...\n");
        ConstString addr = NetworkBase::getEnvironment("ROS_MASTER_URI");
        c = Contact::fromString(addr.c_str());
        if (c.isValid()) {
            c = c.addCarrier("xmlrpc");
            c = c.addName(nc.getNamespace().c_str());
            NameConfig nc;
            nc.setAddress(Address::fromContact(c));
            nc.setMode("ros");
            nc.toFile();
            serverUsed = true;
         }
    }
    return c;
}

bool RosNameSpace::writeToNameServer(PortWriter& cmd,
                                     PortReader& reply,
                                     const ContactStyle& style) {
    DummyConnector con0;
    cmd.write(con0.getWriter());
    Bottle in;
    in.read(con0.getReader());
    ConstString key = in.get(0).asString();
    ConstString arg1 = in.get(1).asString();

    Bottle cmd2;
    if (key=="query") {
        Contact c = queryName(arg1.c_str()).addName("");
        Bottle reply2;
        reply2.addString(arg1.c_str());
        reply2.addString(c.toString().c_str());
        DummyConnector con;
        reply2.write(con.getWriter());
        reply.read(con.getReader());
        return true;
    } else if (key=="list") {
        cmd2.addString("getSystemState");
        cmd2.addString("dummy_id");
    } else {
        return false;
    }
    bool ok = NetworkBase::write(getNameServerContact(),
                                 cmd2,
                                 reply,
                                 style);
    printf("ok? %d\n", ok);
    return ok;

}


ConstString RosNameSpace::toRosName(const ConstString& name) {
    if (name.find(":")==ConstString::npos) return name;
    ConstString result;
    for (int i=0; i<name.length(); i++) {
        if (name[i]!=':') {
            result += name[i];
        } else {
            result += "__";
        }
    }
    return result;
}

ConstString RosNameSpace::fromRosName(const ConstString& name) {
    if (name.find("__")==ConstString::npos) return name;
    // length is at least 2
    ConstString result;
    int ct = 0;
    for (int i=0; i<name.length(); i++) {
        if (name[i]!='_') {
            if (ct) result += '_';
            result += name[i];
            ct = 0;
        } else {
            ct++;
            if (ct==2) {
                result += ':';
                ct = 0;
            }
        }
    }
    if (ct) result += '_';
    return result;
}

ConstString RosNameSpace::toRosNodeName(const ConstString& name) {
    return toRosName(name);
}

ConstString RosNameSpace::fromRosNodeName(const ConstString& name) {
    return fromRosName(name);
}

Contact RosNameSpace::rosify(const Contact& contact) {
    return Contact::bySocket("http",contact.getHost().c_str(),
                             contact.getPort());
}
