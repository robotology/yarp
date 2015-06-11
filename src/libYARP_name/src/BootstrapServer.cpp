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
#endif

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
#ifdef YARP_HAS_ACE
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
#endif

BootstrapServer::BootstrapServer(NameService& owner) {
#ifdef YARP_HAS_ACE
    implementation = new BootstrapServerAdapter(owner);
    if (implementation==NULL) {
        fprintf(stderr,"Cannot allocate ServerAdapter\n");
        ::exit(1);
    }
#else
    fprintf(stderr,"No BootstrapServer available without ACE multicast\n");
    ::exit(1);
#endif
}

BootstrapServer::~BootstrapServer() {
#ifdef YARP_HAS_ACE
    if (implementation!=NULL) {
        delete ((BootstrapServerAdapter*)implementation);
        implementation = NULL;
    }
#endif
}

bool BootstrapServer::start() {
#ifdef YARP_HAS_ACE
    if (implementation!=NULL) {
        return ((BootstrapServerAdapter*)implementation)->start();
    }
#endif
    return false;
}

bool BootstrapServer::stop() {
#ifdef YARP_HAS_ACE
    if (implementation!=NULL) {
        return ((BootstrapServerAdapter*)implementation)->stop();
    }
#endif
    return false;
}


bool BootstrapServer::configFileBootstrap(yarp::os::Contact& contact,
                                          bool configFileRequired,
                                          bool mayEditConfigFile) {
    Contact suggest = contact;
    
    // see what address is lying around
    Contact prev;
    NameConfig conf;
    if (conf.fromFile()) {
        prev = conf.getAddress();
    } else if (configFileRequired) {
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
    
    if (!configFileRequired)  {
        // finally, should make sure IP is local, and if not, correct it
        if (!conf.isLocalName(suggest.getHost())) {
            fprintf(stderr,"Overriding non-local address for name server\n");
            suggest = Contact::bySocket(suggest.getCarrier(),
                                        conf.getHostName(),suggest.getPort())
                .addName(suggest.getRegName());
        } else {
            // Let's just check we're not a loopback
            ConstString betterHost = conf.getHostName(false,suggest.getHost());
            if (betterHost!=suggest.getHost()) {
                fprintf(stderr,"Overriding loopback address for name server\n");
                suggest = Contact::bySocket(suggest.getCarrier(),
                                            betterHost,suggest.getPort())
                    .addName(suggest.getRegName());
            }
        }
    }

    bool changed = false;
    if (prev.isValid()) {
        changed = (prev.getHost() != suggest.getHost()) ||
            (prev.getPort() != suggest.getPort()) ||
            (conf.getMode() != "yarp" && conf.getMode() != "");
    }
    if (changed && !mayEditConfigFile) {
        fprintf(stderr,"PROBLEM: need to change settings in %s\n",
                conf.getConfigFileName().c_str());
        fprintf(stderr,"  Current settings: host %s port %d family %s\n",
                prev.getHost().c_str(), prev.getPort(), 
                (conf.getMode()=="")?"yarp":conf.getMode().c_str());
        fprintf(stderr,"  Desired settings:  host %s port %d family %s\n",
                suggest.getHost().c_str(), suggest.getPort(), "yarp");
        fprintf(stderr,"Please specify '--write' if it is ok to overwrite current settings, or\n");
        fprintf(stderr,"Please specify '--read' to use the current settings, or\n");
        fprintf(stderr,"delete %s\n", conf.getConfigFileName().c_str());
        return false;
    }
    bool shouldSave = changed || !prev.isValid();

    if (shouldSave) {
        // and save
        conf.setAddress(suggest);
        if (!conf.toFile()) {
            fprintf(stderr,"Could not save configuration file %s\n",
                    conf.getConfigFileName().c_str());
        }
    }

    contact = suggest;
    return true;
}


Contact BootstrapServer::where() {
#ifdef YARP_HAS_ACE
    Contact addr = FallbackNameServer::getAddress();
    addr.addName("fallback");
    return addr;
#else
    return Contact();
#endif
}

//#endif

