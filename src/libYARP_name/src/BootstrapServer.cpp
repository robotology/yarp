// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>

#ifdef YARP_HAS_ACE

#include <yarp/os/impl/FallbackNameServer.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/all.h>

#include <yarp/name/NameServerConnectionHandler.h>

#include <yarp/name/BootstrapServer.h>

using namespace yarp::os;
using namespace yarp::name;
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
            ::exit(1);
        }
    }

    virtual ~BootstrapServerAdapter() {
        if (fallback!=NULL) delete fallback;
        fallback = NULL;
    }

    virtual String apply(const String& txt, const Contact& remote) {
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
        ::exit(1);
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
    Contact suggest = contact;
    
    // see what address is lying around
    Contact prev;
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
        if (suggest.getHost()=="...") {
            suggest = Contact::bySocket(suggest.getCarrier(),
                                        prev.getHost(),suggest.getPort())
                .addName(suggest.getRegName());
        }
        if (suggest.getCarrier()=="...") {
            suggest = Contact::bySocket(prev.getCarrier(),
                                        suggest.getHost(),suggest.getPort())
                .addName(suggest.getRegName());
        }
        if (suggest.getPort()==0) {
            suggest = Contact::bySocket(suggest.getCarrier(),
                                        suggest.getHost(),prev.getPort())
                .addName(suggest.getRegName());
        }
    }

    if (suggest.getRegName()=="...") {
        suggest = Contact::bySocket(suggest.getCarrier(),
                                    suggest.getHost(),suggest.getPort())
            .addName(conf.getNamespace());
    }
    
    // still something not set?
    if (suggest.getPort()==0) {
        suggest = Contact::bySocket(suggest.getCarrier(),
                                    suggest.getHost(),10000)
            .addName(suggest.getRegName());
    }
    if (suggest.getHost()=="...") {
        // should get my IP
        suggest = Contact::bySocket(suggest.getCarrier(),
                                    conf.getHostName(),suggest.getPort())
            .addName(suggest.getRegName());
    }
    
    // finally, should make sure IP is local, and if not, correct it
    if (!conf.isLocalName(suggest.getHost())) {
        fprintf(stderr,"Overriding non-local address for name server\n");
        suggest = Contact::bySocket(suggest.getCarrier(),
                                    conf.getHostName(),suggest.getPort())
            .addName(suggest.getRegName());

    }
    
    // and save
    conf.setAddress(suggest);
    if (!conf.toFile()) {
        fprintf(stderr,"Could not save configuration file %s\n",
                conf.getConfigFileName().c_str());
    }

    contact = suggest;
    return true;
}


Contact BootstrapServer::where() {
    Contact addr = FallbackNameServer::getAddress();
    addr.addName("fallback");
    return addr;
}

#endif

