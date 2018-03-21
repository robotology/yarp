/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Istituto Italiano di Tecnologia (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */


#include "ymanager.h"
#include <yarp/os/Time.h>

int main(int argc, char* argv[])
{
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
    yarp.setVerbosity(-1);

    YConsoleManager ymanager(argc, argv);

    return 0;
}
