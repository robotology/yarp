/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Log.h>
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
}

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
    }

    SECTION("Test yDebug")
    {
        CNT_RESET

        CNT yDebug("This is a debug");
        CNT yDebug("This is %s (%d)", "a debug", i);
        CNT yDebug("The end of line is removed from this debug\n");
        CNT yCDebug(LOG_COMPONENT, "This is a debug with a component");
        CNT yCDebug(LOG_COMPONENT, "This is %s (%d)", "a debug with a component", i);
        CNT yCDebug(LOG_COMPONENT, "The end of line is removed from this debug with a component\n");
        CNT yCDebug(LOG_COMPONENT_NOFW, "This debug with a component is not forwarded");
        CNT yCTrace(LOG_COMPONENT_NULL, "This trace with a component is neither not printed nor forwarded");
    }

    SECTION("Test yInfo")
    {
        CNT_RESET

        CNT yInfo("This is info");
        CNT yInfo("This is %s (%d)", "info", i);
        CNT yInfo("The end of line is removed from this info\n");
        CNT yCInfo(LOG_COMPONENT, "This is info with a component");
        CNT yCInfo(LOG_COMPONENT, "This is %s (%d)", "info with a component", i);
        CNT yCInfo(LOG_COMPONENT, "The end of line is removed from this info with a component\n");
        CNT yCInfo(LOG_COMPONENT_NOFW, "This info with a component is not forwarded");
        CNT yCTrace(LOG_COMPONENT_NULL, "This trace with a component is neither not printed nor forwarded");
    }

    SECTION("Test yWarning")
    {
        CNT_RESET

        CNT yWarning("This is a warning");
        CNT yWarning("This is %s (%d)", "a warning", i);
        CNT yWarning("The end of line is removed from this warning\n");
        CNT yCWarning(LOG_COMPONENT, "This is a warning with a component");
        CNT yCWarning(LOG_COMPONENT, "This is %s (%d)", "a warning with a component", i);
        CNT yCWarning(LOG_COMPONENT, "The end of line is removed from this warning with a component\n");
        CNT yCWarning(LOG_COMPONENT_NOFW, "This warning with a component is not forwarded");
        CNT yCWarning(LOG_COMPONENT_NULL, "This warning with a component is neither not printed nor forwarded");
    }

    SECTION("Test yError")
    {
        CNT_RESET

        CNT yError("This is an error");
        CNT yError("This is %s (%d)", "an error", i);
        CNT yError("The end of line is removed from this error\n");
        CNT yCError(LOG_COMPONENT, "This is an error with a component");
        CNT yCError(LOG_COMPONENT, "This is %s (%d)", "an error with a component", i);
        CNT yCError(LOG_COMPONENT, "The end of line is removed from this error with a component\n");
        CNT yCError(LOG_COMPONENT_NOFW, "This error with a component is not forwarded");
        CNT yCError(LOG_COMPONENT_NULL, "This error with a component is neither not printed nor forwarded");
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
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yTraceThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yTraceThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yTraceThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCTraceOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCTraceThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCTraceThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCTraceThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
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
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yDebugThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yDebugThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yDebugThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCDebugOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCDebugThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCDebugThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCDebugThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
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
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yInfoThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yInfoThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yInfoThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCInfoOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCInfoThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCInfoThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCInfoThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
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
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yWarningThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yWarningThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yWarningThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCWarningOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCWarningThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCWarningThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCWarningThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
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
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yErrorThreadOnce(
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yErrorThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yErrorThreadThrottle(period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCErrorOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCErrorThreadOnce(LOG_COMPONENT,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCErrorThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCErrorThreadThrottle(LOG_COMPONENT, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    // same tests, but for customTime now
    SECTION("Test yTraceExternalTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        // Warning: By default, trace lines not using the component are
        //          not printed
        CNT yTraceExternalTime(now, "This is a trace");
        CNT yTraceExternalTime(now, "This is %s (%d)", "a trace", i);
        CNT yTraceExternalTime(now, "The end of line is removed from this trace\n");
        CNT yCTraceExternalTime(LOG_COMPONENT, now, "This is a trace with a component");
        CNT yCTraceExternalTime(LOG_COMPONENT, now, "This is %s (%d)", "a trace with a component", i);
        CNT yCTraceExternalTime(LOG_COMPONENT, now, "The end of line is removed from this trace with a component\n");
        CNT yCTraceExternalTime(LOG_COMPONENT_NOFW, now, "This trace with a component is not forwarded");
        CNT yCTraceExternalTime(LOG_COMPONENT_NULL, now, "This trace with a component is neither not printed nor forwarded");
    }

    SECTION("Test yDebugExternalTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        CNT yDebugExternalTime(now, "This is a debug");
        CNT yDebugExternalTime(now, "This is %s (%d)", "a debug", i);
        CNT yDebugExternalTime(now, "The end of line is removed from this debug\n");
        CNT yCDebugExternalTime(LOG_COMPONENT, now, "This is a debug with a component");
        CNT yCDebugExternalTime(LOG_COMPONENT, now, "This is %s (%d)", "a debug with a component", i);
        CNT yCDebugExternalTime(LOG_COMPONENT, now, "The end of line is removed from this debug with a component\n");
        CNT yCDebugExternalTime(LOG_COMPONENT_NOFW, now, "This debug with a component is not forwarded");
        CNT yCTraceExternalTime(LOG_COMPONENT_NULL, now, "This trace with a component is neither not printed nor forwarded");
    }

    SECTION("Test yInfoExternalTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        CNT yInfoExternalTime(now, "This is info");
        CNT yInfoExternalTime(now, "This is %s (%d)", "info", i);
        CNT yInfoExternalTime(now, "The end of line is removed from this info\n");
        CNT yCInfoExternalTime(LOG_COMPONENT, now, "This is info with a component");
        CNT yCInfoExternalTime(LOG_COMPONENT, now, "This is %s (%d)", "info with a component", i);
        CNT yCInfoExternalTime(LOG_COMPONENT, now, "The end of line is removed from this info with a component\n");
        CNT yCInfoExternalTime(LOG_COMPONENT_NOFW, now, "This info with a component is not forwarded");
        CNT yCTraceExternalTime(LOG_COMPONENT_NULL, now, "This trace with a component is neither not printed nor forwarded");
    }

    SECTION("Test yWarningExternalTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        CNT yWarningExternalTime(now, "This is a warning");
        CNT yWarningExternalTime(now, "This is %s (%d)", "a warning", i);
        CNT yWarningExternalTime(now, "The end of line is removed from this warning\n");
        CNT yCWarningExternalTime(LOG_COMPONENT, now, "This is a warning with a component");
        CNT yCWarningExternalTime(LOG_COMPONENT, now, "This is %s (%d)", "a warning with a component", i);
        CNT yCWarningExternalTime(LOG_COMPONENT, now, "The end of line is removed from this warning with a component\n");
        CNT yCWarningExternalTime(LOG_COMPONENT_NOFW, now, "This warning with a component is not forwarded");
        CNT yCWarningExternalTime(LOG_COMPONENT_NULL, now, "This warning with a component is neither not printed nor forwarded");
    }

    SECTION("Test yErrorExternalTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        CNT yErrorExternalTime(now, "This is an error");
        CNT yErrorExternalTime(now, "This is %s (%d)", "an error", i);
        CNT yErrorExternalTime(now, "The end of line is removed from this error\n");
        CNT yCErrorExternalTime(LOG_COMPONENT, now, "This is an error with a component");
        CNT yCErrorExternalTime(LOG_COMPONENT, now, "This is %s (%d)", "an error with a component", i);
        CNT yCErrorExternalTime(LOG_COMPONENT, now, "The end of line is removed from this error with a component\n");
        CNT yCErrorExternalTime(LOG_COMPONENT_NOFW, now, "This error with a component is not forwarded");
        CNT yCErrorExternalTime(LOG_COMPONENT_NULL, now, "This error with a component is neither not printed nor forwarded");
    }

    SECTION("Test yTraceExternalTimeOnce, yTraceExternalTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yTraceExternalTimeOnce(start, "This line is printed only once.");
            CNT yTraceExternalTimeOnce(start, "Also this line is printed only once.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yTraceExternalTimeOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yTraceExternalTimeThreadOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yTraceExternalTimeThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yTraceExternalTimeThreadThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCTraceExternalTimeOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCTraceExternalTimeThreadOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCTraceExternalTimeThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCTraceExternalTimeThreadThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yDebugExternalTimeOnce, yDebugExternalTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yDebugExternalTimeOnce(start, "This line is printed only once.");
            CNT yDebugExternalTimeOnce(start, "Also this line is printed only once.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yDebugExternalTimeOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yDebugExternalTimeThreadOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yDebugExternalTimeThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yDebugExternalTimeThreadThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCDebugExternalTimeOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCDebugExternalTimeThreadOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCDebugExternalTimeThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCDebugExternalTimeThreadThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yInfoExternalTimeOnce, yInfoExternalTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yInfoExternalTimeOnce(start, "This line is printed only once.");
            CNT yInfoExternalTimeOnce(start, "Also this line is printed only once.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yInfoExternalTimeOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yInfoExternalTimeThreadOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yInfoExternalTimeThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yInfoExternalTimeThreadThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCInfoExternalTimeOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCInfoExternalTimeThreadOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCInfoExternalTimeThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCInfoExternalTimeThreadThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yWarningExternalTimeOnce, yWarningExternalTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yWarningExternalTimeOnce(start, "This line is printed only once.");
            CNT yWarningExternalTimeOnce(start, "Also this line is printed only once.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yWarningExternalTimeOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yWarningExternalTimeThreadOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yWarningExternalTimeThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yWarningExternalTimeThreadThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCWarningExternalTimeOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCWarningExternalTimeThreadOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCWarningExternalTimeThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCWarningExternalTimeThreadThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yErrorExternalTimeOnce, yErrorExternalTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yErrorExternalTimeOnce(start, "This line is printed only once.");
            CNT yErrorExternalTimeOnce(start, "Also this line is printed only once.");
            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }
                    CNT yErrorExternalTimeOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yErrorExternalTimeThreadOnce(now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yErrorExternalTimeThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yErrorExternalTimeThreadThrottle(now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCErrorExternalTimeOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed only by the first thread coming here",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCErrorExternalTimeThreadOnce(LOG_COMPONENT, now,
                        "[Time: %f][Thread id: 0x%s] This line is printed by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str());
                    CNT yCErrorExternalTimeThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
                        period);
                    CNT yCErrorExternalTimeThreadThrottle(LOG_COMPONENT, now, period,
                        "[Time: %f][Thread id: 0x%s] This line is printed at most once every %fs by every thread",
                        now - start,
                        yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str(),
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
