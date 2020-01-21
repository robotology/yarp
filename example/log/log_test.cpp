/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <vector>

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <iostream>


int main(int argc, char *argv[])
{
    //yarp::os::Network yarp;

    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    int i = 13;
    std::vector<int> v(4);
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    v[3] = 4;

    //yarp::os::Time::delay(10);
    yTrace("This is a trace");
    yTrace("This is %s (%d)", "a trace", i);
    yTrace();
    yTrace() << "This is" << "another" << "trace" << i;
    yTrace() << v;

    fprintf(stdout,"This is a fprintf(stdout), only yarprun will forward it\n");
    fprintf(stderr,"This is a fprintf(stderr), only yarprun will forward it\n");
    std::cout <<   "This is a cout, only yarprun will forward it" << std::endl;
    std::cerr <<   "This is a cerr, only yarprun will forward it" << std::endl;

    yDebug("This is a debug");
    yDebug("This is %s (%d)", "a debug", i);
    yDebug();
    yDebug() << "This is" << "another" << "debug" << i;
    yDebug() << v;

    yInfo("This is info");
    yInfo("This is %s (%d)", "info", i);
    yInfo();
    yInfo() << "This is" << "more" << "info" << i;
    yInfo() << v;

    yWarning("This is a warning");
    yWarning("This is %s (%d)", "a warning", i);
    yWarning();
    yWarning() << "This is" << "another" << "warning" << i;
    yWarning() << v;

    yError("This is an error");
    yError("This is %s (%d)", "an error", i);
    yError();
    yError() << "This is" << "another" << "error" << i;
    yError() << v;

    if (rf.check("no_fatal")) return 0;

    //yarp::os::Time::delay(10);
    yFatal() << "This is the end.";
    yFatal("This is the end."); // should never be called.
}
