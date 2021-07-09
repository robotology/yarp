/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Carriers.h>
#include <yarp/os/Contact.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/impl/NameConfig.h>

using yarp::companion::impl::Companion;
using yarp::os::Carriers;
using yarp::os::Contact;
using yarp::os::NetworkBase;
using yarp::os::OutputProtocol;
using yarp::os::impl::NameConfig;

int Companion::cmdWhere(int argc, char *argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    NameConfig nc;
    nc.fromFile();
    if (nc.getAddress().isValid()) {
        yCInfo(COMPANION,
               "Looking for name server on %s, port number %d",
               nc.getAddress().getHost().c_str(),
               nc.getAddress().getPort());
        yCInfo(COMPANION, "If there is a long delay, try:");
        yCInfo(COMPANION, "  yarp conf --clean");
    }
    Contact address = NetworkBase::getNameServerContact();

    bool reachable = false;
    if (address.isValid()) {
        OutputProtocol *out = Carriers::connect(address);
        if (out != nullptr) {
            reachable = true;
            out->close();
            delete out;
            out = nullptr;
        }
    }

    if (address.isValid()&&reachable) {
        yCInfo(COMPANION,
               "%sName server %s is available at ip %s port %d",
               ((nc.getMode() == "ros") ? "ROS " : ""),
               nc.getNamespace().c_str(),
               address.getHost().c_str(),
               address.getPort());
        if (address.getCarrier()=="tcp") {
            yCInfo(COMPANION,
                   "Name server %s can be browsed at http://%s:%d/",
                   nc.getNamespace().c_str(),
                   address.getHost().c_str(),
                   address.getPort());
        }
    } else {
        NameConfig conf;
        bool haveFile = conf.fromFile();
        Contact address = conf.getAddress();

        yCInfo(COMPANION);
        yCInfo(COMPANION, "=======================================================================");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== PROBLEM");
        if (haveFile) {
            yCInfo(COMPANION, "== No valid YARP name server is available.");
            yCInfo(COMPANION, "== Here is the expected configuration:");
            yCInfo(COMPANION,
                   "==   host: %s port number: %d",
                   address.getHost().c_str(),
                   address.getPort());
            yCInfo(COMPANION, "==   namespace: %s", nc.getNamespace().c_str());
            if (conf.getMode()!="" && conf.getMode()!="//") {
                yCInfo(COMPANION, "==   type of name server: %s", conf.getMode().c_str());
            }
            yCInfo(COMPANION, "== But such a name server was not found.");
        } else {
            yCInfo(COMPANION, "== No address for a YARP name server is available.");
            yCInfo(COMPANION, "== A configuration file giving the location of the ");
            yCInfo(COMPANION, "== YARP name server is required, but was not found.");
        }
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== SHORT SOLUTION");
        yCInfo(COMPANION, "== If you are fairly confident there is a name server running, try:");
        yCInfo(COMPANION, "== $ yarp detect --write");
        yCInfo(COMPANION, "== If you just want to make a quick test, start your own name server:");
        yCInfo(COMPANION, "== $ yarp namespace /your/name");
        yCInfo(COMPANION, "== $ yarp server");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== DETAILED SOLUTION");
        yCInfo(COMPANION, "== To try to fix this problem automatically, do:");
        yCInfo(COMPANION, "== $ yarp detect --write");
        yCInfo(COMPANION, "== This will search your network for a nameserver");
        yCInfo(COMPANION, "== and then write the result to a configuration file.");
        yCInfo(COMPANION, "== If you know the address of the name server, you");
        yCInfo(COMPANION, "== can bypass this search by doing:");
        yCInfo(COMPANION, "== $ yarp conf [ip address] [port number]");
        yCInfo(COMPANION, "== If you would like to search the network for a");
        yCInfo(COMPANION, "== nameserver but *not* automatically update the");
        yCInfo(COMPANION, "== configuration file, do:");
        yCInfo(COMPANION, "== $ yarp detect");
        yCInfo(COMPANION, "== Or to determine the name of the required");
        yCInfo(COMPANION, "== configuration file for manual viewing/editing, do:");
        yCInfo(COMPANION, "== $ yarp conf");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "=======================================================================");

        return 1;
    }
    return 0;
}
