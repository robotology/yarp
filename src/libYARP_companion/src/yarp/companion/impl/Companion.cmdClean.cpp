/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/Contact.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/Network.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/Property.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::Carriers;
using yarp::os::Contact;
using yarp::os::NetworkBase;
using yarp::os::OutputProtocol;
using yarp::os::Property;
using yarp::os::Value;
using yarp::os::impl::NameConfig;


int Companion::cmdClean(int argc, char *argv[])
{
    Property options;
    if (argc==0) {
        yCInfo(COMPANION, "# If the cleaning process has long delays, you may wish to use a timeout, ");
        yCInfo(COMPANION, "# specifying how long to wait (in seconds) for a test connection to a port:");
        yCInfo(COMPANION, "#   yarp clean --timeout 5.0");
    } else {
        options.fromCommand(argc, argv, false);
    }

    NameConfig nc;
    std::string name = nc.getNamespace();
    Bottle msg, reply;
    msg.addString("bot");
    msg.addString("list");
    yCInfo(COMPANION, "Requesting list of ports from name server... ");
    NetworkBase::write(name,
                       msg,
                       reply);
    int ct = reply.size()-1;
    yCInfo(COMPANION, "got %d port%s", ct, (ct!=1)?"s":"");
    double timeout = -1;
    if (options.check("timeout")) {
        timeout = options.find("timeout").asFloat64();
    }
    if (timeout <= 0) {
        timeout = -1;
        yCInfo(COMPANION, "No timeout; to specify one, do \"yarp clean --timeout NN.N\"");
    } else {
        yCInfo(COMPANION, "Using a timeout of %g seconds", timeout);
    }
    for (size_t i=1; i<reply.size(); i++) {
        Bottle *entry = reply.get(i).asList();
        if (entry != nullptr) {
            std::string port = entry->check("name", Value("")).asString();
            if (port!="" && port!="fallback" && port!=name) {
                Contact c = Contact::fromConfig(*entry);
                if (c.getCarrier()=="mcast") {
                    yCInfo(COMPANION, "Skipping mcast port %s...", port.c_str());
                } else {
                    Contact addr = c;
                    yCInfo(COMPANION, "Testing %s at %s",
                           port.c_str(),
                           addr.toURI().c_str());
                    if (addr.isValid()) {
                        if (timeout>=0) {
                            addr.setTimeout((float)timeout);
                        }
                        OutputProtocol *out = Carriers::connect(addr);
                        if (out == nullptr) {
                            yCInfo(COMPANION, "* No response, removing port %s", port.c_str());
                            NetworkBase::unregisterName(port);
                        } else {
                            delete out;
                        }
                    }
                }
            } else {
                if (port!="") {
                    yCInfo(COMPANION, "Ignoring %s", port.c_str());
                }
            }
        }
    }
    yCInfo(COMPANION, "Giving name server a chance to do garbage collection.");
    std::string serverName = NetworkBase::getNameServerName();
    Bottle cmd2("gc"), reply2;
    NetworkBase::write(serverName, cmd2, reply2);
    yCInfo(COMPANION, "Name server says: %s", reply2.toString().c_str());

    return 0;
}
