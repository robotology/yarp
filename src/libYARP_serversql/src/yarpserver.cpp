/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/serversql/yarpserversql.h>
#include <yarp/serversql/Server.h>

#include <yarp/serversql/impl/TripleSourceCreator.h>
#include <yarp/serversql/impl/NameServiceOnTriples.h>
#include <yarp/serversql/impl/AllocatorOnTriples.h>
#include <yarp/serversql/impl/SubscriberOnSql.h>
#include <yarp/serversql/impl/StyleNameService.h>
#include <yarp/serversql/impl/ComposedNameService.h>
#include <yarp/serversql/impl/ParseName.h>

#include <yarp/conf/system.h>
#include <yarp/os/all.h>
#include <yarp/os/Os.h>
#include <yarp/os/RosNameSpace.h>
#include <yarp/os/Carriers.h>

#include <yarp/name/NameServerManager.h>
#include <yarp/name/BootstrapServer.h>

#include <yarp/os/impl/NameClient.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>


// For a yarp server running on the port 10000
// * the first port assigned will be 10002
// * the last port assigned will be 19999
#define MIN_PORT_NUMBER_OFFSET 2
#define MAX_PORT_NUMBER_OFFSET 9999

using namespace yarp::os;
using namespace yarp::name;
using namespace yarp::serversql::impl;
using namespace std;

class NameServerContainer : public ComposedNameService
{
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

    NameServerContainer()
    {
        silent = false;
        space = nullptr;
    }

    virtual ~NameServerContainer()
    {
        subscriber.clear();
        if (space) {
            delete space;
            space = nullptr;
        }
    }

    void setSilent(bool silent)
    {
        ns.setSilent(silent);
        subscriber.setSilent(silent);
        this->silent = silent;
    }

    const Contact& where()
    {
        return contact;
    }

    Contact whereDelegate()
    {
        if (!space) {
            return Contact();
        }
        return space->getNameServerContact();
    }

    void preregister(const Contact& c)
    {
        Network::registerContact(c);
        subscriber.welcome(c.getName().c_str(),1);
    }

    bool open(Searchable& options)
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

        if (!subscriber.open(subdbFilename.c_str())) {
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
            std::string lstStr(lst.toString().c_str());
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
            Contact c = Contact::fromString(addr.c_str());
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
};

int yarp::serversql::Server::run(int argc, char** argv)
{
    Property options;
    bool     silent(false);
    FILE*    out;

    options.fromCommand(argc, argv, false);
    silent = options.check("silent");
    out    = silent ? tmpfile() : stdout;

    fprintf(out, "    __  __ ___  ____   ____\n\
    \\ \\/ //   ||  _ \\ |  _ \\\n\
     \\  // /| || |/ / | |/ /\n\
     / // ___ ||  _ \\ |  _/\n\
    /_//_/  |_||_| \\_\\|_|\n\
    ========================\n\n");

    if (options.check("help")) {
        printf("Welcome to the YARP name server.\n");
        printf("  --write                  Write IP address and socket on the configuration file.\n");
        printf("  --config filename.conf   Load options from a file.\n");
        printf("  --portdb ports.db        Store port information in named database.\n");
        printf("                           Must not be on an NFS file system.\n");
        printf("                           Set to :memory: to store in memory (faster).\n");
        printf("  --subdb subs.db          Store subscription information in named database.\n");
        printf("                           Must not be on an NFS file system.\n");
        printf("                           Set to :memory: to store in memory (faster).\n");
        printf("  --ip IP.AD.DR.ESS        Set IP address of server.\n");
        printf("  --socket NNNNN           Set port number of server.\n");
        printf("  --web dir                Serve web resources from given directory.\n");
        printf("  --no-web-cache           Reload pages from file for each request.\n");
        printf("  --ros                    Delegate pub/sub to ROS name server.\n");
        printf("  --silent                 Start in silent mode.\n");
        //this->stop();
        if (silent) {
            fclose(out);
        }
        return 0;
    } else {
        fprintf(out, "Call with --help for information on available options\n");
    }

    NameServerContainer nc;
    if (!nc.open(options)) {
        return 1;
    }

    nc.setSilent(silent);

    bool ok = false;
    NameServerManager name(nc);
    BootstrapServer fallback(name);
    Port server;
    Contact alt;
    yarp::os::Bottle cmd;
    yarp::os::Bottle reply;
    double messageCounter(0);
    double pollingRate(.1);

    name.setPort(server);
    server.setReaderCreator(name);

    ok = server.open(nc.where(),false);
    if (!ok) {
        fprintf(stderr, "Name server failed to open\n");
        if (silent) {
            fclose(out);
        }
        return 1;
    }

    printf("\n");
    fallback.start();


    // Repeat registrations for the server and fallback server -
    // these registrations are more complete.
    fprintf(out, "Registering name server with itself:\n");
    nc.preregister(nc.where());
    nc.preregister(fallback.where());

    alt = nc.whereDelegate();

    if (alt.isValid()) {
        nc.preregister(alt);
    }
    nc.goPublic();

    //Setting nameserver property
    cmd.addString("set");
    cmd.addString(server.getName());
    cmd.addString("nameserver");
    cmd.addString("true");

    yarp::os::impl::NameClient::getNameClient().send(cmd, reply);

    fprintf(out, "Name server can be browsed at http://%s:%d/\n",
           nc.where().getHost().c_str(), nc.where().getPort());
    fprintf(out, "\nOk.  Ready!\n");

    while(!shouldStop) {
        messageCounter += pollingRate;
        SystemClock::delaySystem(pollingRate);
        double dummy;

        if(std::modf(messageCounter / 600.0, &dummy) < .00001) {
            fprintf(out, "Name server running happily\n");
        }
    }

    fprintf(out, "closing yarp server\n");
    if (silent) {
        fclose(out);
    }
    server.close();
    return 0;
}

yarp::os::NameStore *yarpserver_create(yarp::os::Searchable& options)
{
    NameServerContainer *nc = new NameServerContainer;
    if (!nc) {
        return nullptr;
    }
    nc->setSilent(true);
    if (!nc->open(options)) {
        delete nc;
        return nullptr;
    }
    nc->goPublic();
    return nc;
}

int yarpserver_main(int argc, char *argv[])
{
    Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
    yarp::serversql::Server yServer;
    return yServer.run(argc, argv);
}
