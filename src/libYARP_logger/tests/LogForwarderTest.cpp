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

void testLogForwarder( yarp::os::impl::LogForwarder& fwd, yarp::yarpLogger::LoggerEngine* the_logger,std::string logportName, std::string carrier)
{
    INFO("Testing LogForwarder with carrier: " + carrier);

    //connect the logger to the forwarder
    bool logger_connected = yarp::os::Network::connect(logportName, "/logger", carrier, false);
    CHECK(logger_connected);

    const int max_num_messages = 10000;
    uint64_t counter = 0;
    double t = 0.00001;

    const double start = yarp::os::SystemClock::nowSystem();

    // send some messages
    double ptime = yarp::os::SystemClock::nowSystem();
    for (size_t i = 0; i < max_num_messages; i++)
    {
        double ctime = yarp::os::SystemClock::nowSystem();
        double difftime = ctime-ptime;
        std::string message = "message " + std::to_string(counter) + " " + std::to_string(ctime) + " " + std::to_string(difftime);
        fwd.forward(message);
        counter++;
        yarp::os::Time::delay(t);
        ptime = ctime;
    }
    const double end = yarp::os::SystemClock::nowSystem();
    double elapsed = end-start;
    std::string info_msg = "Test duration: " + std::to_string(elapsed);
    yDebug() << info_msg;

    yarp::os::Time::delay(1.0);

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

    testLogForwarder(fwd, the_logger, logportName, "fast_tcp");
    testLogForwarder(fwd, the_logger, logportName, "fast_tcp+send.portmonitor+file.simulated_network_delay+delay_ms.100+type.dll");
    testLogForwarder(fwd, the_logger, logportName, "fast_tcp+recv.portmonitor+file.simulated_network_delay+delay_ms.100+type.dll");

    yarp::os::Time::delay(1.0);

    //stop the logger
    the_logger->stop_logging();
    delete the_logger;
    the_logger = nullptr;

    //stop the forwarder
    fwd.shutdown();
}
