/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/impl/NameConfig.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::impl::NameConfig;

int Companion::cmdNamespace(int argc, char *argv[])
{
    NameConfig nc;
    if (argc!=0) {
        std::string fname = nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
        yCInfo(COMPANION, "Setting namespace in: %s", fname.c_str());
        yCInfo(COMPANION, "Remove this file to revert to the default namespace (/root)");
        Bottle cmd;
        for (int i=0; i<argc; i++) {
            cmd.addString(argv[i]);
        }
        nc.writeConfig(fname, cmd.toString());
    }

    Bottle ns = nc.getNamespaces();

    //Bottle bot(nc.readConfig(fname).c_str());
    //std::string space = bot.get(0).asString().c_str();
    if (ns.size()==0) {
        yCInfo(COMPANION, "No namespace specified");
    }
    if (ns.size()==1) {
        yCInfo(COMPANION, "YARP namespace: %s", ns.get(0).asString().c_str());
    } else {
        yCInfo(COMPANION, "YARP namespaces: %s", ns.toString().c_str());
    }
    return 0;
}
