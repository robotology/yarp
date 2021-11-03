/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Thread.h>
#include <yarp/os/NetType.h>

#include <yarp/os/impl/LogForwarder.h>

#include <array>
#include <thread>

#include <catch.hpp>
#include <harness.h>

namespace {
YARP_LOG_COMPONENT(LOG_COMPONENT,
                   "yarp.test.os.LogStreamTest.cstyle",
                   yarp::os::Log::TraceType,
                   yarp::os::Log::TraceType)
YARP_LOG_COMPONENT(LOG_COMPONENT_NOFW,
                   "yarp.test.os.LogStreamTest.cstyle.nofw",
                   yarp::os::Log::TraceType,
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
YARP_LOG_COMPONENT(LOG_COMPONENT_NULL,
                   "yarp.test.os.LogStreamTest.cstyle.null",
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::LogTypeReserved,
                   nullptr,
                   nullptr)

YARP_LOG_COMPONENT(LOGSTREAM_COMPONENT,
                   "yarp.test.os.LogStreamTest.streamstyle",
                   yarp::os::Log::TraceType,
                   yarp::os::Log::TraceType)
YARP_LOG_COMPONENT(LOGSTREAM_COMPONENT_NOFW,
                   "yarp.test.os.LogStreamTest.streamstyle.nofw",
                   yarp::os::Log::TraceType,
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
YARP_LOG_COMPONENT(LOGSTREAM_COMPONENT_NULL,
                   "yarp.test.os.LogStreamTest.streamstyle.null",
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::LogTypeReserved,
                   nullptr,
                   nullptr)

constexpr const char* id = "test_id";
} // namespace

#if 1
# define CNT_RESET
# define CNT
#else
# define CNT_RESET int cnt = 1;
# define CNT fprintf(stderr, "    --- <%d> ---\n", cnt++);
#endif


TEST_CASE("os::LogStreamTest", "[yarp::os]")
{
    // Ensure that the log forwarder is initialized, in order to avoid mixing
    // the output
    yarp::os::impl::LogForwarder::getInstance();

    int i = 13;
    std::vector<int> v(4);
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    v[3] = 4;

    SECTION("Test yTrace")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        // Warning: By default, trace lines not using the component are
        //          not printed
        CNT yTrace("This is a trace");
        CNT yTrace("This is %s (%d)", "a trace", i);
        CNT yTrace("The end of line is removed from this trace\n");
        CNT yCTrace(LOG_COMPONENT, "This is a trace with a component");
        CNT yCTrace(LOG_COMPONENT, "This is %s (%d)", "a trace with a component", i);
        CNT yCTrace(LOG_COMPONENT, "The end of line is removed from this trace with a component\n");
        CNT yCTrace(LOG_COMPONENT_NOFW, "This trace with a component is not forwarded");
        CNT yCTrace(LOG_COMPONENT_NULL, "This trace with a component is neither not printed nor forwarded");

        CNT yTrace();
        CNT yTrace() << "This is" << "another" << "trace" << i;
        CNT yTrace() << v;
        CNT yTrace() << "The end of line is removed from this trace\n";
        CNT yCTrace(LOGSTREAM_COMPONENT);
        CNT yCTrace(LOGSTREAM_COMPONENT) << "This is" << "another" << "trace with a component" << i;
        CNT yCTrace(LOGSTREAM_COMPONENT) << v;
        CNT yCTrace(LOGSTREAM_COMPONENT_NOFW) << "This trace with a component is not forwarded" << i;
        CNT yCTrace(LOGSTREAM_COMPONENT_NULL) << "This trace with a component is neither not printed nor forwarded";

        CNT yTraceExternalTime(start, "This is a trace");
        CNT yTraceExternalTime(start, "This is %s (%d)", "a trace", i);
        CNT yTraceExternalTime(start, "The end of line is removed from this trace\n");
        CNT yCTraceExternalTime(LOG_COMPONENT, start, "This is a trace with a component");
        CNT yCTraceExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "a trace with a component", i);
        CNT yCTraceExternalTime(LOG_COMPONENT, start, "The end of line is removed from this trace with a component\n");
        CNT yCTraceExternalTime(LOG_COMPONENT_NOFW, start, "This trace with a component is not forwarded");
        CNT yCTraceExternalTime(LOG_COMPONENT_NULL, start, "This trace with a component is neither not printed nor forwarded");

        CNT yTraceExternalTime(start);
        CNT yTraceExternalTime(start) << "This is" << "another" << "trace" << i;
        CNT yTraceExternalTime(start) << v;
        CNT yTraceExternalTime(start) << "The end of line is removed from this trace\n";
        CNT yCTraceExternalTime(LOGSTREAM_COMPONENT, start);
        CNT yCTraceExternalTime(LOGSTREAM_COMPONENT, start) << "This is" << "another" << "trace with a component" << i;
        CNT yCTraceExternalTime(LOGSTREAM_COMPONENT, start) << v;
        CNT yCTraceExternalTime(LOGSTREAM_COMPONENT_NOFW, start) << "This trace with a component is not forwarded" << i;
        CNT yCTraceExternalTime(LOGSTREAM_COMPONENT_NULL, start) << "This trace with a component is neither not printed nor forwarded";

        CNT yITrace(id, "This is a trace with an id");
        CNT yITrace(id, "This is %s (%d)", "a trace with an id", i);
        CNT yITrace(id, "The end of line is removed from this trace with an id\n");
        CNT yCITrace(LOG_COMPONENT, id, "This is a trace with a component and an id");
        CNT yCITrace(LOG_COMPONENT, id, "This is %s (%d)", "a trace with a component and an id", i);
        CNT yCITrace(LOG_COMPONENT, id, "The end of line is removed from this trace with a component and an id\n");
        CNT yCITrace(LOG_COMPONENT_NOFW, id, "This trace with a component and an id is not forwarded");
        CNT yCITrace(LOG_COMPONENT_NULL, id, "This trace with a component and an id is neither not printed nor forwarded");

        CNT yITrace(id);
        CNT yITrace(id) << "This is" << "another" << "trace with an id" << i;
        CNT yITrace(id) << v;
        CNT yITrace(id) << "The end of line is removed from this trace with an id\n";
        CNT yCITrace(LOGSTREAM_COMPONENT, id);
        CNT yCITrace(LOGSTREAM_COMPONENT, id) << "This is" << "another" << "trace with a component and an id" << i;
        CNT yCITrace(LOGSTREAM_COMPONENT, id) << v;
        CNT yCITrace(LOGSTREAM_COMPONENT_NOFW, id) << "This trace with a component and an id is not forwarded" << i;
        CNT yCITrace(LOGSTREAM_COMPONENT_NULL, id) << "This trace with a component and an id is neither not printed nor forwarded";

        CNT yITraceExternalTime(id, start, "This is a trace with an id");
        CNT yITraceExternalTime(id, start, "This is %s (%d)", "a trace with an id", i);
        CNT yITraceExternalTime(id, start, "The end of line is removed from this trace with an id\n");
        CNT yCITraceExternalTime(LOG_COMPONENT, id, start, "This is a trace with a component and an id");
        CNT yCITraceExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "a trace with a component and an id", i);
        CNT yCITraceExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this trace with a component and an id\n");
        CNT yCITraceExternalTime(LOG_COMPONENT_NOFW, id, start, "This trace with a component and an id is not forwarded");
        CNT yCITraceExternalTime(LOG_COMPONENT_NULL, id, start, "This trace with a component and an id is neither not printed nor forwarded");

