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

#ifdef ICUB_USE_REALTIME_LINUX
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#endif //ICUB_USE_REALTIME_LINUX

YARP_DECLARE_DEVICES(icubmod)

int main(int argc, char *argv[])
{
    yarp::os::Network yarp; //initialize network, this goes before everything

    if (!yarp.checkNetwork()) {
        yFatal() << "Sorry YARP network does not seem to be available, is the yarp server available?";
    }

#ifdef ICUB_USE_REALTIME_LINUX
	struct sched_param sch_param;
	sch_param.__sched_priority = sched_get_priority_max(SCHED_FIFO)/3; //33
	if( sched_setscheduler(0, SCHED_FIFO, &sch_param) != 0 )
		 yWarning() << "Cannot set the scheduler to FIFO. (check superuser permission)";

    if( mlockall(MCL_CURRENT | MCL_FUTURE) != 0 )
        yWarning() << "Cannot lock memory swapping (check superuser permission)";
#endif //ICUB_USE_REALTIME_LINUX

    yarp::os::Time::turboBoost();

    YARP_REGISTER_DEVICES(icubmod)

    yarp::os::ResourceFinder rf;
    rf.setVerbose();
    rf.setDefaultConfigFile("robotInterface.ini");
    rf.configure(argc, argv);

    // Create and run our module
    RobotInterface::Module module;
    return module.runModule(rf);
}
