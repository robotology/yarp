/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/Thread.h>
#include <yarp/os/NetType.h>

#include <yarp/os/impl/LogForwarder.h>

#include <array>
#include <thread>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

namespace {
YARP_LOG_COMPONENT(LOG_COMPONENT,
                   "yarp.test.os.LogTest.cstyle",
                   yarp::os::Log::TraceType,
                   yarp::os::Log::TraceType)
YARP_LOG_COMPONENT(LOG_COMPONENT_NOFW,
                   "yarp.test.os.LogTest.cstyle.nofw",
                   yarp::os::Log::TraceType,
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
YARP_LOG_COMPONENT(LOG_COMPONENT_NULL,
                   "yarp.test.os.LogTest.cstyle.null",
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

TEST_CASE("os::LogTest", "[yarp::os]")
{
    // Ensure that the log forwarder is initialized, in order to avoid mixing
    // the output
    yarp::os::impl::LogForwarder::getInstance();

    int i = 42;

    SECTION("Test yTrace")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        // Warning: By default, trace lines not using the component are
        //          not printed
        CNT yTrace("");
        CNT yTrace("This is a trace");
        CNT yTrace("This is %s (%d)", "a trace", i);
        CNT yTrace("The end of line is removed from this trace\n");
        CNT yCTrace(LOG_COMPONENT, "This is a trace with a component");
        CNT yCTrace(LOG_COMPONENT, "This is %s (%d)", "a trace with a component", i);
        CNT yCTrace(LOG_COMPONENT, "The end of line is removed from this trace with a component\n");
        CNT yCTrace(LOG_COMPONENT_NOFW, "This trace with a component is not forwarded");
        CNT yCTrace(LOG_COMPONENT_NULL, "This trace with a component is neither not printed nor forwarded");
        CNT yTraceExternalTime(start, "This is a trace");
        CNT yTraceExternalTime(start, "This is %s (%d)", "a trace", i);
        CNT yTraceExternalTime(start, "The end of line is removed from this trace\n");
        CNT yCTraceExternalTime(LOG_COMPONENT, start, "This is a trace with a component");
        CNT yCTraceExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "a trace with a component", i);
        CNT yCTraceExternalTime(LOG_COMPONENT, start, "The end of line is removed from this trace with a component\n");
        CNT yCTraceExternalTime(LOG_COMPONENT_NOFW, start, "This trace with a component is not forwarded");
        CNT yCTraceExternalTime(LOG_COMPONENT_NULL, start, "This trace with a component is neither not printed nor forwarded");
        CNT yITrace(id, "This is a trace with an id");
        CNT yITrace(id, "This is %s (%d)", "a trace with an id", i);
        CNT yITrace(id, "The end of line is removed from this trace with an id\n");
        CNT yCITrace(LOG_COMPONENT, id, "This is a trace with a component and an id");
        CNT yCITrace(LOG_COMPONENT, id, "This is %s (%d)", "a trace with a component and an id", i);
        CNT yCITrace(LOG_COMPONENT, id, "The end of line is removed from this trace with a component and an id\n");
        CNT yCITrace(LOG_COMPONENT_NOFW, id, "This trace with a component and an id is not forwarded");
        CNT yCITrace(LOG_COMPONENT_NULL, id, "This trace with a component and an id is neither not printed nor forwarded");
        CNT yITraceExternalTime(id, start, "This is a trace with an id");
        CNT yITraceExternalTime(id, start, "This is %s (%d)", "a trace with an id", i);
        CNT yITraceExternalTime(id, start, "The end of line is removed from this trace with an id\n");
        CNT yCITraceExternalTime(LOG_COMPONENT, id, start, "This is a trace with a component and an id");
        CNT yCITraceExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "a trace with a component and an id", i);
        CNT yCITraceExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this trace with a component and an id\n");
        CNT yCITraceExternalTime(LOG_COMPONENT_NOFW, id, start, "This trace with a component and an id is not forwarded");
        CNT yCITraceExternalTime(LOG_COMPONENT_NULL, id, start, "This trace with a component and an id is neither not printed nor forwarded");
    }

