/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/conf/environment.h>

#include <yarp/os/Contact.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/Carrier.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/impl/NameConfig.h>

using yarp::companion::impl::Companion;
using yarp::os::Contact;
using yarp::os::Carrier;
using yarp::os::Carriers;
using yarp::os::NetworkBase;
using yarp::os::OutputProtocol;
using yarp::os::impl::NameConfig;


int Companion::cmdDetect(int argc, char *argv[])
{
    //NameConfig nc;
    //NameClient& nic = NameClient::getNameClient2();
    //nc.fromFile();
    //nic.setScan();
    bool shouldUseServer = false;
    if (argc>0) {
        if (std::string(argv[0])=="--write") {
            //nic.setSave();
            shouldUseServer = true;
        } else {
            yCError(COMPANION, "Argument not understood");
            return 1;
        }
    }

    bool didScan = false;
    bool didUse = false;
    Contact addr = NetworkBase::detectNameServer(shouldUseServer,
                                                 didScan,
                                                 didUse);
    if (addr.isValid()) {
        yCInfo(COMPANION, "Checking for name server at ip %s port %d",
               addr.getHost().c_str(),
               addr.getPort());
        yCInfo(COMPANION, "If there is a long delay, try:");
        yCInfo(COMPANION, "  yarp conf --clean");
    }
    OutputProtocol *out = Carriers::connect(addr);
    bool ok = (out != nullptr);
    if (ok) {
        delete out;
    }
    if (ok) {
        yCInfo(COMPANION);
        yCInfo(COMPANION, "=========================================================");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== FOUND");
        yCInfo(COMPANION, "== %s is available at ip %s port %d",
               addr.getName().c_str(),
               addr.getHost().c_str(), addr.getPort());
        yCInfo(COMPANION, "== %s can be browsed at http://%s:%d/",
               addr.getName().c_str(),
               addr.getHost().c_str(), addr.getPort());
        if (didScan&&!didUse) {
            yCInfo(COMPANION, "== ");
            yCInfo(COMPANION, "== WARNING");
            yCInfo(COMPANION, "== This address was found by scanning the network, but");
            yCInfo(COMPANION, "== has not been saved to a configuration file.");
            yCInfo(COMPANION, "== Regular YARP programs will not be able to use the ");
            yCInfo(COMPANION, "== name server until this address is saved.  To do so:");
            yCInfo(COMPANION, "==   yarp detect --write");
        }
        if (didUse) {
            yCInfo(COMPANION, "== ");
            yCInfo(COMPANION, "== Address saved.");
            yCInfo(COMPANION, "== YARP programs will now be able to use the name server.");
        }
        yCInfo(COMPANION, "== ");
        yCInfo(COMPANION, "=========================================================");
    } else {
        yCInfo(COMPANION);
        yCInfo(COMPANION, "=========================================================");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== PROBLEM");
        yCInfo(COMPANION, "== No valid YARP name server was found.");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== TIPS");
        yCInfo(COMPANION, "== #1 Make sure a YARP name server is running.");
        yCInfo(COMPANION, "== A command for starting the server is:");
        yCInfo(COMPANION, "== $ yarp server");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== #2 Make sure the YARP name server is running in the");
        yCInfo(COMPANION, "== same namespace as you.  Your namespace is set as:");
        yCInfo(COMPANION, "==   %s", NetworkBase::getNameServerName().c_str());
        yCInfo(COMPANION, "== You can change your namespace to /EXAMPLE by doing:");
        yCInfo(COMPANION, "==   yarp namespace /EXAMPLE");
        yCInfo(COMPANION, "== You can check your namespace by doing:");
        yCInfo(COMPANION, "==   yarp namespace");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== #3 Find out the ip address and port number the YARP");
        yCInfo(COMPANION, "== name server is running on, and do:");
        yCInfo(COMPANION, "== $ yarp conf [ip address] [port number]");
        yCInfo(COMPANION, "== This information is printed out when the server is");
        yCInfo(COMPANION, "== started.");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "== #4 To determine the name of the required configuration");
        yCInfo(COMPANION, "== file for manual viewing/editing, do:");
        yCInfo(COMPANION, "== $ yarp conf");
        yCInfo(COMPANION, "== The simplest possible configuration file would look");
        yCInfo(COMPANION, "== like something this:");
        yCInfo(COMPANION, "==   192.168.0.1 10000");
        yCInfo(COMPANION, "==");
        yCInfo(COMPANION, "=========================================================");
        return 1;
    }
    return 0;
}
