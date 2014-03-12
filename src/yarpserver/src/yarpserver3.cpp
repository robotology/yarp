// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/conf/system.h>
#include <yarp/os/all.h>
#include <yarp/os/Os.h>
#include <yarp/os/RosNameSpace.h>

#include <yarp/name/NameServerManager.h>
#include <yarp/name/BootstrapServer.h>

#include <yarp/yarpserversql/yarpserversql.h>

#include "TripleSourceCreator.h"
#include "NameServiceOnTriples.h"
#include "AllocatorOnTriples.h"
#include "SubscriberOnSql.h"
#include "StyleNameService.h"
#include "ComposedNameService.h"
#include "ParseName.h"

using namespace yarp::os;
using namespace yarp::name;
using namespace std;

class NameServerContainer : public ComposedNameService {
private:
    Contact contact;
    TripleSourceCreator db;
    SubscriberOnSql subscriber;
    AllocatorConfig config;
    AllocatorOnTriples alloc;
    NameServiceOnTriples ns;
    StyleNameService style;
    ComposedNameService combo1;
    bool silent;
    NameSpace *space;
public:
    using ComposedNameService::open;

    NameServerContainer() {
        silent = false;
        space = NULL;
    }

    virtual ~NameServerContainer() {
        subscriber.clear();
        if (space) delete space;
        space = NULL;
    }

    void setSilent(bool silent) {
        ns.setSilent(silent);
        subscriber.setSilent(silent);
        this->silent = silent;
    }

    const Contact& where() {
        return contact;
    }

    Contact whereDelegate() {
        if (!space) return Contact();
        return space->getNameServerContact();
    }

    void preregister(const Contact& c) {
        Network::registerContact(c);
        subscriber.welcome(c.getName().c_str(),1);
    }

    bool open(Searchable& options) {
        ConstString dbDefault = ":memory:";
        ConstString subdbDefault = ":memory:";
        
        if (options.check("memory")) {
            fprintf(stderr,"The --memory option was given, but that is now a default. Continuing.\n");
        }
        
        ConstString dbFilename = options.check("portdb",
                                               Value(dbDefault)).asString();
        ConstString subdbFilename = options.check("subdb",
                                                  Value(subdbDefault)).asString();

        ConstString ip = options.check("ip",Value("...")).asString();
        int sock = options.check("socket",Value(Network::getDefaultPortRange())).asInt();
        bool cautious = options.check("cautious");
        bool verbose = options.check("verbose");
        
        if (!silent) {
            printf("Using port database: %s\n", 
                   dbFilename.c_str());
            printf("Using subscription database: %s\n", 
                   subdbFilename.c_str());
            if (dbFilename!=":memory:" || subdbFilename!=":memory:") {
                printf("If you ever need to clear the name server's state, just delete those files.\n\n");
            }
            printf("IP address: %s\n", 
                   (ip=="...")?"default":ip.c_str());
            printf("Port number: %d\n", sock);
        }

        bool reset = false;
        if (options.check("ip")||options.check("socket")) {
            fprintf(stderr,"Database needs to be reset, IP or port number set.\n");
            reset = true;
        }

        TripleSource *pmem = db.open(dbFilename.c_str(),cautious,reset);
        if (pmem == NULL) {
            fprintf(stderr,"Aborting, ports database failed to open.\n");
            return false;
        }
        if (verbose) {
            pmem->setVerbose(1);
        }

        if (!subscriber.open(subdbFilename.c_str())) {
            fprintf(stderr,"Aborting, subscription database failed to open.\n");
            return false;
        }
        if (verbose) {
            subscriber.setVerbose(true);
        }

        contact = 
            Contact::byName("...").addSocket("tcp",ip.c_str(),sock);
        
        if (!options.check("local")) {
            BootstrapServer::configFileBootstrap(contact);
        }

        if (options.check("ros") || NetworkBase::getEnvironment("YARP_USE_ROS")!="") {
            ConstString addr = NetworkBase::getEnvironment("ROS_MASTER_URI");
            Contact c = Contact::fromString(addr.c_str());
            if (c.isValid()) {
                c = c.addCarrier("xmlrpc").addName("/ros");
                space = new RosNameSpace(c);
                subscriber.setDelegate(space);
                ns.setDelegate(space);
                fprintf(stderr, "Using ROS with ROS_MASTER_URI=%s\n", addr.c_str());
            } else {
                fprintf(stderr, "Cannot find ROS, check ROS_MASTER_URI (currently '%s')\n", addr.c_str());
                ::exit(1);
            }
        }

        config.minPortNumber = contact.getPort()+2;
        config.maxPortNumber = contact.getPort()+9999;
        alloc.open(pmem,config);
        ns.open(pmem,&alloc,contact);
        NetworkBase::queryBypass(&ns);
        subscriber.setStore(ns);
        ns.setSubscriber(&subscriber);
        style.configure(options);
        combo1.open(subscriber,style);
        open(combo1,ns);
        return true;
    }
};