        CNT yITraceExternalTime(id, start);
        CNT yITraceExternalTime(id, start) << "This is" << "another" << "trace with an id" << i;
        CNT yITraceExternalTime(id, start) << v;
        CNT yITraceExternalTime(id, start) << "The end of line is removed from this trace with an id\n";
        CNT yCITraceExternalTime(LOGSTREAM_COMPONENT, id, start);
        CNT yCITraceExternalTime(LOGSTREAM_COMPONENT, id, start) << "This is" << "another" << "trace with a component and an id" << i;
        CNT yCITraceExternalTime(LOGSTREAM_COMPONENT, id, start) << v;
        CNT yCITraceExternalTime(LOGSTREAM_COMPONENT_NOFW, id, start) << "This trace with a component and an id is not forwarded" << i;
        CNT yCITraceExternalTime(LOGSTREAM_COMPONENT_NULL, id, start) << "This trace with a component and an id is neither not printed nor forwarded";
    }

    SECTION("Test yDebug")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        CNT yDebug("This is a debug");
        CNT yDebug("This is %s (%d)", "a debug", i);
        CNT yDebug("The end of line is removed from this debug\n");
        CNT yCDebug(LOG_COMPONENT, "This is a debug with a component");
        CNT yCDebug(LOG_COMPONENT, "This is %s (%d)", "a debug with a component", i);
        CNT yCDebug(LOG_COMPONENT, "The end of line is removed from this debug with a component\n");
        CNT yCDebug(LOG_COMPONENT_NOFW, "This debug with a component is not forwarded");
        CNT yCDebug(LOG_COMPONENT_NULL, "This debug with a component is neither not printed nor forwarded");

        CNT yDebug();
        CNT yDebug() << "This is" << "another" << "debug" << i;
        CNT yDebug() << v;
        CNT yDebug() << "The end of line is removed from this debug\n";
        CNT yCDebug(LOGSTREAM_COMPONENT);
        CNT yCDebug(LOGSTREAM_COMPONENT) << "This is" << "another" << "debug with a component" << i;
        CNT yCDebug(LOGSTREAM_COMPONENT) << v;
        CNT yCDebug(LOGSTREAM_COMPONENT_NOFW) << "This debug with a component is not forwarded" << i;
        CNT yCDebug(LOGSTREAM_COMPONENT_NULL) << "This debug with a component is neither not printed nor forwarded";

        CNT yDebugExternalTime(start, "This is a debug");
        CNT yDebugExternalTime(start, "This is %s (%d)", "a debug", i);
        CNT yDebugExternalTime(start, "The end of line is removed from this debug\n");
        CNT yCDebugExternalTime(LOG_COMPONENT, start, "This is a debug with a component");
        CNT yCDebugExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "a debug with a component", i);
        CNT yCDebugExternalTime(LOG_COMPONENT, start, "The end of line is removed from this debug with a component\n");
        CNT yCDebugExternalTime(LOG_COMPONENT_NOFW, start, "This debug with a component is not forwarded");
        CNT yCDebugExternalTime(LOG_COMPONENT_NULL, start, "This debug with a component is neither not printed nor forwarded");

        CNT yDebugExternalTime(start);
        CNT yDebugExternalTime(start) << "This is" << "another" << "debug" << i;
        CNT yDebugExternalTime(start) << v;
        CNT yDebugExternalTime(start) << "The end of line is removed from this debug\n";
        CNT yCDebugExternalTime(LOGSTREAM_COMPONENT, start);
        CNT yCDebugExternalTime(LOGSTREAM_COMPONENT, start) << "This is" << "another" << "debug with a component" << i;
        CNT yCDebugExternalTime(LOGSTREAM_COMPONENT, start) << v;
        CNT yCDebugExternalTime(LOGSTREAM_COMPONENT_NOFW, start) << "This debug with a component is not forwarded" << i;
        CNT yCDebugExternalTime(LOGSTREAM_COMPONENT_NULL, start) << "This debug with a component is neither not printed nor forwarded";

        CNT yIDebug(id, "This is a debug with an id");
        CNT yIDebug(id, "This is %s (%d)", "a debug with an id", i);
        CNT yIDebug(id, "The end of line is removed from this debug with an id\n");
        CNT yCIDebug(LOG_COMPONENT, id, "This is a debug with a component and an id");
        CNT yCIDebug(LOG_COMPONENT, id, "This is %s (%d)", "a debug with a component and an id", i);
        CNT yCIDebug(LOG_COMPONENT, id, "The end of line is removed from this debug with a component and an id\n");
        CNT yCIDebug(LOG_COMPONENT_NOFW, id, "This debug with a component and an id is not forwarded");
        CNT yCIDebug(LOG_COMPONENT_NULL, id, "This debug with a component and an id is neither not printed nor forwarded");

        CNT yIDebug(id);
        CNT yIDebug(id) << "This is" << "another" << "debug with an id" << i;
        CNT yIDebug(id) << v;
        CNT yIDebug(id) << "The end of line is removed from this debug with an id\n";
        CNT yCIDebug(LOGSTREAM_COMPONENT, id);
        CNT yCIDebug(LOGSTREAM_COMPONENT, id) << "This is" << "another" << "debug with a component and an id" << i;
        CNT yCIDebug(LOGSTREAM_COMPONENT, id) << v;
        CNT yCIDebug(LOGSTREAM_COMPONENT_NOFW, id) << "This debug with a component and an id is not forwarded" << i;
        CNT yCIDebug(LOGSTREAM_COMPONENT_NULL, id) << "This debug with a component and an id is neither not printed nor forwarded";

        CNT yIDebugExternalTime(id, start, "This is a debug with an id");
        CNT yIDebugExternalTime(id, start, "This is %s (%d)", "a debug with an id", i);
        CNT yIDebugExternalTime(id, start, "The end of line is removed from this debug with an id\n");
        CNT yCIDebugExternalTime(LOG_COMPONENT, id, start, "This is a debug with a component and an id");
        CNT yCIDebugExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "a debug with a component and an id", i);
        CNT yCIDebugExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this debug with a component and an id\n");
        CNT yCIDebugExternalTime(LOG_COMPONENT_NOFW, id, start, "This debug with a component and an id is not forwarded");
        CNT yCIDebugExternalTime(LOG_COMPONENT_NULL, id, start, "This debug with a component and an id is neither not printed nor forwarded");

        CNT yIDebugExternalTime(id, start);
        CNT yIDebugExternalTime(id, start) << "This is" << "another" << "debug with an id" << i;
        CNT yIDebugExternalTime(id, start) << v;
        CNT yIDebugExternalTime(id, start) << "The end of line is removed from this debug with an id\n";
        CNT yCIDebugExternalTime(LOGSTREAM_COMPONENT, id, start);
        CNT yCIDebugExternalTime(LOGSTREAM_COMPONENT, id, start) << "This is" << "another" << "debug with a component and an id" << i;
        CNT yCIDebugExternalTime(LOGSTREAM_COMPONENT, id, start) << v;
        CNT yCIDebugExternalTime(LOGSTREAM_COMPONENT_NOFW, id, start) << "This debug with a component and an id is not forwarded" << i;
        CNT yCIDebugExternalTime(LOGSTREAM_COMPONENT_NULL, id, start) << "This debug with a component and an id is neither not printed nor forwarded";
    }

    SECTION("Test yInfo")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        CNT yInfo("This is info");
        CNT yInfo("This is %s (%d)", "info", i);
        CNT yInfo("The end of line is removed from this info\n");
        CNT yCInfo(LOG_COMPONENT, "This is info with a component");
        CNT yCInfo(LOG_COMPONENT, "This is %s (%d)", "info with a component", i);
        CNT yCInfo(LOG_COMPONENT, "The end of line is removed from this info with a component\n");
        CNT yCInfo(LOG_COMPONENT_NOFW, "This info with a component is not forwarded");
        CNT yCInfo(LOG_COMPONENT_NULL, "This info with a component is neither not printed nor forwarded");

        CNT yInfo();
        CNT yInfo() << "This is" << "more" << "info" << i;
        CNT yInfo() << v;
        CNT yInfo() << "The end of line is removed from this info\n";
        CNT yCInfo(LOGSTREAM_COMPONENT);
        CNT yCInfo(LOGSTREAM_COMPONENT) << "This is" << "more" << "info with a component" << i;
        CNT yCInfo(LOGSTREAM_COMPONENT) << v;
        CNT yCInfo(LOGSTREAM_COMPONENT_NOFW) << "This info with a component is not forwarded" << i;
        CNT yCInfo(LOGSTREAM_COMPONENT_NULL) << "This info with a component is neither not printed nor forwarded";

        CNT yInfoExternalTime(start, "This is info");
        CNT yInfoExternalTime(start, "This is %s (%d)", "info", i);
        CNT yInfoExternalTime(start, "The end of line is removed from this info\n");
        CNT yCInfoExternalTime(LOG_COMPONENT, start, "This is info with a component");
        CNT yCInfoExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "info with a component", i);
        CNT yCInfoExternalTime(LOG_COMPONENT, start, "The end of line is removed from this info with a component\n");
        CNT yCInfoExternalTime(LOG_COMPONENT_NOFW, start, "This info with a component is not forwarded");
        CNT yCInfoExternalTime(LOG_COMPONENT_NULL, start, "This info with a component is neither not printed nor forwarded");

        CNT yInfoExternalTime(start);
        CNT yInfoExternalTime(start) << "This is" << "more" << "info" << i;
        CNT yInfoExternalTime(start) << v;
        CNT yInfoExternalTime(start) << "The end of line is removed from this info\n";
        CNT yCInfoExternalTime(LOGSTREAM_COMPONENT, start);
        CNT yCInfoExternalTime(LOGSTREAM_COMPONENT, start) << "This is" << "more" << "info with a component" << i;
        CNT yCInfoExternalTime(LOGSTREAM_COMPONENT, start) << v;
        CNT yCInfoExternalTime(LOGSTREAM_COMPONENT_NOFW, start) << "This info with a component is not forwarded" << i;
        CNT yCInfoExternalTime(LOGSTREAM_COMPONENT_NULL, start) << "This info with a component is neither not printed nor forwarded";

        CNT yIInfo(id, "This is info with an id");
        CNT yIInfo(id, "This is %s (%d)", "info with an id", i);
        CNT yIInfo(id, "The end of line is removed from this info with an id\n");
        CNT yCIInfo(LOG_COMPONENT, id, "This is info with a component and an id");
        CNT yCIInfo(LOG_COMPONENT, id, "This is %s (%d)", "info with a component and an id", i);
        CNT yCIInfo(LOG_COMPONENT, id, "The end of line is removed from this info with a component and an id\n");
        CNT yCIInfo(LOG_COMPONENT_NOFW, id, "This info with a component and an id is not forwarded");
        CNT yCIInfo(LOG_COMPONENT_NULL, id, "This info with a component and an id is neither not printed nor forwarded");

        CNT yIInfo(id);
        CNT yIInfo(id) << "This is" << "another" << "info with an id" << i;
        CNT yIInfo(id) << v;
        CNT yIInfo(id) << "The end of line is removed from this info with an id\n";
        CNT yCIInfo(LOGSTREAM_COMPONENT, id);
        CNT yCIInfo(LOGSTREAM_COMPONENT, id) << "This is" << "another" << "info with a component and an id" << i;
        CNT yCIInfo(LOGSTREAM_COMPONENT, id) << v;
        CNT yCIInfo(LOGSTREAM_COMPONENT_NOFW, id) << "This info with a component and an id is not forwarded" << i;
        CNT yCIInfo(LOGSTREAM_COMPONENT_NULL, id) << "This info with a component and an id is neither not printed nor forwarded";

        CNT yIInfoExternalTime(id, start, "This is info with an id");
        CNT yIInfoExternalTime(id, start, "This is %s (%d)", "info with an id", i);
        CNT yIInfoExternalTime(id, start, "The end of line is removed from this info with an id\n");
        CNT yCIInfoExternalTime(LOG_COMPONENT, id, start, "This is info with a component and an id");
        CNT yCIInfoExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "info with a component and an id", i);
        CNT yCIInfoExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this info with a component and an id\n");
        CNT yCIInfoExternalTime(LOG_COMPONENT_NOFW, id, start, "This info with a component and an id is not forwarded");
        CNT yCIInfoExternalTime(LOG_COMPONENT_NULL, id, start, "This info with a component and an id is neither not printed nor forwarded");

        CNT yIInfoExternalTime(id, start);
        CNT yIInfoExternalTime(id, start) << "This is" << "another" << "info with an id" << i;
        CNT yIInfoExternalTime(id, start) << v;
        CNT yIInfoExternalTime(id, start) << "The end of line is removed from this info with an id\n";
        CNT yCIInfoExternalTime(LOGSTREAM_COMPONENT, id, start);
        CNT yCIInfoExternalTime(LOGSTREAM_COMPONENT, id, start) << "This is" << "another" << "info with a component and an id" << i;
        CNT yCIInfoExternalTime(LOGSTREAM_COMPONENT, id, start) << v;
        CNT yCIInfoExternalTime(LOGSTREAM_COMPONENT_NOFW, id, start) << "This info with a component and an id is not forwarded" << i;
        CNT yCIInfoExternalTime(LOGSTREAM_COMPONENT_NULL, id, start) << "This info with a component and an id is neither not printed nor forwarded";
    }

    SECTION("Test yWarning")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        CNT yWarning("This is a warning");
        CNT yWarning("This is %s (%d)", "a warning", i);
        CNT yWarning("The end of line is removed from this warning\n");
        CNT yCWarning(LOG_COMPONENT, "This is a warning with a component");
        CNT yCWarning(LOG_COMPONENT, "This is %s (%d)", "a warning with a component", i);
        CNT yCWarning(LOG_COMPONENT, "The end of line is removed from this warning with a component\n");
        CNT yCWarning(LOG_COMPONENT_NOFW, "This warning with a component is not forwarded");
        CNT yCWarning(LOG_COMPONENT_NULL, "This warning with a component is neither not printed nor forwarded");

        CNT yWarning();
        CNT yWarning() << "This is" << "another" << "warning" << i;
        CNT yWarning() << v;
        CNT yWarning() << "The end of line is removed from this warning\n";
        CNT yCWarning(LOGSTREAM_COMPONENT);
        CNT yCWarning(LOGSTREAM_COMPONENT) << "This is" << "another" << "warning with a component" << i;
        CNT yCWarning(LOGSTREAM_COMPONENT) << v;
        CNT yCWarning(LOGSTREAM_COMPONENT_NOFW) << "This warning with a component is not forwarded" << i;
        CNT yCWarning(LOGSTREAM_COMPONENT_NULL) << "This warning with a component is neither not printed nor forwarded";

        CNT yWarningExternalTime(start, "This is a warning");
        CNT yWarningExternalTime(start, "This is %s (%d)", "a warning", i);
        CNT yWarningExternalTime(start, "The end of line is removed from this warning\n");
        CNT yCWarningExternalTime(LOG_COMPONENT, start, "This is a warning with a component");
        CNT yCWarningExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "a warning with a component", i);
        CNT yCWarningExternalTime(LOG_COMPONENT, start, "The end of line is removed from this warning with a component\n");
        CNT yCWarningExternalTime(LOG_COMPONENT_NOFW, start, "This warning with a component is not forwarded");
        CNT yCWarningExternalTime(LOG_COMPONENT_NULL, start, "This warning with a component is neither not printed nor forwarded");

        CNT yWarningExternalTime(start);
        CNT yWarningExternalTime(start) << "This is" << "another" << "warning" << i;
        CNT yWarningExternalTime(start) << v;
        CNT yWarningExternalTime(start) << "The end of line is removed from this warning\n";
        CNT yCWarningExternalTime(LOGSTREAM_COMPONENT, start);
        CNT yCWarningExternalTime(LOGSTREAM_COMPONENT, start) << "This is" << "another" << "warning with a component" << i;
        CNT yCWarningExternalTime(LOGSTREAM_COMPONENT, start) << v;
        CNT yCWarningExternalTime(LOGSTREAM_COMPONENT_NOFW, start) << "This warning with a component is not forwarded" << i;
        CNT yCWarningExternalTime(LOGSTREAM_COMPONENT_NULL, start) << "This warning with a component is neither not printed nor forwarded";

        CNT yIWarning(id, "This is a warning with an id");
        CNT yIWarning(id, "This is %s (%d)", "a warning with an id", i);
        CNT yIWarning(id, "The end of line is removed from this warning with an id\n");
        CNT yCIWarning(LOG_COMPONENT, id, "This is a warning with a component and an id");
        CNT yCIWarning(LOG_COMPONENT, id, "This is %s (%d)", "a warning with a component and an id", i);
        CNT yCIWarning(LOG_COMPONENT, id, "The end of line is removed from this warning with a component and an id\n");
        CNT yCIWarning(LOG_COMPONENT_NOFW, id, "This warning with a component and an id is not forwarded");
        CNT yCIWarning(LOG_COMPONENT_NULL, id, "This warning with a component and an id is neither not printed nor forwarded");

        CNT yIWarning(id);
        CNT yIWarning(id) << "This is" << "another" << "warning with an id" << i;
        CNT yIWarning(id) << v;
        CNT yIWarning(id) << "The end of line is removed from this warning with an id\n";
        CNT yCIWarning(LOGSTREAM_COMPONENT, id);
        CNT yCIWarning(LOGSTREAM_COMPONENT, id) << "This is" << "another" << "warning with a component and an id" << i;
        CNT yCIWarning(LOGSTREAM_COMPONENT, id) << v;
        CNT yCIWarning(LOGSTREAM_COMPONENT_NOFW, id) << "This warning with a component and an id is not forwarded" << i;
        CNT yCIWarning(LOGSTREAM_COMPONENT_NULL, id) << "This warning with a component and an id is neither not printed nor forwarded";

        CNT yIWarningExternalTime(id, start, "This is a warning with an id");
        CNT yIWarningExternalTime(id, start, "This is %s (%d)", "a warning with an id", i);
        CNT yIWarningExternalTime(id, start, "The end of line is removed from this warning with an id\n");
        CNT yCIWarningExternalTime(LOG_COMPONENT, id, start, "This is a warning with a component and an id");
        CNT yCIWarningExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "a warning with a component and an id", i);
        CNT yCIWarningExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this warning with a component and an id\n");
        CNT yCIWarningExternalTime(LOG_COMPONENT_NOFW, id, start, "This warning with a component and an id is not forwarded");
        CNT yCIWarningExternalTime(LOG_COMPONENT_NULL, id, start, "This warning with a component and an id is neither not printed nor forwarded");

        CNT yIWarningExternalTime(id, start);
        CNT yIWarningExternalTime(id, start) << "This is" << "another" << "warning with an id" << i;
        CNT yIWarningExternalTime(id, start) << v;
        CNT yIWarningExternalTime(id, start) << "The end of line is removed from this warning with an id\n";
        CNT yCIWarningExternalTime(LOGSTREAM_COMPONENT, id, start);
        CNT yCIWarningExternalTime(LOGSTREAM_COMPONENT, id, start) << "This is" << "another" << "warning with a component and an id" << i;
        CNT yCIWarningExternalTime(LOGSTREAM_COMPONENT, id, start) << v;
        CNT yCIWarningExternalTime(LOGSTREAM_COMPONENT_NOFW, id, start) << "This warning with a component and an id is not forwarded" << i;
        CNT yCIWarningExternalTime(LOGSTREAM_COMPONENT_NULL, id, start) << "This warning with a component and an id is neither not printed nor forwarded";
    }

    SECTION("Test yError")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        CNT yError("This is a error");
        CNT yError("This is %s (%d)", "a error", i);
        CNT yError("The end of line is removed from this error\n");
        CNT yCError(LOG_COMPONENT, "This is a error with a component");
        CNT yCError(LOG_COMPONENT, "This is %s (%d)", "a error with a component", i);
        CNT yCError(LOG_COMPONENT, "The end of line is removed from this error with a component\n");
        CNT yCError(LOG_COMPONENT_NOFW, "This error with a component is not forwarded");
        CNT yCError(LOG_COMPONENT_NULL, "This error with a component is neither not printed nor forwarded");

        CNT yError();
        CNT yError() << "This is" << "another" << "error" << i;
        CNT yError() << v;
        CNT yError() << "The end of line is removed from this error\n";
        CNT yCError(LOGSTREAM_COMPONENT);
        CNT yCError(LOGSTREAM_COMPONENT) << "This is" << "another" << "error with a component" << i;
        CNT yCError(LOGSTREAM_COMPONENT) << v;
        CNT yCError(LOGSTREAM_COMPONENT_NOFW) << "This error with a component is not forwarded" << i;
        CNT yCError(LOGSTREAM_COMPONENT_NULL) << "This error with a component is neither not printed nor forwarded";

        CNT yErrorExternalTime(start, "This is a error");
        CNT yErrorExternalTime(start, "This is %s (%d)", "a error", i);
        CNT yErrorExternalTime(start, "The end of line is removed from this error\n");
        CNT yCErrorExternalTime(LOG_COMPONENT, start, "This is a error with a component");
        CNT yCErrorExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "a error with a component", i);
        CNT yCErrorExternalTime(LOG_COMPONENT, start, "The end of line is removed from this error with a component\n");
        CNT yCErrorExternalTime(LOG_COMPONENT_NOFW, start, "This error with a component is not forwarded");
        CNT yCErrorExternalTime(LOG_COMPONENT_NULL, start, "This error with a component is neither not printed nor forwarded");

        CNT yErrorExternalTime(start);
        CNT yErrorExternalTime(start) << "This is" << "another" << "error" << i;
        CNT yErrorExternalTime(start) << v;
        CNT yErrorExternalTime(start) << "The end of line is removed from this error\n";
        CNT yCErrorExternalTime(LOGSTREAM_COMPONENT, start);
        CNT yCErrorExternalTime(LOGSTREAM_COMPONENT, start) << "This is" << "another" << "error with a component" << i;
        CNT yCErrorExternalTime(LOGSTREAM_COMPONENT, start) << v;
        CNT yCErrorExternalTime(LOGSTREAM_COMPONENT_NOFW, start) << "This error with a component is not forwarded" << i;
        CNT yCErrorExternalTime(LOGSTREAM_COMPONENT_NULL, start) << "This error with a component is neither not printed nor forwarded";

        CNT yIError(id, "This is an error with an id");
        CNT yIError(id, "This is %s (%d)", "an error with an id", i);
        CNT yIError(id, "The end of line is removed from this error with an id\n");
        CNT yCIError(LOG_COMPONENT, id, "This is an error with a component and an id");
        CNT yCIError(LOG_COMPONENT, id, "This is %s (%d)", "an error with a component and an id", i);
        CNT yCIError(LOG_COMPONENT, id, "The end of line is removed from this error with a component and an id\n");
        CNT yCIError(LOG_COMPONENT_NOFW, id, "This error with a component and an id is not forwarded");
        CNT yCIError(LOG_COMPONENT_NULL, id, "This error with a component and an id is neither not printed nor forwarded");

        CNT yIError(id);
        CNT yIError(id) << "This is" << "another" << "error with an id" << i;
        CNT yIError(id) << v;
        CNT yIError(id) << "The end of line is removed from this error with an id\n";
        CNT yCIError(LOGSTREAM_COMPONENT, id);
        CNT yCIError(LOGSTREAM_COMPONENT, id) << "This is" << "another" << "error with a component and an id" << i;
        CNT yCIError(LOGSTREAM_COMPONENT, id) << v;
        CNT yCIError(LOGSTREAM_COMPONENT_NOFW, id) << "This error with a component and an id is not forwarded" << i;
        CNT yCIError(LOGSTREAM_COMPONENT_NULL, id) << "This error with a component and an id is neither not printed nor forwarded";

        CNT yIErrorExternalTime(id, start, "This is an error with an id");
        CNT yIErrorExternalTime(id, start, "This is %s (%d)", "an error with an id", i);
        CNT yIErrorExternalTime(id, start, "The end of line is removed from this error with an id\n");
        CNT yCIErrorExternalTime(LOG_COMPONENT, id, start, "This is an error with a component and an id");
        CNT yCIErrorExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "an error with a component and an id", i);
        CNT yCIErrorExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this error with a component and an id\n");
        CNT yCIErrorExternalTime(LOG_COMPONENT_NOFW, id, start, "This error with a component and an id is not forwarded");
        CNT yCIErrorExternalTime(LOG_COMPONENT_NULL, id, start, "This error with a component and an id is neither not printed nor forwarded");

        CNT yIErrorExternalTime(id, start);
        CNT yIErrorExternalTime(id, start) << "This is" << "another" << "error with an id" << i;
        CNT yIErrorExternalTime(id, start) << v;
        CNT yIErrorExternalTime(id, start) << "The end of line is removed from this error with an id\n";
        CNT yCIErrorExternalTime(LOGSTREAM_COMPONENT, id, start);
        CNT yCIErrorExternalTime(LOGSTREAM_COMPONENT, id, start) << "This is" << "another" << "error with a component and an id" << i;
        CNT yCIErrorExternalTime(LOGSTREAM_COMPONENT, id, start) << v;
        CNT yCIErrorExternalTime(LOGSTREAM_COMPONENT_NOFW, id, start) << "This error with a component and an id is not forwarded" << i;
        CNT yCIErrorExternalTime(LOGSTREAM_COMPONENT_NULL, id, start) << "This error with a component and an id is neither not printed nor forwarded";
    }

    SECTION("Test yTraceOnce and yTraceThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yTraceOnce() << "This line is printed only once.";
            CNT yTraceOnce() << "Also this line is printed only once.";
            CNT yTraceExternalTimeOnce(start) << "Also this line is printed only once with external time.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yTraceOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yTraceThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yTraceThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yTraceThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCTraceOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCTraceThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCTraceThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCTraceThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yTraceExternalTimeOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yTraceExternalTimeThreadOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yTraceExternalTimeThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yTraceExternalTimeThreadThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCTraceExternalTimeOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCTraceExternalTimeThreadOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCTraceExternalTimeThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCTraceExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yITraceOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yITraceThreadOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yITraceThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yITraceThreadThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCITraceOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCITraceThreadOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCITraceThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCITraceThreadThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yITraceExternalTimeOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yITraceExternalTimeThreadOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yITraceExternalTimeThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yITraceExternalTimeThreadThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCITraceExternalTimeOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCITraceExternalTimeThreadOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCITraceExternalTimeThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCITraceExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yDebugOnce and yDebugThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yDebugOnce() << "This line is printed only once.";
            CNT yDebugOnce() << "Also this line is printed only once.";
            CNT yDebugExternalTimeOnce(start) << "Also this line is printed only once with external time.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yDebugOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yDebugThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yDebugThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yDebugThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCDebugOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCDebugThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCDebugThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCDebugThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yDebugExternalTimeOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yDebugExternalTimeThreadOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yDebugExternalTimeThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yDebugExternalTimeThreadThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCDebugExternalTimeOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCDebugExternalTimeThreadOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCDebugExternalTimeThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCDebugExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yIDebugOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yIDebugThreadOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yIDebugThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yIDebugThreadThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCIDebugOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCIDebugThreadOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCIDebugThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCIDebugThreadThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yIDebugExternalTimeOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yIDebugExternalTimeThreadOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yIDebugExternalTimeThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yIDebugExternalTimeThreadThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCIDebugExternalTimeOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCIDebugExternalTimeThreadOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCIDebugExternalTimeThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCIDebugExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yInfoOnce and yInfoThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yInfoOnce() << "This line is printed only once.";
            CNT yInfoOnce() << "Also this line is printed only once.";
            CNT yInfoExternalTimeOnce(start) << "Also this line is printed only once with external time.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yInfoOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yInfoThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yInfoThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yInfoThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCInfoOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCInfoThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCInfoThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCInfoThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yInfoExternalTimeOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yInfoExternalTimeThreadOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yInfoExternalTimeThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yInfoExternalTimeThreadThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCInfoExternalTimeOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCInfoExternalTimeThreadOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCInfoExternalTimeThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCInfoExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yIInfoOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yIInfoThreadOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yIInfoThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yIInfoThreadThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCIInfoOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCIInfoThreadOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCIInfoThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCIInfoThreadThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yIInfoExternalTimeOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yIInfoExternalTimeThreadOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yIInfoExternalTimeThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yIInfoExternalTimeThreadThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCIInfoExternalTimeOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCIInfoExternalTimeThreadOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCIInfoExternalTimeThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCIInfoExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yWarningOnce and yWarningThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yWarningOnce() << "This line is printed only once.";
            CNT yWarningOnce() << "Also this line is printed only once.";
            CNT yWarningExternalTimeOnce(start) << "Also this line is printed only once with external time.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yWarningOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yWarningThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yWarningThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yWarningThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCWarningOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCWarningThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCWarningThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCWarningThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yWarningExternalTimeOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yWarningExternalTimeThreadOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yWarningExternalTimeThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yWarningExternalTimeThreadThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCWarningExternalTimeOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCWarningExternalTimeThreadOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCWarningExternalTimeThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCWarningExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yIWarningOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yIWarningThreadOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yIWarningThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yIWarningThreadThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCIWarningOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCIWarningThreadOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCIWarningThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCIWarningThreadThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yIWarningExternalTimeOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yIWarningExternalTimeThreadOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yIWarningExternalTimeThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yIWarningExternalTimeThreadThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCIWarningExternalTimeOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCIWarningExternalTimeThreadOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCIWarningExternalTimeThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCIWarningExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yErrorOnce and yErrorThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yErrorOnce() << "This line is printed only once.";
            CNT yErrorOnce() << "Also this line is printed only once.";
            CNT yErrorExternalTimeOnce(start) << "Also this line is printed only once with external time.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yErrorOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yErrorThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yErrorThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yErrorThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCErrorOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCErrorThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCErrorThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCErrorThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yErrorExternalTimeOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yErrorExternalTimeThreadOnce(start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yErrorExternalTimeThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yErrorExternalTimeThreadThrottle(start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCErrorExternalTimeOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCErrorExternalTimeThreadOnce(LOGSTREAM_COMPONENT, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCErrorExternalTimeThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCErrorExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yIErrorOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yIErrorThreadOnce(id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yIErrorThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yIErrorThreadThrottle(id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCIErrorOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCIErrorThreadOnce(LOGSTREAM_COMPONENT, id)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCIErrorThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCIErrorThreadThrottle(LOGSTREAM_COMPONENT, id, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yIErrorExternalTimeOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yIErrorExternalTimeThreadOnce(id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yIErrorExternalTimeThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yIErrorExternalTimeThreadThrottle(id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCIErrorExternalTimeOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCIErrorExternalTimeThreadOnce(LOGSTREAM_COMPONENT, id, start)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCIErrorExternalTimeThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCIErrorExternalTimeThreadThrottle(LOGSTREAM_COMPONENT, id, start, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Other log tests")
    {
        CNT_RESET

        CNT yInfo() << "This is a very long long sentence. "
        "The purpose is to check if the logger is able to handle and display "
        "nicely very long messages, eventually displaying them on multiple "
        "lines or using other mechanisms (e.g. pop-up, user hints etc)";

        CNT yInfo() << "This is a sentence that spawn on multiple lines:\nThis is line 2.\nThis is line 3.\nThis is the last line.";

        CNT yInfo() << "\nThis is text contains an end of line at the beginning of the sentence";

        CNT yInfo() << "This is text contains an end of line at the end of the sentence\n";

        CNT yInfo() << "This is text contains special characters that could cause issues like 1-\", 2-(, 3-), 4-[, 5-], 6-{, 7-}, 8-\t, 9-%%";
    }
}
