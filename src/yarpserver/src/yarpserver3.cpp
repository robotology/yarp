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

#include <yarp/name/NameServerManager.h>
#ifdef YARP_HAS_ACE
#include <yarp/name/BootstrapServer.h>
#endif

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
        printf("  --memory                 Shortcut to use :memory: for portdb and subdb\n");
        printf("  --ip IP.AD.DR.ESS        Set IP address of server.\n");
        printf("  --socket NNNNN           Set port number of server.\n");
        printf("  --web dir                Serve web resources from given directory.\n");
        printf("  --no-web-cache           Reload pages from file for each request.\n");
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

    ConstString dbDefault = "ports.db";
    ConstString subdbDefault = "subs.db";

    if (options.check("memory")) {
        dbDefault = ":memory:";
        subdbDefault = ":memory:";
    }
    
    ConstString dbFilename = options.check("portdb",
                                           Value(dbDefault)).asString();
    ConstString subdbFilename = options.check("subdb",
                                              Value(subdbDefault)).asString();
    ConstString ip = options.check("ip",Value("...")).asString();
    int sock = options.check("socket",Value(Network::getDefaultPortRange())).asInt();
    bool cautious = options.check("cautious");
    bool verbose = options.check("verbose");

    printf("Using port database: %s\n", 
           dbFilename.c_str());
    printf("Using subscription database: %s\n", 
           subdbFilename.c_str());
    printf("If you ever need to clear the name server's state, just delete those files.\n\n");
    printf("IP address: %s\n", 
           (ip=="...")?"default":ip.c_str());
    printf("Port number: %d\n", sock);
    
    Network yarp;

    bool reset = false;
    if (options.check("ip")||options.check("socket")) {
        fprintf(stderr,"Database needs to be reset, IP or port number set.\n");
        reset = true;
    }

    TripleSourceCreator db;
    TripleSource *pmem = db.open(dbFilename.c_str(),cautious,reset);
    if (pmem == NULL) {
        fprintf(stderr,"Aborting, ports database failed to open.\n");
        return 1;
    }
    if (verbose) {
        pmem->setVerbose(1);
    }

    SubscriberOnSql subscriber;
    if (!subscriber.open(subdbFilename.c_str())) {
        fprintf(stderr,"Aborting, subscription database failed to open.\n");
        return 1;
    }
    if (verbose) {
        subscriber.setVerbose(true);
    }

    Contact contact = 
        Contact::byName("...").addSocket("tcp",ip.c_str(),sock);
    
#ifdef YARP_HAS_ACE
    BootstrapServer::configFileBootstrap(contact);
#endif

    AllocatorConfig config;
    config.minPortNumber = contact.getPort()+2;
    config.maxPortNumber = contact.getPort()+9999;
    AllocatorOnTriples alloc(pmem,config);
    NameServiceOnTriples ns(pmem,&alloc,contact);
    yarp.queryBypass(&ns);
    subscriber.setStore(ns);
    ns.setSubscriber(&subscriber);
    StyleNameService style;
    style.configure(options);
    ComposedNameService combo1(subscriber,style);
    ComposedNameService combo(combo1,ns);
    NameServerManager name(combo);
#ifdef YARP_HAS_ACE
    BootstrapServer fallback(name);
#endif

    Port server;
    name.setPort(server);
    server.setReaderCreator(name);
    bool ok = server.open(contact,false);
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
    Network::registerContact(contact);
    subscriber.welcome(contact.getName().c_str(),1);
#ifdef YARP_HAS_ACE
    Network::registerContact(fallback.where());
    subscriber.welcome(fallback.where().getName().c_str(),1);
#endif
    ns.goPublic();
    printf("Name server can be browsed at http://%s:%d/\n",
           contact.getHost().c_str(), contact.getPort());
    printf("\nOk.  Ready!\n");
    
    while (true) {
        Time::delay(600);
        printf("Name server running happily\n");
    }
    server.close();

    db.close();

    return 0;
}
