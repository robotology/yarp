/*
 * Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <vector>

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>


int main(int argc, char *argv[])
{
    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    int i = 13;
    std::vector<int> v(4);
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    v[3] = 4;

    yTrace("This is a trace");
    yTrace("This is %s (%d)", "a trace", i);
    yTrace();
    yTrace() << "This is" << "another" << "trace" << i;
    yTrace() << v;

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

    yFatal() << "This is the end.";
    yFatal("This is the end."); // should never be called.
}
