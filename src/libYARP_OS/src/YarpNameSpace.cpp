// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/YarpNameSpace.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameConfig.h>

using namespace yarp::os;
using namespace yarp::os::impl;

#define HELPER(x) (*((NameClient*)((x)->system_resource)))

YarpNameSpace::YarpNameSpace(const Contact& contact) {
    system_resource = NameClient::create();
    YARP_ASSERT(system_resource!=NULL);
    HELPER(this).setContact(contact);
    this->contact = contact;
}

YarpNameSpace::~YarpNameSpace() {
    if (system_resource!=NULL) {
        delete &HELPER(this);
        system_resource = NULL;
    }
}

Contact YarpNameSpace::queryName(const ConstString& name) {
    NameClient& nic = HELPER(this);
    Address address = nic.queryName(name);
    return address.toContact();
}


Contact YarpNameSpace::registerName(const ConstString& name) {
    NameClient& nic = HELPER(this);
    Address address = nic.registerName(name);
    return address.toContact();
}

Contact YarpNameSpace::registerContact(const Contact& contact) {
    NameClient& nic = HELPER(this);
    Address address = nic.registerName(contact.getName().c_str(),
                                       Address::fromContact(contact));
    return address.toContact();
}

Contact YarpNameSpace::unregisterName(const ConstString& name) {
    NameClient& nic = HELPER(this);
    Address address = nic.unregisterName(name);
    return address.toContact();
}

Contact YarpNameSpace::unregisterContact(const Contact& contact) {
    NameClient& nic = HELPER(this);
    Address address = nic.unregisterName(contact.getName().c_str());
    return address.toContact();
}


bool YarpNameSpace::setProperty(const ConstString& name, const ConstString& key, 
                                const Value& value) {
    Bottle command;
    command.addString("bot");
    command.addString("set");
    command.addString(name);
    command.addString(key);
    command.add(value);
    Bottle reply;
    NameClient& nic = HELPER(this);
    nic.send(command,reply);
    return reply.size()>0;
}

Value *YarpNameSpace::getProperty(const ConstString& name, const ConstString& key) {
    Bottle command;
    command.addString("bot");
    command.addString("get");
    command.addString(name);
    command.addString(key);
    Bottle reply;
    NameClient& nic = HELPER(this);
    nic.send(command,reply);
    return Value::makeValue(reply.toString());
}

Contact YarpNameSpace::detectNameServer(bool useDetectedServer,
                                        bool& scanNeeded,
                                        bool& serverUsed) {
    NameConfig nc;
    NameClient& nic = HELPER(this);
    nic.setFakeMode(false);
    nic.updateAddress();
    nic.setScan();
    if (useDetectedServer) {
        nic.setSave();
    }
    nic.send("ping",false);
    scanNeeded = nic.didScan();
    serverUsed = nic.didSave();

    Contact c = nic.getAddress().toContact();
    if (scanNeeded) {
        Address addr = nic.getAddress();
        c = c.addSocket("tcp",addr.getName().c_str(),addr.getPort());
    }
    c = c.addName(nc.getNamespace().c_str());
    return c;
}


bool YarpNameSpace::writeToNameServer(PortWriter& cmd,
                                      PortReader& reply,
                                      const ContactStyle& style) {
    Contact srv = getNameServerContact();
    String cmd0 = "NAME_SERVER";

    DummyConnector con0;
    cmd.write(con0.getWriter());
    Bottle in;
    in.read(con0.getReader());
    for (int i=0; i<in.size(); i++) {
        cmd0 += " ";
        cmd0 += in.get(i).toString().c_str();
    }
    NameClient& nic = HELPER(this);
    String result = nic.send(cmd0);
    Bottle reply2;
    reply2.addString(result.c_str());
    DummyConnector con;
    reply2.write(con.getWriter());
    reply.read(con.getReader());
    return result!="";
}


