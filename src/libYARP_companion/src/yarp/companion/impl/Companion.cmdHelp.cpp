/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/LogStream.h>

using yarp::companion::impl::Companion;


int Companion::cmdHelp(int argc, char *argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    yCInfo(COMPANION, "Usage:");
    yCInfo(COMPANION, "  <yarp> [admin] <command> [arg1 [arg2 ...]]");
    yCInfo(COMPANION);
    yCInfo(COMPANION, "Here are commands you can use:");
    yCInfo(COMPANION);
    for (unsigned i=0; i < names.size(); i++) {
        std::string name = names[i];
        const std::string& tip = tips[i];
        while (name.length() < 16) {
            name += " ";
        }
        if (!tip.empty()) {
            yCInfo(COMPANION, "%s %s", name.c_str(), tip.c_str());
        }
    }
    return 0;
}
