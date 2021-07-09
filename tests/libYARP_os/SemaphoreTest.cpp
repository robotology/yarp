/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/os/Semaphore.h>

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>

#include <cmath>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

class SemaphoreTestHelper : public Thread
{
public:
    Semaphore x;
    int state;

    SemaphoreTestHelper() : x(0)
    {
        state = 1;
    }

    virtual void run() override
    {
        x.wait();
        state = 2;
        x.post();
    }
};

TEST_CASE("os::SemaphoreTest", "[yarp::os]")
{
    SECTION("basic semaphore sanity check")
    {
        Semaphore x(0);
        x.post();
        x.post();
        CHECK(x.check()); // pop one
        CHECK(x.check()); // pop two
        CHECK_FALSE(x.check()); // pop one too many
    }

    SECTION("check blocking behavior")
    {
        SemaphoreTestHelper helper;
        helper.start();
        Time::delay(0.5);
        CHECK(helper.state == 1); // helper blocked
        helper.x.post();
        Time::delay(0.5);
        helper.x.wait();
        CHECK(helper.state == 2); // helper unblocked
        helper.x.post();
        helper.stop();
    }

    SECTION("check timed blocking behavior")
    {
        Semaphore x(0);
        bool result = x.waitWithTimeout(0.5);
        CHECK_FALSE(result); // wait timed out ok
        x.post();
        result = x.waitWithTimeout(1000);
        CHECK(result); // wait succeeded
    }
}
