/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>
#include <yarp/companion/yarpcompanion.h>

#include <yarp/os/ResourceFinder.h>

using yarp::companion::impl::Companion;
using yarp::os::ResourceFinder;

int yarp::companion::main(int argc, char *argv[])
{
    ResourceFinder& rf = ResourceFinder::getResourceFinderSingleton();
    if (!rf.isConfigured()) {
        rf.configure(argc, argv);
    }

    // eliminate 0th arg, the program name
    argc--;
    argv++;

    if (argc<=0) {
        yCInfo(COMPANION, "This is the YARP companion.");
        yCInfo(COMPANION, "Call with the argument \"help\" to see a list of ways to use this program.");
        return 0;
    }

    Companion& instance = Companion::getInstance();
    bool adminMode = false;
    bool more = true;
    while (more && argc>0) {
        more = false;
        std::string s = std::string(argv[0]);

        // "pray" command requires the full command line, therefore it is
        // handled before anything else
        if (s == std::string("pray")) {
            argc++;
            argv--;
            return instance.cmdPray(argc, argv);
        }

        if (s == std::string("verbose") || s == std::string("quiet")) {
            yCWarning(COMPANION, "The %s argument is deprecated.", s.c_str());
            argc--;
            argv++;
            more = true;
        }

        if (s == std::string("admin")) {
            adminMode = true;
            argc--;
            argv++;
            more = true;
        }
    }

    if (argc<=0) {
        yCError(COMPANION, "Please supply a command");
        return 1;
    }

    const char *cmd = argv[0];
    argc--;
    argv++;
    instance.setAdminMode(adminMode);
    return instance.dispatch(cmd, argc, argv);
}
