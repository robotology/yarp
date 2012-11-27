/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>, Lorenzo Natale
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Module.h"
#include <debugStream/Debug.h>

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/dev/Drivers.h>

YARP_DECLARE_DEVICES(icubmod)

int main(int argc, char *argv[])
{
    yarp::os::Network yarp; //initialize network, this goes before everything

    if (!yarp.checkNetwork()) {
        yFatal() << "Sorry YARP network does not seem to be available, is the yarp server available?";
    }

    yarp::os::Time::turboBoost();

    YARP_REGISTER_DEVICES(icubmod)

    yarp::os::ResourceFinder rf;
    rf.setVerbose();
    rf.setDefaultConfigFile("robot-interface.ini");
    rf.configure(argc, argv);

    // Create and run our module
    RobotInterface::Module module;
    return module.runModule(rf);
}
