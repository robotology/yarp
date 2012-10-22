/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>, Lorenzo Natale
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Module.h"

#include <debugStream/Debug.h>

#include "drivers/ControlBoardWrapper.h"

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

//BEGIN
// TODO Move in device library
YARP_DECLARE_DEVICES(icubmod)
//END


int main(int argc, char *argv[])
{
    yTrace();
    yarp::os::Network yarp; //initialize network, this goes before everything

    if (!yarp.checkNetwork()) {
        yFatal() << "Sorry YARP network does not seem to be available, is the yarp server available?";
    }

    yarp::os::Time::turboBoost();

    yTrace();

//BEGIN
// TODO Move in device library
    YARP_REGISTER_DEVICES(icubmod)

    yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<ControlBoardWrapper>
        ("controlboardwrapper",
         "",
         "ControlBoardWrapper"));
//END

    // Create and run our module
    RobotInterface::Module module;
    module.setName("/icub");
    return module.runModule(argc, argv);
}
