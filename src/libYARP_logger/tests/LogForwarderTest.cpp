/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SystemClock.h>
#include <yarp/os/NetType.h>
#include <yarp/os/LogStream.h>

#include <yarp/os/impl/LogForwarder.h>
#include <yarp/logger/YarpLogger.h>

#include <array>
#include <thread>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

void testLogForwarder( yarp::os::impl::LogForwarder& fwd, yarp::yarpLogger::LoggerEngine* the_logger,std::string logportName, std::string carrier, double delay)
{
    INFO("Testing LogForwarder with carrier: " + carrier);

    //connect the logger to the forwarder
    bool logger_connected = yarp::os::Network::connect(logportName, "/logger", carrier, false);
    CHECK(logger_connected);

    const int max_num_messages = 1000;
    uint64_t counter = 0;

    yDebug() << "Now sending " << max_num_messages << " messages with a network delay of "+ std::to_string(delay) +"ms. It might take a while..";

    // send some messages
    const double start = yarp::os::SystemClock::nowSystem();
    double ptime = yarp::os::SystemClock::nowSystem();
    for (size_t i = 0; i < max_num_messages; i++)
    {
        double ctime = yarp::os::SystemClock::nowSystem();
        double difftime = ctime-ptime;
        std::string message = "message " + std::to_string(counter) + " " + std::to_string(ctime) + " " + std::to_string(difftime);
        fwd.forward(message);
        counter++;
        //   yarp::os::Time::delay(0.00001);
        ptime = ctime;
    }
    const double end = yarp::os::SystemClock::nowSystem();
    double elapsed = end-start;
    std::string info_msg;
    info_msg = "Time required to send log messages: " + std::to_string(elapsed) + "s " +
             + " (" + std::to_string(elapsed/double(max_num_messages)*1000.0) + "ms for message)";
    yDebug() << info_msg;
    if (delay!=0) {CHECK (elapsed < max_num_messages * delay);}

    //When adding a simulated delay to the network, some log messages might not arrive
    //immediately to the logger. Therefeore, it should be kept active for an amount
    //of time equal to the numer of messages multiplied by the delay time plus a small margin.
    double estimated_test_duration = (max_num_messages * delay) + elapsed;
    const double margin = 2.0; // 2 seconds margin to ensure that all messages are received
    info_msg = "Estimated time to wait to receive all log messages: " + std::to_string(estimated_test_duration)
             + " (+ margin = " + std::to_string(margin) + ") s";
    yDebug() << info_msg;
    yDebug() << "Waiting...";
    yarp::os::Time::delay(estimated_test_duration+margin);
    yDebug() << "Wait complete";

    //check what the logger received from the forwarder
    std::list<yarp::yarpLogger::MessageEntry> messages;
    the_logger->get_messages(messages);
    size_t messagesnum = messages.size();
    CHECK(messagesnum == max_num_messages);

    messages.clear();
    the_logger->clear();
    the_logger->get_messages(messages);
    messagesnum = messages.size();
    CHECK(messagesnum == 0);
}

TEST_CASE("logger::LogForwarderTest", "[yarp::logger]")
{
    YARP_REQUIRE_PLUGIN("simulated_network_delay", "portmonitor")

#if defined(DISABLE_FAILING_VALGRIND_TESTS)
    // Skipping because valgrind introduces unpredicatable delays
    // To receive all messages the logger should be kept running
    // for a longer (unknown) time.
    YARP_SKIP_TEST("Skipping failing tests under valgrind")
#endif

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::NetworkBase::setLocalMode(true);

    yarp::os::Time::delay(1.0);

    //create the forwarder
    static yarp::os::impl::LogForwarder& fwd =  yarp::os::impl::LogForwarder::getInstance();
    std::string logportName = fwd.getLogPortName();

    //create the logger
    yarp::yarpLogger::LoggerEngine* the_logger = nullptr;
    the_logger = new yarp::yarpLogger::LoggerEngine ("/logger");
    the_logger->start_logging();

    yarp::os::Time::delay(1.0);

    const double required_delay = 0.07; // 70 ms delay for the simulated network
    std::string required_delay_s = std::to_string(int(required_delay*1000.0));
    testLogForwarder(fwd, the_logger, logportName, "fast_tcp",0.0);
    testLogForwarder(fwd, the_logger, logportName, "fast_tcp+send.portmonitor+file.simulated_network_delay+delay_ms."+required_delay_s+"+type.dll", required_delay);
    testLogForwarder(fwd, the_logger, logportName, "fast_tcp+recv.portmonitor+file.simulated_network_delay+delay_ms."+required_delay_s+"+type.dll", required_delay);

    yarp::os::Time::delay(1.0);

    //stop the logger
    the_logger->stop_logging();
    delete the_logger;
    the_logger = nullptr;

    //stop the forwarder
    fwd.shutdown();
}
