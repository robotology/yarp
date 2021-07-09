/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::NetworkBase;
using yarp::os::Property;

int Companion::cmdSample(int argc, char *argv[]) {
    BufferedPort<Bottle> port;

    Property options;
    options.fromCommand(argc, argv, false);
    if (argc==0 || !((options.check("period")||options.check("rate"))&&options.check("output"))) {
        yCInfo(COMPANION, "This is yarp sample. Syntax:");
        yCInfo(COMPANION, "  yarp sample --output /port --period 0.01");
        yCInfo(COMPANION, "  yarp sample --output /port --rate 100");
        yCInfo(COMPANION, "  yarp sample --input /p1 --output /p2 --rate 50 --carrier udp");
        yCInfo(COMPANION, "  yarp sample --output /port --rate 100 --show");
        yCInfo(COMPANION, "Data is read from the input port and repeated on the output port at the");
        yCInfo(COMPANION, "specified rate/period.  If the 'show' flag is given, the data is also printed");
        yCInfo(COMPANION, "on standard output.");
        return 1;
    }

    if (!port.open(options.find("output").asString())) {
        yCError(COMPANION, "Failed to open output port");
        return 1;
    }
    if (options.check("period")) {
        port.setTargetPeriod(options.find("period").asFloat64());
    }
    if (options.check("rate")) {
        port.setTargetPeriod(1.0/options.find("rate").asFloat64());
    }
    if (options.check("input")) {
        std::string input = options.find("input").asString();
        std::string carrier = options.find("carrier").asString();
        if (carrier!="") {
            NetworkBase::connect(input.c_str(), port.getName().c_str(),
                                 carrier.c_str());
        } else {
            NetworkBase::connect(input, port.getName());
        }
    }

    bool show = options.check("show");
    while (true) {
        Bottle *bot = port.read();
        if (!bot) continue;
        if (show) {
            yCInfo(COMPANION, "%s", bot->toString().c_str());
        }
        if (port.getOutputCount()>0) {
            port.prepare() = *bot;
            port.write();
        }
    }

    return 0;
}