    SECTION("Test yDebug")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        CNT yDebug("");
        CNT yDebug("This is a debug");
        CNT yDebug("This is %s (%d)", "a debug", i);
        CNT yDebug("The end of line is removed from this debug\n");
        CNT yCDebug(LOG_COMPONENT, "This is a debug with a component");
        CNT yCDebug(LOG_COMPONENT, "This is %s (%d)", "a debug with a component", i);
        CNT yCDebug(LOG_COMPONENT, "The end of line is removed from this debug with a component\n");
        CNT yCDebug(LOG_COMPONENT_NOFW, "This debug with a component is not forwarded");
        CNT yCDebug(LOG_COMPONENT_NULL, "This debug with a component is neither not printed nor forwarded");
        CNT yDebugExternalTime(start, "This is a debug");
        CNT yDebugExternalTime(start, "This is %s (%d)", "a debug", i);
        CNT yDebugExternalTime(start, "The end of line is removed from this debug\n");
        CNT yCDebugExternalTime(LOG_COMPONENT, start, "This is a debug with a component");
        CNT yCDebugExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "a debug with a component", i);
        CNT yCDebugExternalTime(LOG_COMPONENT, start, "The end of line is removed from this debug with a component\n");
        CNT yCDebugExternalTime(LOG_COMPONENT_NOFW, start, "This debug with a component is not forwarded");
        CNT yCDebugExternalTime(LOG_COMPONENT_NULL, start, "This debug with a component is neither not printed nor forwarded");
        CNT yIDebug(id, "This is a debug with an id");
        CNT yIDebug(id, "This is %s (%d)", "a debug with an id", i);
        CNT yIDebug(id, "The end of line is removed from this debug with an id\n");
        CNT yCIDebug(LOG_COMPONENT, id, "This is a debug with a component and an id");
        CNT yCIDebug(LOG_COMPONENT, id, "This is %s (%d)", "a debug with a component and an id", i);
        CNT yCIDebug(LOG_COMPONENT, id, "The end of line is removed from this debug with a component and an id\n");
        CNT yCIDebug(LOG_COMPONENT_NOFW, id, "This debug with a component and an id is not forwarded");
        CNT yCIDebug(LOG_COMPONENT_NULL, id, "This debug with a component and an id is neither not printed nor forwarded");
        CNT yIDebugExternalTime(id, start, "This is a debug with an id");
        CNT yIDebugExternalTime(id, start, "This is %s (%d)", "a debug with an id", i);
        CNT yIDebugExternalTime(id, start, "The end of line is removed from this debug with an id\n");
        CNT yCIDebugExternalTime(LOG_COMPONENT, id, start, "This is a debug with a component and an id");
        CNT yCIDebugExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "a debug with a component and an id", i);
        CNT yCIDebugExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this debug with a component and an id\n");
        CNT yCIDebugExternalTime(LOG_COMPONENT_NOFW, id, start, "This debug with a component and an id is not forwarded");
        CNT yCIDebugExternalTime(LOG_COMPONENT_NULL, id, start, "This debug with a component and an id is neither not printed nor forwarded");
    }

    SECTION("Test yInfo")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        CNT yInfo("");
        CNT yInfo("This is info");
        CNT yInfo("This is %s (%d)", "info", i);
        CNT yInfo("The end of line is removed from this info\n");
        CNT yCInfo(LOG_COMPONENT, "This is info with a component");
        CNT yCInfo(LOG_COMPONENT, "This is %s (%d)", "info with a component", i);
        CNT yCInfo(LOG_COMPONENT, "The end of line is removed from this info with a component\n");
        CNT yCInfo(LOG_COMPONENT_NOFW, "This info with a component is not forwarded");
        CNT yCInfo(LOG_COMPONENT_NULL, "This info with a component is neither not printed nor forwarded");
        CNT yInfoExternalTime(start, "This is info");
        CNT yInfoExternalTime(start, "This is %s (%d)", "info", i);
        CNT yInfoExternalTime(start, "The end of line is removed from this info\n");
        CNT yCInfoExternalTime(LOG_COMPONENT, start, "This is info with a component");
        CNT yCInfoExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "info with a component", i);
        CNT yCInfoExternalTime(LOG_COMPONENT, start, "The end of line is removed from this info with a component\n");
        CNT yCInfoExternalTime(LOG_COMPONENT_NOFW, start, "This info with a component is not forwarded");
        CNT yCInfoExternalTime(LOG_COMPONENT_NULL, start, "This info with a component is neither not printed nor forwarded");
        CNT yIInfo(id, "This is info with an id");
        CNT yIInfo(id, "This is %s (%d)", "info with an id", i);
        CNT yIInfo(id, "The end of line is removed from this info with an id\n");
        CNT yCIInfo(LOG_COMPONENT, id, "This is info with a component and an id");
        CNT yCIInfo(LOG_COMPONENT, id, "This is %s (%d)", "info with a component and an id", i);
        CNT yCIInfo(LOG_COMPONENT, id, "The end of line is removed from this info with a component and an id\n");
        CNT yCIInfo(LOG_COMPONENT_NOFW, id, "This info with a component and an id is not forwarded");
        CNT yCIInfo(LOG_COMPONENT_NULL, id, "This info with a component and an id is neither not printed nor forwarded");
        CNT yIInfoExternalTime(id, start, "This is info with an id");
        CNT yIInfoExternalTime(id, start, "This is %s (%d)", "info with an id", i);
        CNT yIInfoExternalTime(id, start, "The end of line is removed from this info with an id\n");
        CNT yCIInfoExternalTime(LOG_COMPONENT, id, start, "This is info with a component and an id");
        CNT yCIInfoExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "info with a component and an id", i);
        CNT yCIInfoExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this info with a component and an id\n");
        CNT yCIInfoExternalTime(LOG_COMPONENT_NOFW, id, start, "This info with a component and an id is not forwarded");
        CNT yCIInfoExternalTime(LOG_COMPONENT_NULL, id, start, "This info with a component and an id is neither not printed nor forwarded");
    }

    SECTION("Test yWarning")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        CNT yWarning("");
        CNT yWarning("This is a warning");
        CNT yWarning("This is %s (%d)", "a warning", i);
        CNT yWarning("The end of line is removed from this warning\n");
        CNT yCWarning(LOG_COMPONENT, "This is a warning with a component");
        CNT yCWarning(LOG_COMPONENT, "This is %s (%d)", "a warning with a component", i);
        CNT yCWarning(LOG_COMPONENT, "The end of line is removed from this warning with a component\n");
        CNT yCWarning(LOG_COMPONENT_NOFW, "This warning with a component is not forwarded");
        CNT yCWarning(LOG_COMPONENT_NULL, "This warning with a component is neither not printed nor forwarded");
        CNT yWarningExternalTime(start, "This is a warning");
        CNT yWarningExternalTime(start, "This is %s (%d)", "a warning", i);
        CNT yWarningExternalTime(start, "The end of line is removed from this warning\n");
        CNT yCWarningExternalTime(LOG_COMPONENT, start, "This is a warning with a component");
        CNT yCWarningExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "a warning with a component", i);
        CNT yCWarningExternalTime(LOG_COMPONENT, start, "The end of line is removed from this warning with a component\n");
        CNT yCWarningExternalTime(LOG_COMPONENT_NOFW, start, "This warning with a component is not forwarded");
        CNT yCWarningExternalTime(LOG_COMPONENT_NULL, start, "This warning with a component is neither not printed nor forwarded");
        CNT yIWarning(id, "This is a warning with an id");
        CNT yIWarning(id, "This is %s (%d)", "a warning with an id", i);
        CNT yIWarning(id, "The end of line is removed from this warning with an id\n");
        CNT yCIWarning(LOG_COMPONENT, id, "This is a warning with a component and an id");
        CNT yCIWarning(LOG_COMPONENT, id, "This is %s (%d)", "a warning with a component and an id", i);
        CNT yCIWarning(LOG_COMPONENT, id, "The end of line is removed from this warning with a component and an id\n");
        CNT yCIWarning(LOG_COMPONENT_NOFW, id, "This warning with a component and an id is not forwarded");
        CNT yCIWarning(LOG_COMPONENT_NULL, id, "This warning with a component and an id is neither not printed nor forwarded");
        CNT yIWarningExternalTime(id, start, "This is a warning with an id");
        CNT yIWarningExternalTime(id, start, "This is %s (%d)", "a warning with an id", i);
        CNT yIWarningExternalTime(id, start, "The end of line is removed from this warning with an id\n");
        CNT yCIWarningExternalTime(LOG_COMPONENT, id, start, "This is a warning with a component and an id");
        CNT yCIWarningExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "a warning with a component and an id", i);
        CNT yCIWarningExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this warning with a component and an id\n");
        CNT yCIWarningExternalTime(LOG_COMPONENT_NOFW, id, start, "This warning with a component and an id is not forwarded");
        CNT yCIWarningExternalTime(LOG_COMPONENT_NULL, id, start, "This warning with a component and an id is neither not printed nor forwarded");
    }

    SECTION("Test yError")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();

        CNT yError("");
        CNT yError("This is an error");
        CNT yError("This is %s (%d)", "an error", i);
        CNT yError("The end of line is removed from this error\n");
        CNT yCError(LOG_COMPONENT, "This is an error with a component");
        CNT yCError(LOG_COMPONENT, "This is %s (%d)", "an error with a component", i);
        CNT yCError(LOG_COMPONENT, "The end of line is removed from this error with a component\n");
        CNT yCError(LOG_COMPONENT_NOFW, "This error with a component is not forwarded");
        CNT yCError(LOG_COMPONENT_NULL, "This error with a component is neither not printed nor forwarded");
        CNT yErrorExternalTime(start, "This is an error");
        CNT yErrorExternalTime(start, "This is %s (%d)", "an error", i);
        CNT yErrorExternalTime(start, "The end of line is removed from this error\n");
        CNT yCErrorExternalTime(LOG_COMPONENT, start, "This is an error with a component");
        CNT yCErrorExternalTime(LOG_COMPONENT, start, "This is %s (%d)", "an error with a component", i);
        CNT yCErrorExternalTime(LOG_COMPONENT, start, "The end of line is removed from this error with a component\n");
        CNT yCErrorExternalTime(LOG_COMPONENT_NOFW, start, "This error with a component is not forwarded");
        CNT yCErrorExternalTime(LOG_COMPONENT_NULL, start, "This error with a component is neither not printed nor forwarded");
        CNT yIError(id, "This is an error with an id");
        CNT yIError(id, "This is %s (%d)", "an error with an id", i);
        CNT yIError(id, "The end of line is removed from this error with an id\n");
        CNT yCIError(LOG_COMPONENT, id, "This is an error with a component and an id");
        CNT yCIError(LOG_COMPONENT, id, "This is %s (%d)", "an error with a component and an id", i);
        CNT yCIError(LOG_COMPONENT, id, "The end of line is removed from this error with a component and an id\n");
        CNT yCIError(LOG_COMPONENT_NOFW, id, "This error with a component and an id is not forwarded");
        CNT yCIError(LOG_COMPONENT_NULL, id, "This error with a component and an id is neither not printed nor forwarded");
        CNT yIErrorExternalTime(id, start, "This is an error with an id");
        CNT yIErrorExternalTime(id, start, "This is %s (%d)", "an error with an id", i);
        CNT yIErrorExternalTime(id, start, "The end of line is removed from this error with an id\n");
        CNT yCIErrorExternalTime(LOG_COMPONENT, id, start, "This is an error with a component and an id");
        CNT yCIErrorExternalTime(LOG_COMPONENT, id, start, "This is %s (%d)", "an error with a component and an id", i);
        CNT yCIErrorExternalTime(LOG_COMPONENT, id, start, "The end of line is removed from this error with a component and an id\n");
        CNT yCIErrorExternalTime(LOG_COMPONENT_NOFW, id, start, "This error with a component and an id is not forwarded");
        CNT yCIErrorExternalTime(LOG_COMPONENT_NULL, id, start, "This error with a component and an id is neither not printed nor forwarded");
    }

    SECTION("Test yTraceOnce, yTraceThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yTraceOnce("This line is printed only once.");
            CNT yTraceOnce("Also this line is printed only once.");
            CNT yTraceExternalTimeOnce(start, "Also this line is printed only once with external time.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yTraceOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yTraceThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yTraceThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yTraceThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCTraceOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCTraceThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCTraceThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCTraceThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yTraceExternalTimeOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yTraceExternalTimeThreadOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yTraceExternalTimeThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yTraceExternalTimeThreadThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCTraceExternalTimeOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCTraceExternalTimeThreadOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCTraceExternalTimeThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCTraceExternalTimeThreadThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yITraceOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yITraceThreadOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yITraceThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yITraceThreadThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCITraceOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCITraceThreadOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCITraceThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCITraceThreadThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yITraceExternalTimeOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yITraceExternalTimeThreadOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yITraceExternalTimeThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yITraceExternalTimeThreadThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCITraceExternalTimeOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCITraceExternalTimeThreadOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCITraceExternalTimeThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCITraceExternalTimeThreadThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yDebugOnce, yDebugThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yDebugOnce("This line is printed only once.");
            CNT yDebugOnce("Also this line is printed only once.");
            CNT yDebugExternalTimeOnce(start, "Also this line is printed only once with external time.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yDebugOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yDebugThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yDebugThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yDebugThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCDebugOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCDebugThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCDebugThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCDebugThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yDebugExternalTimeOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yDebugExternalTimeThreadOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yDebugExternalTimeThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yDebugExternalTimeThreadThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCDebugExternalTimeOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCDebugExternalTimeThreadOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCDebugExternalTimeThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCDebugExternalTimeThreadThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIDebugOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIDebugThreadOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIDebugThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIDebugThreadThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIDebugOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIDebugThreadOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIDebugThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIDebugThreadThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIDebugExternalTimeOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIDebugExternalTimeThreadOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIDebugExternalTimeThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIDebugExternalTimeThreadThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIDebugExternalTimeOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIDebugExternalTimeThreadOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIDebugExternalTimeThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIDebugExternalTimeThreadThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yInfoOnce, yInfoThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yInfoOnce("This line is printed only once.");
            CNT yInfoOnce("Also this line is printed only once.");
            CNT yInfoExternalTimeOnce(start, "Also this line is printed only once with external time.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yInfoOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yInfoThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yInfoThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yInfoThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCInfoOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCInfoThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCInfoThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCInfoThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yInfoExternalTimeOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yInfoExternalTimeThreadOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yInfoExternalTimeThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yInfoExternalTimeThreadThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCInfoExternalTimeOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCInfoExternalTimeThreadOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCInfoExternalTimeThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCInfoExternalTimeThreadThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIInfoOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIInfoThreadOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIInfoThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIInfoThreadThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIInfoOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIInfoThreadOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIInfoThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIInfoThreadThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIInfoExternalTimeOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIInfoExternalTimeThreadOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIInfoExternalTimeThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIInfoExternalTimeThreadThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIInfoExternalTimeOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIInfoExternalTimeThreadOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIInfoExternalTimeThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIInfoExternalTimeThreadThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);

                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yWarningOnce, yWarningThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yWarningOnce("This line is printed only once.");
            CNT yWarningOnce("Also this line is printed only once.");
            CNT yWarningExternalTimeOnce(start, "Also this line is printed only once with external time.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yWarningOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yWarningThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yWarningThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yWarningThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCWarningOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCWarningThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCWarningThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCWarningThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yWarningExternalTimeOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yWarningExternalTimeThreadOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yWarningExternalTimeThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yWarningExternalTimeThreadThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCWarningExternalTimeOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCWarningExternalTimeThreadOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCWarningExternalTimeThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCWarningExternalTimeThreadThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIWarningOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIWarningThreadOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIWarningThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIWarningThreadThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIWarningOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIWarningThreadOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIWarningThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIWarningThreadThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIWarningExternalTimeOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIWarningExternalTimeThreadOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIWarningExternalTimeThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIWarningExternalTimeThreadThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIWarningExternalTimeOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIWarningExternalTimeThreadOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIWarningExternalTimeThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIWarningExternalTimeThreadThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yErrorOnce, yErrorThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yErrorOnce("This line is printed only once.");
            CNT yErrorOnce("Also this line is printed only once.");
            CNT yErrorExternalTimeOnce(start, "Also this line is printed only once with external time.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yErrorOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yErrorThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yErrorThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yErrorThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCErrorOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCErrorThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCErrorThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCErrorThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yErrorExternalTimeOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yErrorExternalTimeThreadOnce(start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yErrorExternalTimeThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yErrorExternalTimeThreadThrottle(start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCErrorExternalTimeOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCErrorExternalTimeThreadOnce(LOG_COMPONENT, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCErrorExternalTimeThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCErrorExternalTimeThreadThrottle(LOG_COMPONENT, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIErrorOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIErrorThreadOnce(id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIErrorThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIErrorThreadThrottle(id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIErrorOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIErrorThreadOnce(LOG_COMPONENT, id,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIErrorThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIErrorThreadThrottle(LOG_COMPONENT, id, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIErrorExternalTimeOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIErrorExternalTimeThreadOnce(id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yIErrorExternalTimeThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yIErrorExternalTimeThreadThrottle(id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIErrorExternalTimeOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIErrorExternalTimeThreadOnce(LOG_COMPONENT, id, start,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCIErrorExternalTimeThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCIErrorExternalTimeThreadThrottle(LOG_COMPONENT, id, start, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::conf::numeric::to_hex_string(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
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

        CNT yInfo("This is a very long long sentence. "
        "The purpose is to check if the logger is able to handle and display "
        "nicely very long messages, eventually displaying them on multiple "
        "lines or using other mechanisms (e.g. pop-up, user hints etc)");

        CNT yInfo("This is an even longer sentence.\n"
        "The purpose is to check if the logger is able to handle messages\n"
        "that are longer than YARP_MAX_STATIC_LOG_MSG_SIZE (that at the time\n"
        "of writing this test is set to 1024 characters).\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890.");

        CNT yInfo("This is a sentence that spawn on multiple lines:\nThis is line 2.\nThis is line 3.\nThis is the last line.");

        CNT yInfo("\nThis is text contains an end of line at the beginning of the sentence");

        CNT yInfo("This is text contains an end of line at the end of the sentence\n");

        CNT yInfo("This is text contains special characters that could cause issues like 1-\", 2-(, 3-), 4-[, 5-], 6-{, 7-}, 8-\t, 9-%%");
    }
}
