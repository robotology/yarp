/*
 * Copyright (C) 2008 RobotCub Consortium
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>, Lorenzo Natale
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Debug.h"
#include "RobotInterface.h"
#include "XMLReader.h"

#include "drivers/debugging.h"
#include "drivers/ControlBoardWrapper.h"
#include "drivers/ControlBoardWrapper2.h"

#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

YARP_DECLARE_DEVICES(icubmod)

namespace {
static bool terminated = false;
static bool askAbort = false;
//yarp::os::IRobotInterface *ri = 0;
}

static void sighandler (int) {
    static int ct = 0;
    ct++;

    warning() << "Asking to shut down";
    terminated = true;

    switch (ct) {
    case 1:
    case 2:
        break;
    case 3:
        warning() << "Aborting parking...";
//         if(ri!=0) {
//             ri->abort();
//         }
        break;
    case 4:
    case 5:
        warning() << "iCubInterface is already shutting down, this might take a while";
    case 6:
    default:
        warning() << "Seriously killing the application";
        yarp::os::exit(-1);
    }
}

int main(int argc, char *argv[])
{
    yarp::os::Network yarp; //initialize network, this goes before everything

    if (!yarp.checkNetwork()) {
        fatal() << "Sorry YARP network does not seem to be available, is the yarp server available?";
    }

    if( NULL == (AC_trace_file = fopen("/home/icub/trace.log", "w+")) )
    {
        printf("Cannot open file /home/icub/trace.log, using stdout\n");
        AC_trace_file = stdout;
    }

    if( NULL == (AC_debug_file = fopen("/home/icub/debug.log", "w+")) )
    {
        printf("Cannot open file /home/icub/debug.log, using stdout\n");
        AC_debug_file = stdout;
    }

    YARP_REGISTER_DEVICES(icubmod)

    //add local driver to factory
    yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<ControlBoardWrapper2>
        ("controlboardwrapper2",
         "",
         "ControlBoardWrapper2"));

    yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<ControlBoardWrapper>
        ("controlboardwrapper",
         "",
         "ControlBoardWrapper"));

    yarp::os::signal(yarp::os::YARP_SIGINT, sighandler);
    yarp::os::signal(yarp::os::YARP_SIGTERM, sighandler);

    yarp::os::Time::turboBoost();

    //for compatibility with old usage of iCubInterface, the use of the ResourceFinder
    //here is merely functional and should NOT be taken as an example
    yarp::os::ResourceFinder rf;
    rf.setVerbose();
    rf.setDefaultConfigFile("iCubInterface.ini");
    rf.configure("ICUB_ROOT", argc, argv);
    yarp::os::ConstString configFile = rf.findFile("config");
    yarp::os::ConstString cartRightArm = rf.findFile("cartRightArm");
    yarp::os::ConstString cartLeftArm = rf.findFile("cartLeftArm");

    RobotInterface::XMLReader reader;
    RobotInterface::Robot robot = reader.getRobot("/opt/iit/src/robotInterface/icub_torso.xml");

    debug() << robot;

    std::vector<yarp::dev::PolyDriver*> drivers;
    for (RobotInterface::DeviceList::const_iterator it = robot.devices().begin(); it != robot.devices().end(); it++) {
        const RobotInterface::Device &device = *it;

        debug() << device;

        Property p = device.paramsAsProperty();
        yarp::dev::PolyDriver *driver = new yarp::dev::PolyDriver();
        if (driver->open(p)) {
            drivers.push_back(driver);
        } else {
            warning() << "Cannot open device" << device.name();
        }
    }

    debug() << "Devices created";

    for (std::vector<yarp::dev::PolyDriver*>::iterator it = drivers.begin(); it != drivers.end(); it++) {
        yarp::dev::PolyDriver* driver = *it;
        if (!driver->isValid()) {
            warning() << "Driver is not valid";
        }
        if (!driver->close()) {
            warning() << "Cannot close driver";
        }
        delete driver;
    }
}
