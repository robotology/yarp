/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Contact.h>
#include <yarp/os/impl/NameConfig.h>

using yarp::companion::impl::Companion;
using yarp::os::Contact;
using yarp::os::impl::NameConfig;

int Companion::cmdConf(int argc, char *argv[])
{
    NameConfig nc;
    if (argc==0) {
        yCInfo(COMPANION, "%s", nc.getConfigFileName().c_str());
        return 0;
    }
    if (argc>=2) {
        nc.fromFile();
        Contact prev = nc.getAddress();
        std::string prevMode = nc.getMode();
        Contact next(argv[0], atoi(argv[1]));
        nc.setAddress(next);
        if (argc>=3) {
            nc.setMode(argv[2]);
        } else {
            nc.setMode("yarp");
        }
        nc.toFile();
        nc.fromFile();
        Contact current = nc.getAddress();
        std::string currentMode = nc.getMode();
        yCInfo(COMPANION, "Configuration file:");
        yCInfo(COMPANION, "  %s", nc.getConfigFileName().c_str());
        if (prev.isValid()) {
            yCInfo(COMPANION, "Stored:");
            yCInfo(COMPANION, "  host %s port number %d (%s name server)",
                   prev.getHost().c_str(),
                   prev.getPort(),
                   prevMode.c_str());
        }
        if (current.isValid()) {
            yCInfo(COMPANION, "Now stores:");
            yCInfo(COMPANION, "  host %s port number %d (%s name server)",
                   current.getHost().c_str(),
                   current.getPort(),
                   currentMode.c_str());
        } else {
            yCError(COMPANION, "is not valid!");
            yCError(COMPANION, "Expected:");
            yCError(COMPANION, "  yarp conf [ip address] [port number]");
            yCError(COMPANION, "  yarp conf [ip address] [port number] [yarp|ros]");
            yCError(COMPANION, "For example:");
            yCError(COMPANION, "  yarp conf 192.168.0.1 10000");
            return 1;
        }
        return 0;
    }
    if (argc==1) {
        if (std::string(argv[0])=="--clean") {
            nc.toFile(true);
            yCInfo(COMPANION, "Cleared configuration file:");
            yCInfo(COMPANION, "  %s", nc.getConfigFileName().c_str());
            return 0;
        }
    }
    yCError(COMPANION, "Command not understood");
    return 1;
}
