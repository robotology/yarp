// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/impl/FallbackNameServer.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/all.h>

#include "BootstrapServer.h"
#include "NameServerConnectionHandler.h"

using namespace yarp::os;
using namespace yarp::os::impl;

/**
 *
 * Adapt YARP multicast server to use a different NameService.
 *
 */
class BootstrapServerAdapter : public NameServerStub {
private:
    FallbackNameServer *fallback;
    NameServerConnectionHandler handler;
public:
    BootstrapServerAdapter(NameService& owner) : handler(&owner) {
        fallback = new FallbackNameServer(*this);
        if (fallback==NULL) {
            fprintf(stderr,"Cannot allocate ServerAdapter\n");
            exit(1);
        }
    }

    virtual ~BootstrapServerAdapter() {
        if (fallback!=NULL) delete fallback;
        fallback = NULL;
    }

    virtual String apply(const String& txt, const Address& remote) {
        DummyConnector con, con2;
        con.setTextMode(true);
        ConnectionWriter& writer = con.getWriter();
        writer.appendString(txt.c_str());
        bool ok = handler.apply(con.getReader(),&(con2.getWriter()));
        String result = "";
        if (ok) {
            result = con2.getReader().expectText().c_str();
        }
        printf("ASKED %s, GAVE %s\n", txt.c_str(), result.c_str());
        return result;
    }
    
    bool start() {
        return fallback->start();
    }

    bool stop() {
        fallback->close();
        return true;
    }
};

BootstrapServer::BootstrapServer(NameService& owner) {
    implementation = new BootstrapServerAdapter(owner);
    if (implementation==NULL) {
        fprintf(stderr,"Cannot allocate ServerAdapter\n");
        exit(1);
    }
}

BootstrapServer::~BootstrapServer() {
    if (implementation!=NULL) {
        delete ((BootstrapServerAdapter*)implementation);
        implementation = NULL;
    }
}

bool BootstrapServer::start() {
    if (implementation!=NULL) {
        return ((BootstrapServerAdapter*)implementation)->start();
    }
    return false;
}

bool BootstrapServer::stop() {
    if (implementation!=NULL) {
        return ((BootstrapServerAdapter*)implementation)->stop();
    }
    return false;
}


bool BootstrapServer::configFileBootstrap(yarp::os::Contact& contact,
                                          bool autofix) {
    Address suggest = Address::fromContact(contact);
    
    // see what address is lying around
    Address prev;
    NameConfig conf;
    if (conf.fromFile()) {
        prev = conf.getAddress();
    } else if (!autofix) {
        fprintf(stderr,"Could not read configuration file %s\n",
                conf.getConfigFileName().c_str());
        return false;
    }
    
    // merge
    if (prev.isValid()) {
        if (suggest.getName()=="...") {
            suggest = Address(prev.getName(),suggest.getPort(),
                              suggest.getCarrierName(),
                              suggest.getRegName());
        }
        if (suggest.getCarrierName()=="...") {
            suggest = Address(suggest.getName(),suggest.getPort(),
                              prev.getCarrierName(),
                              suggest.getRegName());
        }
        if (suggest.getPort()==0) {
            suggest = Address(suggest.getName(),prev.getPort(),
                              suggest.getCarrierName(),
                              suggest.getRegName());
        }
    }

    if (suggest.getRegName()=="...") {
        suggest = Address(suggest.getName(),suggest.getPort(),
                          suggest.getCarrierName(),
                          conf.getNamespace());
    }
    
    // still something not set?
    if (suggest.getPort()==0) {
        suggest = Address(suggest.getName(),10000,
                          suggest.getCarrierName(),
                          suggest.getRegName());
    }
    if (suggest.getName()=="...") {
        // should get my IP
        suggest = Address(conf.getHostName(),suggest.getPort(),
                          suggest.getCarrierName(),
                          suggest.getRegName());
    }
    
    // finally, should make sure IP is local, and if not, correct it
    if (!conf.isLocalName(suggest.getName())) {
        fprintf(stderr,"Overriding non-local address for name server\n");
        suggest = Address(conf.getHostName(),suggest.getPort(),
                          suggest.getCarrierName(),
                          suggest.getRegName());

    }
    
    // and save
    conf.setAddress(suggest);
    if (!conf.toFile()) {
        fprintf(stderr,"Could not save configuration file %s\n",
                conf.getConfigFileName().c_str());
    }

    contact = suggest.toContact();
    return true;
}


Contact BootstrapServer::where() {
    Address addr = FallbackNameServer::getAddress();
    addr.addRegName("fallback");
    return addr.toContact();
}


