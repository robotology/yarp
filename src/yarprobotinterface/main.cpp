/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *          Lorenzo Natale <lorenzo.natale@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */


#include "Module.h"
#include <yarp/os/LogStream.h>

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/dev/Drivers.h>

#ifdef ICUB_USE_REALTIME_LINUX
#include <csignal>
#include <unistd.h>
#include <sys/mman.h>
#endif //ICUB_USE_REALTIME_LINUX

int main(int argc, char *argv[])
{
    yarp::os::Network yarp; //initialize network, this goes before everything

    if (!yarp.checkNetwork()) {
        yFatal() << "Sorry YARP network does not seem to be available, is the yarp server available?";
    }

#ifdef ICUB_USE_REALTIME_LINUX
    struct sched_param sch_param;
    sch_param.__sched_priority = sched_get_priority_max(SCHED_FIFO)/3; //33
    if( sched_setscheduler(0, SCHED_FIFO, &sch_param) != 0 ) {
        yWarning() << "Cannot set the scheduler to FIFO. (check superuser permission)";
    }
    //if( mlockall(MCL_CURRENT | MCL_FUTURE) != 0 )
    //    yWarning() << "Cannot lock memory swapping (check superuser permission)";
#endif //ICUB_USE_REALTIME_LINUX

    yarp::os::Time::turboBoost();

    yarp::os::ResourceFinder &rf(yarp::os::ResourceFinder::getResourceFinderSingleton());
    rf.setVerbose();
    rf.setDefaultConfigFile("yarprobotinterface.ini");
    rf.configure(argc, argv);

    // Create and run our module
    RobotInterface::Module module;
    return module.runModule(rf);
}
