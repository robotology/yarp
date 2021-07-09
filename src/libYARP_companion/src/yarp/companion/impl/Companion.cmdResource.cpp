/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

using yarp::companion::impl::Companion;
using yarp::os::Property;
using yarp::os::ResourceFinder;

int Companion::cmdResource(int argc, char *argv[])
{
    if (argc==0) {
        yCInfo(COMPANION, "Looks for, and prints the complete path to, resource files.");
        yCInfo(COMPANION, "Example usage:");
        yCInfo(COMPANION, "   yarp resource --context context-name --from file-name");
        yCInfo(COMPANION, "   yarp resource --context context-name --find file-name");
        yCInfo(COMPANION, "To show what a config file loads as, specify --show");
        yCInfo(COMPANION);
        yCInfo(COMPANION, "Note that the search through the available contexts complies");
        yCInfo(COMPANION, "with the current policies, therefore the content of the environment");
        yCInfo(COMPANION, "variable YARP_ROBOT_NAME might affect the final result");
        return 0;
    }
    ResourceFinder rf;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    rf.setVerbose();
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
    Property p;
    p.fromCommand(argc, argv, false);
    if (p.check("find")) {
        rf.setDefaultConfigFile(p.find("find").asString().c_str());
    }
    bool ok = rf.configure(argc, argv, false);
    if (ok) {
        if (rf.check("show")) {
            yCInfo(COMPANION, ">>> %s", rf.toString().c_str());
        }
        yCInfo(COMPANION, "\"%s\"", rf.findFile("from").c_str());
    }
    return (ok ? 0 : 1);
}
