/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/serversql/impl/NameServerContainer.h>

#include <yarp/conf/environment.h>

#include <yarp/os/Carriers.h>
#include <yarp/os/Network.h>
#include <yarp/os/RosNameSpace.h>
#include <yarp/os/Value.h>
#include <yarp/name/BootstrapServer.h>
#include <yarp/serversql/impl/LogComponent.h>

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

namespace {
YARP_SERVERSQL_LOG_COMPONENT(NAMESERVERCONTAINER, "yarp.serversql.impl.NameServerContainer")
} // namespace


NameServerContainer::~NameServerContainer()
{
    subscriber.clear();
    if (space) {
        delete space;
        space = nullptr;
    }
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
        yCWarning(NAMESERVERCONTAINER, "The --memory option was given, but that is now a default. Continuing.");
    }

    std::string dbFilename = options.check("portdb",
                                           Value(dbDefault)).asString();
    std::string subdbFilename = options.check("subdb",
                                              Value(subdbDefault)).asString();

    std::string ip = options.check("ip",Value("...")).asString();
    int sock = options.check("socket",Value(Network::getDefaultPortRange())).asInt32();
    bool cautious = options.check("cautious");

    yCInfo(NAMESERVERCONTAINER, "Using port database: %s", dbFilename.c_str());
    yCInfo(NAMESERVERCONTAINER, "Using subscription database: %s", subdbFilename.c_str());
    if (dbFilename!=":memory:" || subdbFilename!=":memory:") {
        yCInfo(NAMESERVERCONTAINER, "If you ever need to clear the name server's state, just delete those files.");
    }
    yCInfo(NAMESERVERCONTAINER, "IP address: %s", (ip=="...")?"default":ip.c_str());
    yCInfo(NAMESERVERCONTAINER, "Port number: %d", sock);

    bool reset = false;
    if (options.check("ip")||options.check("socket")) {
        yCError(NAMESERVERCONTAINER, "Database needs to be reset, IP or port number set.");
        reset = true;
    }

    TripleSource *pmem = db.open(dbFilename.c_str(),cautious,reset);
    if (pmem == nullptr) {
        yCError(NAMESERVERCONTAINER, "Aborting, ports database failed to open.");
        return false;
    }

    if (!subscriber.open(subdbFilename)) {
        yCError(NAMESERVERCONTAINER, "Aborting, subscription database failed to open.");
        return false;
    }

    contact = Contact("...", "tcp", ip, sock);

    if (!options.check("local")) {
        if (!BootstrapServer::configFileBootstrap(contact,
                                                  options.check("read"),
                                                  options.check("write"))) {
            yCError(NAMESERVERCONTAINER, "Aborting.\n");
            return false;
        }
    }

    if (options.check("ros") || yarp::conf::environment::get_string("YARP_USE_ROS")!="") {
        yarp::os::Bottle lst = yarp::os::Carriers::listCarriers();
        std::string lstStr(lst.toString());
        if (lstStr.find("rossrv") == std::string::npos ||
            lstStr.find("tcpros") == std::string::npos ||
            lstStr.find("xmlrpc") == std::string::npos) {
            yCError(NAMESERVERCONTAINER, "Missing one or more required carriers ");
            yCError(NAMESERVERCONTAINER, "for yarpserver --ros (rossrv, tcpros, xmlrpc).\n");
            yCError(NAMESERVERCONTAINER, "Run 'yarp connect --list-carriers' to see carriers on your machine\n");
            yCError(NAMESERVERCONTAINER, "Aborting.\n");
            return false;
        }
        std::string addr = yarp::conf::environment::get_string("ROS_MASTER_URI");
        Contact c = Contact::fromString(addr);
        if (c.isValid()) {
            c.setCarrier("xmlrpc");
            c.setName("/ros");
            space = new RosNameSpace(c);
            subscriber.setDelegate(space);
            ns.setDelegate(space);
            yCInfo(NAMESERVERCONTAINER, "Using ROS with ROS_MASTER_URI=%s\n", addr.c_str());
        } else {
            yCFatal(NAMESERVERCONTAINER, "Cannot find ROS, check ROS_MASTER_URI (currently '%s')\n", addr.c_str());
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
