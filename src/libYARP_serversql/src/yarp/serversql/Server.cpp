/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/serversql/Server.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Port.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/name/BootstrapServer.h>
#include <yarp/name/NameServerManager.h>
#include <yarp/serversql/impl/NameServerContainer.h>
#include <yarp/serversql/impl/LogComponent.h>

#include <cstdio>
#include <cmath>

using yarp::os::Bottle;
using yarp::os::Contact;
using yarp::os::Port;
using yarp::os::Property;
using yarp::os::SystemClock;
using yarp::os::impl::NameClient;
using yarp::name::BootstrapServer;
using yarp::name::NameServerManager;
using yarp::serversql::impl::NameServerContainer;

YARP_SERVERSQL_LOG_COMPONENT(SERVER, "yarp.serversql.Server")


int yarp::serversql::Server::run(int argc, char** argv)
{
    Property options;
    options.fromCommand(argc, argv, false);

    bool verbose = options.check("verbose");
    bool silent = options.check("silent");

    if (verbose) {
        yarp::serversql::impl::LogComponent::setMinumumLogType(yarp::os::Log::DebugType);
    } else if (silent) {
        yarp::serversql::impl::LogComponent::setMinumumLogType(yarp::os::Log::WarningType);
    }

    yCInfo(SERVER, R"===(
     ==========================
    | __  __ ___  ____   ____  |
    | \ \/ //   ||  _ \ |  _ \ |
    |  \  // /| || |/ / | |/ / |
    |  / // ___ ||  _ \ |  _/  |
    | /_//_/  |_||_| \_\|_|    |
    |                          |
     ==========================)===");
    yCInfo(SERVER);

    if (options.check("help")) {
        yCInfo(SERVER, "Welcome to the YARP name server.\n");
        yCInfo(SERVER, "  --write                  Write IP address and socket on the configuration file.\n");
        yCInfo(SERVER, "  --config filename.conf   Load options from a file.\n");
        yCInfo(SERVER, "  --portdb ports.db        Store port information in named database.\n");
        yCInfo(SERVER, "                           Must not be on an NFS file system.\n");
        yCInfo(SERVER, "                           Set to :memory: to store in memory (faster).\n");
        yCInfo(SERVER, "  --subdb subs.db          Store subscription information in named database.\n");
        yCInfo(SERVER, "                           Must not be on an NFS file system.\n");
        yCInfo(SERVER, "                           Set to :memory: to store in memory (faster).\n");
        yCInfo(SERVER, "  --ip IP.AD.DR.ESS        Set IP address of server.\n");
        yCInfo(SERVER, "  --socket NNNNN           Set port number of server.\n");
        yCInfo(SERVER, "  --web dir                Serve web resources from given directory.\n");
        yCInfo(SERVER, "  --no-web-cache           Reload pages from file for each request.\n");
        yCInfo(SERVER, "  --ros                    Delegate pub/sub to ROS name server.\n");
        yCInfo(SERVER, "  --silent                 Start in silent mode.\n");
        //this->stop();

        return 0;
    } else {
        yCInfo(SERVER, "Call with --help for information on available options\n");
    }

    NameServerContainer nc;
    if (!nc.open(options)) {
        return 1;
    }

    bool ok = false;
    NameServerManager name(nc);
    BootstrapServer fallback(name);
    Port server;
    Contact alt;
    Bottle cmd;
    Bottle reply;
    double messageCounter(0);
    double pollingRate(.1);

    name.setPort(server);
    server.setReaderCreator(name);

    ok = server.open(nc.where(),false);
    if (!ok) {
        yCError(SERVER, "Name server failed to open\n");
        return 1;
    }

    fallback.start();


    // Repeat registrations for the server and fallback server -
    // these registrations are more complete.
    yCInfo(SERVER, "Registering name server with itself");
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

    NameClient::getNameClient().send(cmd, reply);

    yCInfo(SERVER, "Name server can be browsed at http:/%s",
              nc.where().toURI(false).c_str());
    yCInfo(SERVER, "Ok. Ready!");

    while(!shouldStop) {
        messageCounter += pollingRate;
        SystemClock::delaySystem(pollingRate);
        double dummy;

        if(std::modf(messageCounter / 600.0, &dummy) < .00001) {
            yCInfo(SERVER, "Name server running happily");
        }
    }

    yCInfo(SERVER, "closing yarp server");
    server.close();
    return 0;
}
