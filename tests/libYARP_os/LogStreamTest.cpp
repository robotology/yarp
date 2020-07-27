/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
}

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

        CNT yWarning();
        CNT yWarning() << "This is" << "another" << "warning" << i;
        CNT yWarning() << v;
        CNT yWarning() << "The end of line is removed from this warning\n";
        CNT yCWarning(LOGSTREAM_COMPONENT);
        CNT yCWarning(LOGSTREAM_COMPONENT) << "This is" << "another" << "warning with a component" << i;
        CNT yCWarning(LOGSTREAM_COMPONENT) << v;
        CNT yCWarning(LOGSTREAM_COMPONENT_NOFW) << "This warning with a component is not forwarded" << i;
        CNT yCWarning(LOGSTREAM_COMPONENT_NULL) << "This warning with a component is neither not printed nor forwarded";
    }

    SECTION("Test yError")
    {
        CNT_RESET

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

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yTraceOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yTraceThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yTraceThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yTraceThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCTraceOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCTraceThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCTraceThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCTraceThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
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

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yDebugOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yDebugThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yDebugThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yDebugThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCDebugOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCDebugThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCDebugThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCDebugThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
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

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yInfoOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yInfoThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yInfoThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yInfoThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCInfoOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCInfoThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCInfoThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCInfoThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
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

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yWarningOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yWarningThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yWarningThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yWarningThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCWarningOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCWarningThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCWarningThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCWarningThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
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

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yErrorOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yErrorThreadOnce()
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yErrorThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yErrorThreadThrottle(period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCErrorOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCErrorThreadOnce(LOGSTREAM_COMPONENT)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCErrorThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCErrorThreadThrottle(LOGSTREAM_COMPONENT, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    // customtime tests
    SECTION("Test yTraceCustomTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        // Warning: By default, trace lines not using the component are
        //          not printed
        CNT yTraceCustomTime(now, "This is a trace");
        CNT yTraceCustomTime(now, "This is %s (%d)", "a trace", i);
        CNT yTraceCustomTime(now, "The end of line is removed from this trace\n");
        CNT yCTraceCustomTime(LOG_COMPONENT, now, "This is a trace with a component");
        CNT yCTraceCustomTime(LOG_COMPONENT, now, "This is %s (%d)", "a trace with a component", i);
        CNT yCTraceCustomTime(LOG_COMPONENT, now, "The end of line is removed from this trace with a component\n");
        CNT yCTraceCustomTime(LOG_COMPONENT_NOFW, now, "This trace with a component is not forwarded");
        CNT yCTraceCustomTime(LOG_COMPONENT_NULL, now, "This trace with a component is neither not printed nor forwarded");

        CNT yTraceCustomTime(now);
        CNT yTraceCustomTime(now) << "This is" << "another" << "trace" << i;
        CNT yTraceCustomTime(now) << v;
        CNT yTraceCustomTime(now) << "The end of line is removed from this trace\n";
        CNT yCTraceCustomTime(LOGSTREAM_COMPONENT, now);
        CNT yCTraceCustomTime(LOGSTREAM_COMPONENT, now) << "This is" << "another" << "trace with a component" << i;
        CNT yCTraceCustomTime(LOGSTREAM_COMPONENT, now) << v;
        CNT yCTraceCustomTime(LOGSTREAM_COMPONENT_NOFW, now) << "This trace with a component is not forwarded" << i;
        CNT yCTraceCustomTime(LOGSTREAM_COMPONENT_NULL, now) << "This trace with a component is neither not printed nor forwarded";
    }

    SECTION("Test yDebugCustomTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        CNT yDebugCustomTime(now, "This is a debug");
        CNT yDebugCustomTime(now, "This is %s (%d)", "a debug", i);
        CNT yDebugCustomTime(now, "The end of line is removed from this debug\n");
        CNT yCDebugCustomTime(LOG_COMPONENT, now, "This is a debug with a component");
        CNT yCDebugCustomTime(LOG_COMPONENT, now, "This is %s (%d)", "a debug with a component", i);
        CNT yCDebugCustomTime(LOG_COMPONENT, now, "The end of line is removed from this debug with a component\n");
        CNT yCDebugCustomTime(LOG_COMPONENT_NOFW, now, "This debug with a component is not forwarded");
        CNT yCDebugCustomTime(LOG_COMPONENT_NULL, now, "This debug with a component is neither not printed nor forwarded");

        CNT yDebugCustomTime(now);
        CNT yDebugCustomTime(now) << "This is" << "another" << "debug" << i;
        CNT yDebugCustomTime(now) << v;
        CNT yDebugCustomTime(now) << "The end of line is removed from this debug\n";
        CNT yCDebugCustomTime(LOGSTREAM_COMPONENT, now);
        CNT yCDebugCustomTime(LOGSTREAM_COMPONENT, now) << "This is" << "another" << "debug with a component" << i;
        CNT yCDebugCustomTime(LOGSTREAM_COMPONENT, now) << v;
        CNT yCDebugCustomTime(LOGSTREAM_COMPONENT_NOFW, now) << "This debug with a component is not forwarded" << i;
        CNT yCDebugCustomTime(LOGSTREAM_COMPONENT_NULL, now) << "This debug with a component is neither not printed nor forwarded";
    }

    SECTION("Test yInfoCustomTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        CNT yInfoCustomTime(now, "This is info");
        CNT yInfoCustomTime(now, "This is %s (%d)", "info", i);
        CNT yInfoCustomTime(now, "The end of line is removed from this info\n");
        CNT yCInfoCustomTime(LOG_COMPONENT, now, "This is info with a component");
        CNT yCInfoCustomTime(LOG_COMPONENT, now, "This is %s (%d)", "info with a component", i);
        CNT yCInfoCustomTime(LOG_COMPONENT, now, "The end of line is removed from this info with a component\n");
        CNT yCInfoCustomTime(LOG_COMPONENT_NOFW, now, "This info with a component is not forwarded");
        CNT yCInfoCustomTime(LOG_COMPONENT_NULL, now, "This info with a component is neither not printed nor forwarded");

        CNT yInfoCustomTime(now);
        CNT yInfoCustomTime(now) << "This is" << "more" << "info" << i;
        CNT yInfoCustomTime(now) << v;
        CNT yInfoCustomTime(now) << "The end of line is removed from this info\n";
        CNT yCInfoCustomTime(LOGSTREAM_COMPONENT, now);
        CNT yCInfoCustomTime(LOGSTREAM_COMPONENT, now) << "This is" << "more" << "info with a component" << i;
        CNT yCInfoCustomTime(LOGSTREAM_COMPONENT, now) << v;
        CNT yCInfoCustomTime(LOGSTREAM_COMPONENT_NOFW, now) << "This info with a component is not forwarded" << i;
        CNT yCInfoCustomTime(LOGSTREAM_COMPONENT_NULL, now) << "This info with a component is neither not printed nor forwarded";
    }

    SECTION("Test yWarningCustomTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        CNT yWarningCustomTime(now, "This is a warning");
        CNT yWarningCustomTime(now, "This is %s (%d)", "a warning", i);
        CNT yWarningCustomTime(now, "The end of line is removed from this warning\n");
        CNT yCWarningCustomTime(LOG_COMPONENT, now, "This is a warning with a component");
        CNT yCWarningCustomTime(LOG_COMPONENT, now, "This is %s (%d)", "a warning with a component", i);
        CNT yCWarningCustomTime(LOG_COMPONENT, now, "The end of line is removed from this warning with a component\n");
        CNT yCWarningCustomTime(LOG_COMPONENT_NOFW, now, "This warning with a component is not forwarded");
        CNT yCWarningCustomTime(LOG_COMPONENT_NULL, now, "This warning with a component is neither not printed nor forwarded");

        CNT yWarningCustomTime(now);
        CNT yWarningCustomTime(now) << "This is" << "another" << "warning" << i;
        CNT yWarningCustomTime(now) << v;
        CNT yWarningCustomTime(now) << "The end of line is removed from this warning\n";
        CNT yCWarningCustomTime(LOGSTREAM_COMPONENT, now);
        CNT yCWarningCustomTime(LOGSTREAM_COMPONENT, now) << "This is" << "another" << "warning with a component" << i;
        CNT yCWarningCustomTime(LOGSTREAM_COMPONENT, now) << v;
        CNT yCWarningCustomTime(LOGSTREAM_COMPONENT_NOFW, now) << "This warning with a component is not forwarded" << i;
        CNT yCWarningCustomTime(LOGSTREAM_COMPONENT_NULL, now) << "This warning with a component is neither not printed nor forwarded";
    }

    SECTION("Test yErrorCustomTime")
    {
        CNT_RESET

        double now = yarp::os::SystemClock::nowSystem();
        CNT yErrorCustomTime(now, "This is a error");
        CNT yErrorCustomTime(now, "This is %s (%d)", "a error", i);
        CNT yErrorCustomTime(now, "The end of line is removed from this error\n");
        CNT yCErrorCustomTime(LOG_COMPONENT, now, "This is a error with a component");
        CNT yCErrorCustomTime(LOG_COMPONENT, now, "This is %s (%d)", "a error with a component", i);
        CNT yCErrorCustomTime(LOG_COMPONENT, now, "The end of line is removed from this error with a component\n");
        CNT yCErrorCustomTime(LOG_COMPONENT_NOFW, now, "This error with a component is not forwarded");
        CNT yCErrorCustomTime(LOG_COMPONENT_NULL, now, "This error with a component is neither not printed nor forwarded");

        CNT yErrorCustomTime(now);
        CNT yErrorCustomTime(now) << "This is" << "another" << "error" << i;
        CNT yErrorCustomTime(now) << v;
        CNT yErrorCustomTime(now) << "The end of line is removed from this error\n";
        CNT yCErrorCustomTime(LOGSTREAM_COMPONENT, now);
        CNT yCErrorCustomTime(LOGSTREAM_COMPONENT, now) << "This is" << "another" << "error with a component" << i;
        CNT yCErrorCustomTime(LOGSTREAM_COMPONENT, now) << v;
        CNT yCErrorCustomTime(LOGSTREAM_COMPONENT_NOFW, now) << "This error with a component is not forwarded" << i;
        CNT yCErrorCustomTime(LOGSTREAM_COMPONENT_NULL, now) << "This error with a component is neither not printed nor forwarded";
    }

    SECTION("Test yTraceCustomTimeOnce and yTraceCustomTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yTraceCustomTimeOnce(start) << "This line is printed only once.";
            CNT yTraceCustomTimeOnce(start) << "Also this line is printed only once.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yTraceCustomTimeOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yTraceCustomTimeThreadOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yTraceCustomTimeThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yTraceCustomTimeThreadThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCTraceCustomTimeOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCTraceCustomTimeThreadOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCTraceCustomTimeThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCTraceCustomTimeThreadThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yDebugCustomTimeOnce and yDebugCustomTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yDebugCustomTimeOnce(start) << "This line is printed only once.";
            CNT yDebugCustomTimeOnce(start) << "Also this line is printed only once.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yDebugCustomTimeOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yDebugCustomTimeThreadOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yDebugCustomTimeThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yDebugCustomTimeThreadThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCDebugCustomTimeOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCDebugCustomTimeThreadOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCDebugCustomTimeThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCDebugCustomTimeThreadThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yInfoCustomTimeOnce and yInfoCustomTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yInfoCustomTimeOnce(start) << "This line is printed only once.";
            CNT yInfoCustomTimeOnce(start) << "Also this line is printed only once.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yInfoCustomTimeOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yInfoCustomTimeThreadOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yInfoCustomTimeThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yInfoCustomTimeThreadThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCInfoCustomTimeOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCInfoCustomTimeThreadOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCInfoCustomTimeThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCInfoCustomTimeThreadThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yWarningCustomTimeOnce and yWarningCustomTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yWarningCustomTimeOnce(start) << "This line is printed only once.";
            CNT yWarningCustomTimeOnce(start) << "Also this line is printed only once.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yWarningCustomTimeOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yWarningCustomTimeThreadOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yWarningCustomTimeThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yWarningCustomTimeThreadThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCWarningCustomTimeOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCWarningCustomTimeThreadOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCWarningCustomTimeThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCWarningCustomTimeThreadThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    SECTION("Test yErrorCustomTimeOnce and yErrorCustomTimeThrottle")
    {
        CNT_RESET

        const double start = yarp::os::SystemClock::nowSystem();
        static constexpr double test_duration = 1.3;
        static constexpr double period = 0.3;
        static constexpr size_t SZ = 4;
        std::array<std::thread, SZ> threads;
        for (auto& t : threads) {
            CNT yErrorCustomTimeOnce(start) << "This line is printed only once.";
            CNT yErrorCustomTimeOnce(start) << "Also this line is printed only once.";

            t = std::thread([start]()
            {
                while (true) {
                    double now = yarp::os::SystemClock::nowSystem();
                    if ((now - start) > test_duration) {
                        break;
                    }

                    CNT yErrorCustomTimeOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yErrorCustomTimeThreadOnce(now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yErrorCustomTimeThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yErrorCustomTimeThreadThrottle(now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s by every thread";

                    CNT yCErrorCustomTimeOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "] This line is printed only by the first thread coming here";

                    CNT yCErrorCustomTimeThreadOnce(LOGSTREAM_COMPONENT, now)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed by every thread";

                    CNT yCErrorCustomTimeThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
                        << "This line is printed at most once every" << period << "s";

                    CNT yCErrorCustomTimeThreadThrottle(LOGSTREAM_COMPONENT, now, period)
                        << "[Time:" << now - start
                        << "][Thread id: 0x" << yarp::os::NetType::toHexString(yarp::os::Thread::getKeyOfCaller()).c_str()
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