yarpserversql_API yarp::os::NameStore *yarpserver3_create(yarp::os::Searchable& options) {
    NameServerContainer *nc = new NameServerContainer;
    if (!nc) return NULL;
    nc->setSilent(true);
    if (!nc->open(options)) {
        delete nc;
        return NULL;
    }
    nc->goPublic();
    return nc;
}

yarpserversql_API int yarpserver3_main(int argc, char *argv[]) {
    // check if YARP version is sufficiently up to date - there was
    // an important bug fix
    Bottle b("ip 10.0.0.10");
    if (b.get(1).asString()!="10.0.0.10") {
        fprintf(stderr, "Sorry, please update YARP version");
        ::exit(1);
    }

    printf("__   __ _    ____  ____  \n\
\\ \\ / // \\  |  _ \\|  _ \\ \n\
 \\ V // _ \\ | |_) | |_) |\n\
  | |/ ___ \\|  _ <|  __/ \n\
  |_/_/   \\_\\_| \\_\\_|    \n\n");

    Property options;
    options.fromCommand(argc,argv);

    if (options.check("help")) {
        printf("Welcome to the YARP name server.\n");
        printf("  --config filename.conf   Load options from a file.\n");
        printf("  --portdb ports.db        Store port infomation in named database.\n");
        printf("                           Must not be on an NFS file system.\n");
        printf("                           Set to :memory: to store in memory (faster).\n");
        printf("  --subdb subs.db          Store subscription infomation in named database.\n");
        printf("                           Must not be on an NFS file system.\n");
        printf("                           Set to :memory: to store in memory (faster).\n");
        printf("  --ip IP.AD.DR.ESS        Set IP address of server.\n");
        printf("  --socket NNNNN           Set port number of server.\n");
        printf("  --web dir                Serve web resources from given directory.\n");
        printf("  --no-web-cache           Reload pages from file for each request.\n");
        printf("  --ros                    Delegate pub/sub to ROS name server.\n");
        return 0;
    } else {
        printf("Call with --help for information on available options\n");
    }

    ConstString configFilename = options.check("config",
                                               Value("yarpserver.conf")).asString();
    if (!options.check("config")) {
        configFilename = Network::getConfigFile(configFilename.c_str());
    }
    if (yarp::os::stat(configFilename.c_str())==0) {
        printf("Reading options from %s\n", configFilename.c_str());
        options.fromConfigFile(configFilename.c_str(),false);
    } else {
        printf("Options can be set on command line or in %s\n", configFilename.c_str());
    }

    Network yarp;

    NameServerContainer nc;
    nc.setSilent(false);
    if (!nc.open(options)) {
        return 1;
    }
 
    NameServerManager name(nc);
#ifdef YARP_HAS_ACE
    BootstrapServer fallback(name);
#endif

    Port server;
    name.setPort(server);
    server.setReaderCreator(name);
    bool ok = server.open(nc.where(),false);
    if (!ok) {
        fprintf(stderr, "Name server failed to open\n");
        return 1;
    }
    printf("\n");

#ifdef YARP_HAS_ACE
    fallback.start();
#endif

    // Repeat registrations for the server and fallback server -
    // these registrations are more complete.
    printf("Registering name server with itself:\n");
    nc.preregister(nc.where());
#ifdef YARP_HAS_ACE
    nc.preregister(fallback.where());
#endif
    Contact alt = nc.whereDelegate();
    if (alt.isValid()) {
        nc.preregister(alt);
    }
    nc.goPublic();
    printf("Name server can be browsed at http://%s:%d/\n",
           nc.where().getHost().c_str(), nc.where().getPort());
    printf("\nOk.  Ready!\n");
    
    while (true) {
        Time::delay(600);
        printf("Name server running happily\n");
    }
    server.close();

    return 0;
}
