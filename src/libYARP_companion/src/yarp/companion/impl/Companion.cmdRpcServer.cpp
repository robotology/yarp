/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/impl/Terminal.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::Port;


int Companion::cmdRpcServer(int argc, char *argv[])
{
    bool drop = false;
    bool stop = false;
    bool echo = false;
    while (argc>=1 && std::string(argv[0]).find("--")==0) {
        std::string cmd = argv[0];
        if (cmd=="--single") {
            drop = true;
        } else if (cmd=="--stop") {
            stop = true;
        } else if (cmd=="--echo") {
            echo = true;
        } else {
            yCError(COMPANION, "Option not recognized: %s", cmd.c_str());
            return 1;
        }
        argv++;
        argc--;
    }
    if (argc<1) {
        yCInfo(COMPANION, "Usage:");
        yCInfo(COMPANION, "  yarp rpcserver [--single] [--stop] [--echo] /port/name");
        yCInfo(COMPANION, "By default, this shows commands and waits for user to enter replies. Flags:");
        yCInfo(COMPANION, "  --single: respond to only a single command per connection, ROS-style");
        yCInfo(COMPANION, "  --stop: stop the server entirely after a single command");
        yCInfo(COMPANION, "  --echo: reply with the message received");
        return 1;
    }

    const char *name = argv[0];

    Port port;
    Companion::installHandler();
    port.setRpcServer();
    applyArgs(port);
    port.open(name);
    Companion::setUnregisterName(name);

    while (true) {
        yCInfo(COMPANION, "Waiting for a message...");
        Bottle cmd;
        Bottle response;
        port.read(cmd, true);
        yCInfo(COMPANION, "Message: %s", cmd.toString().c_str());
        yCInfo(COMPANION, "Reply: ");
        if (echo) {
            response = cmd;
        } else {
            std::string txt = yarp::os::impl::Terminal::getStdin();
            response.fromString(txt);
        }
        if (drop) {
            port.replyAndDrop(response);
        } else {
            port.reply(response);
        }
        if (stop) return 0;
    }
}
