/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/serversql/impl/NameServerContainer.h>

#include <yarp/os/Carriers.h>
#include <yarp/os/Network.h>
#include <yarp/os/RosNameSpace.h>
#include <yarp/os/Value.h>
#include <yarp/name/BootstrapServer.h>

#include <string>

// For a yarp server running on the port 10000
// * the first port assigned will be 10002
// * the last port assigned will be 19999
#define MIN_PORT_NUMBER_OFFSET 2
#define MAX_PORT_NUMBER_OFFSET 9999


using yarp::os::Contact;
using yarp::os::NameSpace;
using yarp::os::Searchable;
using yarp::os::NetworkBase;
using yarp::os::Network;
using yarp::os::Value;
using yarp::os::Carriers;
using yarp::os::RosNameSpace;
using yarp::name::BootstrapServer;
using yarp::serversql::impl::ComposedNameService;
using yarp::serversql::impl::NameServiceOnTriples;
using yarp::serversql::impl::StyleNameService;



using yarp::serversql::impl::NameServerContainer;

NameServerContainer::~NameServerContainer()
{
    subscriber.clear();
    if (space) {
        delete space;
        space = nullptr;
    }
}

void NameServerContainer::setSilent(bool silent)
{
    ns.setSilent(silent);
    subscriber.setSilent(silent);
    this->silent = silent;
}

const Contact& NameServerContainer::where()
{
    return contact;
}

Contact NameServerContainer::whereDelegate()
{
    if (!space) {
        return Contact();
    }
    return space->getNameServerContact();
}

void NameServerContainer::preregister(const Contact& c)
{
    Network::registerContact(c);
    subscriber.welcome(c.getName(),1);
}

bool NameServerContainer::open(Searchable& options)
{
    std::string dbDefault = ":memory:";
    std::string subdbDefault = ":memory:";

    if (options.check("memory")) {
        fprintf(stderr,"The --memory option was given, but that is now a default. Continuing.\n");
    }

    std::string dbFilename = options.check("portdb",
                                           Value(dbDefault)).asString();
    std::string subdbFilename = options.check("subdb",
                                              Value(subdbDefault)).asString();

    std::string ip = options.check("ip",Value("...")).asString();
    int sock = options.check("socket",Value(Network::getDefaultPortRange())).asInt32();
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
    if (pmem == nullptr) {
        fprintf(stderr,"Aborting, ports database failed to open.\n");
        return false;
    }
    if (verbose) {
        pmem->setVerbose(1);
    }

    if (!subscriber.open(subdbFilename)) {
        fprintf(stderr,"Aborting, subscription database failed to open.\n");
        return false;
    }
    if (verbose) {
        subscriber.setVerbose(true);
    }

    contact = Contact("...", "tcp", ip, sock);

    if (!options.check("local")) {
        if (!BootstrapServer::configFileBootstrap(contact,
                                                  options.check("read"),
                                                  options.check("write"))) {
            fprintf(stderr,"Aborting.\n");
            return false;
        }
    }

    if (options.check("ros") || NetworkBase::getEnvironment("YARP_USE_ROS")!="") {
        yarp::os::Bottle lst = yarp::os::Carriers::listCarriers();
        std::string lstStr(lst.toString());
        if (lstStr.find("rossrv") == std::string::npos ||
            lstStr.find("tcpros") == std::string::npos ||
            lstStr.find("xmlrpc") == std::string::npos) {
            fprintf(stderr,"Missing one or more required carriers ");
            fprintf(stderr,"for yarpserver --ros (rossrv, tcpros, xmlrpc).\n");
            fprintf(stderr,"Run 'yarp connect --list-carriers' to see carriers on your machine\n");
            fprintf(stderr,"Aborting.\n");
            return false;
        }
        std::string addr = NetworkBase::getEnvironment("ROS_MASTER_URI");
        Contact c = Contact::fromString(addr);
        if (c.isValid()) {
            c.setCarrier("xmlrpc");
            c.setName("/ros");
            space = new RosNameSpace(c);
            subscriber.setDelegate(space);
            ns.setDelegate(space);
            fprintf(stderr, "Using ROS with ROS_MASTER_URI=%s\n", addr.c_str());
        } else {
            fprintf(stderr, "Cannot find ROS, check ROS_MASTER_URI (currently '%s')\n", addr.c_str());
            ::exit(1);
        }
    }

    config.minPortNumber = contact.getPort() + MIN_PORT_NUMBER_OFFSET;
    config.maxPortNumber = contact.getPort() + MAX_PORT_NUMBER_OFFSET;
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
