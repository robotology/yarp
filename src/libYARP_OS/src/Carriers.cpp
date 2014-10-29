// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/impl/FakeFace.h>
#include <yarp/os/impl/TcpCarrier.h>
#include <yarp/os/impl/TextCarrier.h>

#ifdef YARP_HAS_ACE
#  include <yarp/os/impl/UdpCarrier.h>
#  include <yarp/os/impl/McastCarrier.h>
#  include <yarp/os/impl/ShmemCarrier.h>
#endif

#include <yarp/os/impl/LocalCarrier.h>
#include <yarp/os/impl/NameserCarrier.h>
#include <yarp/os/impl/HttpCarrier.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/YarpPlugin.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Carriers *Carriers::yarp_carriers_instance = NULL;

static bool matchCarrier(const Bytes *header, Bottle& code) {
    int at = 0;
    bool success = true;
    bool done = false;
    for (int i=0; i<code.size() && !done; i++) {
        Value& v = code.get(i);
        if (v.isString()) {
            ConstString str = v.asString();
            for (int j=0; j<(int)str.length(); j++) {
                if ((int)header->length()<=at) {
                    success = false;
                    done = true;
                    break;
                }
                if (str[j] != header->get()[at]) {
                    success = false;
                    done = true;
                    break;
                }
                at++;
            }
        } else {
            at++;
        }
    }
    return success;
}

static bool checkForCarrier(const Bytes *header, Searchable& group) {
    Bottle code = group.findGroup("code").tail();
    if (code.size()==0) return false;
    if (matchCarrier(header,code)) {
        ConstString name = group.find("name").asString();
        if (NetworkBase::registerCarrier(name.c_str(),NULL)) {
            return true;
        }
    }
    return false;
}

static bool scanForCarrier(const Bytes *header) {
    YARP_SPRINTF0(Logger::get(),
                  debug,
                  "Scanning for a carrier by header.");
    YarpPluginSelector selector;
    selector.scan();
    Bottle lst = selector.getSelectedPlugins();
    for (int i=0; i<lst.size(); i++) {
        if (checkForCarrier(header,lst.get(i))) {
            return true;
        }
    }
    return false;
}

Carriers::Carriers() {
    delegates.push_back(new HttpCarrier());
    delegates.push_back(new NameserCarrier());
    delegates.push_back(new LocalCarrier());
#ifdef YARP_HAS_ACE
    //delegates.push_back(new ShmemCarrier(1));
    delegates.push_back(new ShmemCarrier(2)); // new Alessandro version
#endif
    delegates.push_back(new TcpCarrier());
    delegates.push_back(new TcpCarrier(false));
#ifdef YARP_HAS_ACE
    delegates.push_back(new McastCarrier());
    delegates.push_back(new UdpCarrier());
#endif
    delegates.push_back(new TextCarrier());
    delegates.push_back(new TextCarrier(true));
}

Carriers::~Carriers() {
    clear();
}

void Carriers::clear() {
    PlatformVector<Carrier *>& lst = delegates;
    for (unsigned int i=0; i<lst.size(); i++) {
        delete lst[i];
    }
    lst.clear();
}

Carrier *Carriers::chooseCarrier(const String *name, const Bytes *header,
                                 bool load_if_needed,
                                 bool return_template) {
    String s;
    if (name!=NULL) {
        s = *name;
        size_t i = s.find("+");
        if (i!=String::npos) {
            s[i] = '\0';
            s = s.c_str();
            name = &s;
        }
    }
    for (size_t i=0; i<(size_t)delegates.size(); i++) {
        Carrier& c = *delegates[i];
        bool match = false;
        if (name!=NULL) {
            if ((*name) == c.getName()) {
                match = true;
            }
        }
        if (header!=NULL) {
            if (c.checkHeader(*header)) {
                match = true;
            }
        }
        if (match) {
            if (!return_template) return c.create();
            return &c;
        }
    }
    if (load_if_needed) {
        if (name!=NULL) {
            // ok, we didn't find a carrier, but we have a name.
            // let's try to register it, and see if a dll is found.
            if (NetworkBase::registerCarrier(name->c_str(),NULL)) {
                // We made progress, let's try again...
                return Carriers::chooseCarrier(name,header,false);
            }
        } else {
            if (scanForCarrier(header)) {
                // We made progress, let's try again...
                return Carriers::chooseCarrier(name,header,true);
            }
        }
    }
    if (name==NULL) {
        String txt;
        for (int i=0; i<(int)header->length(); i++) {
            txt += NetType::toString(header->get()[i]);
            txt += " ";
        }
        txt += "[";
        for (int i=0; i<(int)header->length(); i++) {
            char ch = header->get()[i];
            if (ch>=32) {
                txt += ch;
            } else {
                txt += '.';
            }
        }
        txt += "]";

        YARP_SPRINTF1(Logger::get(),
                      error,
                      "Could not find carrier for a connection starting with: %s",
                      txt.c_str());
    } else {
        YARP_SPRINTF1(Logger::get(),
                      error,
                      "Could not find carrier \"%s\"",
                      (name!=NULL)?name->c_str():"[bytes]");;
    }
    return NULL;
}


Carrier *Carriers::chooseCarrier(const String& name) {
    return getInstance().chooseCarrier(&name,NULL);
}

Carrier *Carriers::getCarrierTemplate(const String& name) {
    return getInstance().chooseCarrier(&name,NULL,true,true);
}


Carrier *Carriers::chooseCarrier(const Bytes& bytes) {
    return getInstance().chooseCarrier(NULL,&bytes);
}


Face *Carriers::listen(const Contact& address) {
    // for now, only TcpFace exists - otherwise would need to manage
    // multiple possibilities
    Face *face = NULL;
    if (address.getCarrier() == "fake") {
        face = new FakeFace();
    }
    if (face == NULL) {
        face = new TcpFace();
    }
    bool ok = face->open(address);
    if (!ok) {
        delete face;
        face = NULL;
    }
    return face;
}


OutputProtocol *Carriers::connect(const Contact& address) {
    TcpFace tcpFace;
    return tcpFace.write(address);
}


bool Carriers::addCarrierPrototype(Carrier *carrier) {
    getInstance().delegates.push_back(carrier);
    return true;
}


bool Carrier::reply(ConnectionState& proto, SizedWriter& writer) {
    writer.write(proto.os());
    return proto.os().isOk();
}

Carriers& Carriers::getInstance() {
    if (yarp_carriers_instance == NULL) {
        yarp_carriers_instance = new Carriers();
        yAssert(yarp_carriers_instance!=NULL);
    }
    return *yarp_carriers_instance;
}


void Carriers::removeInstance() {
    if (yarp_carriers_instance != NULL) {
        delete yarp_carriers_instance;
        yarp_carriers_instance = NULL;
    }
}


Bottle Carriers::listCarriers() {
    Bottle lst;
    PlatformVector<Carrier *>& delegates = getInstance().delegates;
    for (size_t i=0; i<(size_t)delegates.size(); i++) {
        Carrier& c = *delegates[i];
        lst.addString(c.getName());
    }
    return lst;
}
