/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/NetworkClock.h>
#include <yarp/os/SystemClock.h>

#include <cstdlib>
#include <cstring>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::NetworkClockTest", "[yarp::os]")
{
    NetworkBase::setLocalMode(true);

    SECTION("checking network clock reads correctly")
    {
        std::string nameOfClockPortUsedForTest = "/nameOfClockPortUsedForTest";

        // Open port in which we publish the clock
        BufferedPort<Bottle> clockPort;
        CHECK(clockPort.open(nameOfClockPortUsedForTest));

        // Publish 24.0 seconds to the clock
        Bottle& b = clockPort.prepare();
        b.clear();
        b.addInt32(24);
        b.addInt32(0);
        clockPort.write();

        // Open network clock
        yarp::os::NetworkClock netClock;
        CHECK(netClock.open(nameOfClockPortUsedForTest));

        // Check that the network clock returns timePublished, at least after some attempts
        bool clockNowReturnedExpecteValue = false;
        for(size_t i=0; i < 1000; i++)
        {
            // Publish the time timePublished
            b = clockPort.prepare();
            b.clear();
            int32_t timePublished = 42;
            b.addInt32(timePublished);
            b.addInt32(0);
            clockPort.write();

            // Check if it has been received
            if (netClock.now() == static_cast<double>(timePublished))
            {
                clockNowReturnedExpecteValue = true;
                break;
            }
            yarp::os::SystemClock::delaySystem(0.01);
        }

        CHECK(clockNowReturnedExpecteValue);
    }

}
