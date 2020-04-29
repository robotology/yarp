/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/serversql/Server.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Port.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Log.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/name/BootstrapServer.h>
#include <yarp/name/NameServerManager.h>
#include <yarp/serversql/impl/NameServerContainer.h>

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

namespace {
void print_callback(yarp::os::Log::LogType type,
                    const char* msg,
                    const char* file,
                    const unsigned int line,
                    const char* func,
                    double systemtime,
                    double remotetime,
                    const char* comp_name)
{
    YARP_UNUSED(type);
    YARP_UNUSED(file);
    YARP_UNUSED(line);
    YARP_UNUSED(func);
    YARP_UNUSED(systemtime);
    YARP_UNUSED(remotetime);
    YARP_UNUSED(comp_name);
    static const char* err_str = "[ERROR] ";
    static const char* warn_str = "[WARNING] ";
    static const char* no_str = "";
    printf("%s%s\n",
           ((type == yarp::os::Log::ErrorType) ? err_str : ((type == yarp::os::Log::WarningType) ? warn_str : no_str)),
           msg);
}
}

YARP_LOG_COMPONENT(SERVER,
                   "yarp.serversql.Server",
                   yarp::os::Log::InfoType,
                   yarp::os::Log::LogTypeReserved,
                   print_callback,
                   nullptr)


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
        if (silent) {
            fclose(out);
        }
        return 1;
    }

    nc.setSilent(silent);

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
        if (silent) {
            fclose(out);
        }
        return 1;
    }

    fallback.start();


    // Repeat registrations for the server and fallback server -
    // these registrations are more complete.
    yCInfo(SERVER, "Registering name server with itself:\n");
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

    yCInfo(SERVER, "Name server can be browsed at http://%s:%d/\n",
              nc.where().getHost().c_str(),
              nc.where().getPort());
    yCError(SERVER, "Ok. Ready!\n");

    while(!shouldStop) {
        messageCounter += pollingRate;
        SystemClock::delaySystem(pollingRate);
        double dummy;

        if(std::modf(messageCounter / 600.0, &dummy) < .00001) {
            yCInfo(SERVER, "Name server running happily\n");
        }
    }

    yCInfo(SERVER, "closing yarp server\n");
    server.close();
    if (silent) {
        fclose(out);
    }
    return 0;
}
