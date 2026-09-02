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

    yarp::os::Network::cleanUnresponsivePorts(timeout);

    return 0;
}
