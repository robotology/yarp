/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>, Lorenzo Natale
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Debug.h"
#include "Module.h"

#include "drivers/ControlBoardWrapper.h"

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

//BEGIN
// TODO Move in device library
YARP_DECLARE_DEVICES(icubmod)
//END

#if 0
namespace {
static bool terminated = false;
//static bool askAbort = false;
//yarp::os::IRobotInterface *ri = 0;
}

static void sighandler (int) {
    static int ct = 0;
    ct++;

    yWarning() << "Asking to shut down";
    terminated = true;

//TODO
//    robot.enterPhase(RobotInterface::ActionPhaseShutdown);

    switch (ct) {
    case 1:
    case 2:
        break;
    case 3:
        yWarning() << "Aborting parking...";
//         if(ri!=0) {
//             ri->abort();
//         }
        break;
    case 4:
    case 5:
        yWarning() << "iCubInterface is already shutting down, this might take a while";
    case 6:
    default:
        yWarning() << "Seriously killing the application";
        yarp::os::exit(-1);
    }
}
#endif


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
