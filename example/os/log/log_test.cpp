/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vector>

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/ResourceFinder.h>
#include <iostream>

YARP_LOG_COMPONENT(LOG_COMPONENT, "yarp.example.log_test")
YARP_LOG_COMPONENT(LOG_COMPONENT_NOFW,
                   "yarp.example.log_test.nofw",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr);

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

    fprintf(stdout,"This is a fprintf(stdout), only yarprun will forward it\n");
    fprintf(stderr,"This is a fprintf(stderr), only yarprun will forward it\n");
    std::cout <<   "This is a cout, only yarprun will forward it" << std::endl;
    std::cerr <<   "This is a cerr, only yarprun will forward it" << std::endl;

    yTrace("This is a trace");
    yTrace("This is %s (%d)", "a trace", i);
    yTrace("The end of line is removed from this trace\n");
    yCTrace(LOG_COMPONENT, "This is a trace");
    yCTrace(LOG_COMPONENT, "This is %s (%d)", "a trace", i);
    yCTrace(LOG_COMPONENT, "The end of line is removed from this trace\n");
    yCTrace(LOG_COMPONENT_NOFW, "This trace is not forwarded");

    yTrace();
    yTrace() << "This is" << "another" << "trace" << i;
    yTrace() << v << "and there should be a space before \"and\" and after" << "the first \"after\"";
    yTrace() << "The end of line is removed from this trace\n";
    yCTrace(LOG_COMPONENT);
    yCTrace(LOG_COMPONENT) << "This is" << "another" << "trace" << i;
    yCTrace(LOG_COMPONENT) << v;
    yCTrace(LOG_COMPONENT_NOFW) << "This trace is not forwarded";
    yCTrace(LOG_COMPONENT_NOFW) << "This trace is not forwarded" << i;

    yDebug("This is a debug");
    yDebug("This is %s (%d)", "a debug", i);
    yDebug("The end of line is removed from this debug\n");
    yCDebug(LOG_COMPONENT, "This is a debug");
    yCDebug(LOG_COMPONENT, "This is %s (%d)", "a debug", i);
    yCDebug(LOG_COMPONENT, "The end of line is removed from this debug\n");
    yCDebug(LOG_COMPONENT, "This debug is not forwarded");
    yCDebug(LOG_COMPONENT_NOFW, "This debug is not forwarded");

    yDebug();
    yDebug() << "This is" << "another" << "debug" << i;
    yDebug() << v << "and there should be a space before \"and\" and after" << "the first \"after\"";
    yDebug() << "The end of line is removed from this debug\n";
    yCDebug(LOG_COMPONENT);
    yCDebug(LOG_COMPONENT) << "This is" << "another" << "debug" << i;
    yCDebug(LOG_COMPONENT) << v;
    yCDebug(LOG_COMPONENT_NOFW) << "This debug is not forwarded" << i;
    yCDebug(LOG_COMPONENT_NOFW) << "This debug is not forwarded";

    yInfo("This is info");
    yInfo("This is %s (%d)", "info", i);
    yInfo("The end of line is removed from this info\n");
    yCInfo(LOG_COMPONENT, "This is info");
    yCInfo(LOG_COMPONENT, "This is %s (%d)", "info", i);
    yCInfo(LOG_COMPONENT, "The end of line is removed from this info\n");
    yCInfo(LOG_COMPONENT_NOFW, "This info is not forwarded");

    yInfo();
    yInfo() << "This is" << "more" << "info" << i;
    yInfo() << v << "and there should be a space before \"and\" and after" << "the first \"after\"";
    yInfo() << "The end of line is removed from this info\n";
    yCInfo(LOG_COMPONENT);
    yCInfo(LOG_COMPONENT) << "This is" << "more" << "info" << i;
    yCInfo(LOG_COMPONENT) << v;
    yCInfo(LOG_COMPONENT_NOFW) << "This info is not forwarded" << i;
    yCInfo(LOG_COMPONENT_NOFW) << "This info is not forwarded";

    yWarning("This is a warning");
    yWarning("This is %s (%d)", "a warning", i);
    yWarning("The end of line is removed from this warning\n");
    yCWarning(LOG_COMPONENT, "This is a warning");
    yCWarning(LOG_COMPONENT, "This is %s (%d)", "a warning", i);
    yCWarning(LOG_COMPONENT, "The end of line is removed from this warning\n");
    yCWarning(LOG_COMPONENT_NOFW, "This warning is not forwarded");

    yWarning();
    yWarning() << "This is" << "another" << "warning" << i;
    yWarning() << v << "and there should be a space before \"and\" and after" << "the first \"after\"";
    yWarning() << "The end of line is removed from this warning\n";
    yCWarning(LOG_COMPONENT);
    yCWarning(LOG_COMPONENT) << "This is" << "another" << "warning" << i;
    yCWarning(LOG_COMPONENT) << v;
    yCWarning(LOG_COMPONENT_NOFW) << "This warning is not forwarded" << i;
    yCWarning(LOG_COMPONENT_NOFW) << "This warning is not forwarded";

    yError("This is an error");
    yError("This is %s (%d)", "an error", i);
    yError("The end of line is removed from this error\n");
    yCError(LOG_COMPONENT, "This is an error");
    yCError(LOG_COMPONENT, "This is %s (%d)", "an error", i);
    yCError(LOG_COMPONENT, "The end of line is removed from this error\n");
    yCError(LOG_COMPONENT_NOFW, "This error is not forwarded");

    yError();
    yError() << "This is" << "another" << "error" << i;
    yError() << v << "and there should be a space before \"and\" and after" << "the first \"after\"";
    yError() << "The end of line is removed from this error\n";
    yCError(LOG_COMPONENT);
    yCError(LOG_COMPONENT) << "This is" << "another" << "error" << i;
    yCError(LOG_COMPONENT) << v;
    yCError(LOG_COMPONENT_NOFW) << "This error is not forwarded" << i;
    yCError(LOG_COMPONENT_NOFW) << "This error is not forwarded";


    // yDebug("%d", "Ciao"); // This prints a warning during build if -Wformat is enabled

    if (!rf.check("no_fatal")) {
        yFatal() << "This is the end.";
        // Execution ends here, the next calls are never executed
        yFatal("This is the end.");
        yFatal("This is the end and the end of line is removed\n");
        yCFatal(LOG_COMPONENT) << "This is the end.";
        yCFatal(LOG_COMPONENT, "This is the end.");
        yCFatal(LOG_COMPONENT, "This is the end and the end of line is removed\n");
        yCFatal(LOG_COMPONENT_NOFW, "This is the end and is not forwarded");
        yCFatal(LOG_COMPONENT_NOFW) << "This is the end and is not forwarded" << i;
        yCFatal(LOG_COMPONENT_NOFW) << "This is the end and is not forwarded";
    }
    return 0;
}
