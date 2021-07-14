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

int Companion::cmdRepeat(int argc, char *argv[])
{
    BufferedPort<Bottle> port;

    Property options;
    options.fromCommand(argc, argv, false);
    if (argc==0 || options.check("help"))
    {
        yCInfo(COMPANION, "This is yarp repeat. Syntax:");
        yCInfo(COMPANION, "  yarp repeat /port");
        yCInfo(COMPANION, "/port is both the input port and the output port which repeats data");
        return 1;
    }

    if (argc==1)
    {
        if (!port.open(argv[0]))
        {
            yCError(COMPANION, "Failed to open port: %s", argv[0]);
            return 1;
        }
    }
    else
    {
        yCError(COMPANION, "Invalid command syntax");
        return 1;
    }

    while (true)
    {
        Bottle *bot = port.read();
        if (!bot) {
            continue;
        }
        if (port.getOutputCount()>0)
        {
            port.prepare() = *bot;
            port.write();
        }
    }

    return 0;
}
