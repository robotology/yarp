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
    yAssert(system_resource!=NULL);
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
    return nic.queryName(name);
}


Contact YarpNameSpace::registerName(const ConstString& name) {
    return registerContact(Contact::byName(name));
}

Contact YarpNameSpace::registerContact(const Contact& contact) {
    NameClient& nic = HELPER(this);
    Contact address = nic.registerName(contact.getName().c_str(),
                                       contact);
    if (address.isValid()) {
        NestedContact nc;
        nc.fromString(address.getRegName().c_str());
        ConstString cat = nc.getCategory();
        if (nc.getNestedName()!="") {
            //bool service = (cat.find("1") != ConstString::npos);
            bool publish = (cat.find("+") != ConstString::npos);
            bool subscribe = (cat.find("-") != ConstString::npos);
            ContactStyle style;
            Contact c1 = Contact::byName(nc.getFullName());
            Contact c2 = Contact::byName(ConstString("topic:/") + nc.getNestedName());
            if (subscribe) {
                style.persistenceType = ContactStyle::END_WITH_TO_PORT;
                connectPortToTopic(c2,c1,style);
            }
            if (publish) {
                style.persistenceType = ContactStyle::END_WITH_FROM_PORT;
                connectPortToTopic(c1,c2,style);
            }
        }
    }
    return address;
}

Contact YarpNameSpace::unregisterName(const ConstString& name) {
    NestedContact nc;
    nc.fromString(name);
    ConstString cat = nc.getCategory();
    if (nc.getNestedName()!="") {
        //bool service = (cat.find("1") != ConstString::npos);
        bool publish = (cat.find("+") != ConstString::npos);
        bool subscribe = (cat.find("-") != ConstString::npos);
        ContactStyle style;
        Contact c1 = Contact::byName(nc.getFullName());
        Contact c2 = Contact::byName(ConstString("topic:/") + nc.getNestedName());
        if (subscribe) {
            disconnectPortFromTopic(c2,c1,style);
        }
        if (publish) {
            disconnectPortFromTopic(c1,c2,style);
        }
    }
    NameClient& nic = HELPER(this);
    return nic.unregisterName(name);
}

Contact YarpNameSpace::unregisterContact(const Contact& contact) {
    NameClient& nic = HELPER(this);
    return nic.unregisterName(contact.getName());
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

    Contact c = nic.getAddress();
    c = c.addName(nc.getNamespace().c_str());
    //Contact c = nic.getAddress().toContact();
    //    if (scanNeeded) {
    //        Address addr = nic.getAddress();
    //        c = c.addSocket("tcp",addr.getName().c_str(),addr.getPort());
    ////}
    //c = c.addName(nc.getNamespace().c_str());
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


