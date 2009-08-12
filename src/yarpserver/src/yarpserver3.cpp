// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/all.h>

#include "TripleSourceCreator.h"
#include "NameServiceOnTriples.h"
#include "AllocatorOnTriples.h"
#include "NameServerManager.h"
#include "BootstrapServer.h"

using namespace yarp::os;
using namespace std;

int main(int argc, char *argv[]) {
    // check if YARP version is sufficiently up to date - there was
    // an important bug fix
    Bottle b("ip 10.0.0.10");
    if (b.get(1).asString()!="10.0.0.10") {
        fprintf(stderr, "Sorry, please update YARP version");
        exit(1);
    }

    printf("__   __ _    ____  ____  \n\
\\ \\ / // \\  |  _ \\|  _ \\ \n\
 \\ V // _ \\ | |_) | |_) |\n\
  | |/ ___ \\|  _ <|  __/ \n\
  |_/_/   \\_\\_| \\_\\_|    \n\n");
 

    Network yarp;

    int sock = 0;
    const char *ip = "...";
    bool reset = false;
    if (argc>=2) {
        sock = atoi(argv[1]);
        if (argc>=3) {
            ip = (const char *)argv[2];
        }
        fprintf(stderr,"Arguments supplied; database needs to be reset.\n");
        reset = true;
    }

    TripleSourceCreator db;
    TripleSource *pmem = db.open("yarp.db",reset);
    if (pmem == NULL) {
        fprintf(stderr,"Aborting, database failed to open.\n");
        return 1;
    }

    Contact contact = 
        Contact::byName("...").addSocket("tcp",ip,sock);
    
    BootstrapServer::configFileBootstrap(contact);

    AllocatorConfig config;
    config.minPortNumber = contact.getPort()+2;
    config.maxPortNumber = contact.getPort()+9999;
    AllocatorOnTriples alloc(pmem,config);
    NameServiceOnTriples ns(pmem,&alloc);
    NameServerManager name(ns);
    BootstrapServer fallback(name);

    Port server;
    name.setPort(server);
    server.setReaderCreator(name);
    bool ok = server.open(contact,false);
    if (!ok) {
        fprintf(stderr, "Name server failed to open\n");
        return 1;
    }
    printf("\n");

    fallback.start();

    // Repeat registrations for the server and fallback server -
    // these registrations are more complete.
    Network::registerContact(contact);
    Network::registerContact(fallback.where());

    while (true) {
        Time::delay(60);
        printf("Name server running happily\n");
    }
    server.close();

    db.close();

    return 0;
}
