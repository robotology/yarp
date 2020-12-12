/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

int Companion::cmdRepeat(int argc, char *argv[])
{
    BufferedPort<Bottle> port;

    Property options;
    options.fromCommand(argc, argv, false);
    if (argc==0 || !(options.check("output")))
    {
        yCInfo(COMPANION, "This is yarp repeat. Syntax:");
        yCInfo(COMPANION, "  yarp sample --output /port");
        yCInfo(COMPANION, "Data is read from the input port and repeated on the output port");
        return 1;
    }

    if (!port.open(options.find("output").asString()))
    {
        yCError(COMPANION, "Failed to open output port");
        return 1;
    }

    if (options.check("input"))
    {
        std::string input = options.find("input").asString();
        std::string carrier = options.find("carrier").asString();
        if (carrier!="")
        {
            NetworkBase::connect(input.c_str(), port.getName().c_str(), carrier.c_str());
        }
        else
        {
            NetworkBase::connect(input, port.getName());
        }
    }

    while (true)
    {
        Bottle *bot = port.read();
        if (!bot) continue;
        if (port.getOutputCount()>0)
        {
            port.prepare() = *bot;
            port.write();
        }
    }

    return 0;
}
