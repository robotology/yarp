/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Module.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <yarp/dev/Drivers.h>

int main(int argc, char* argv[])
{
    yarp::os::Network yarp; //initialize network, this goes before everything

    if (!yarp.checkNetwork()) {
        yFatal() << "Sorry YARP network does not seem to be available, is the yarp server available?";
    }

    yarp::os::ResourceFinder& rf(yarp::os::ResourceFinder::getResourceFinderSingleton());
    rf.setDefaultConfigFile("yarprobotinterface.ini");
    rf.configure(argc, argv);

    // Create and run our module
    yarprobotinterface::Module module;
    return module.runModule(rf);
}
