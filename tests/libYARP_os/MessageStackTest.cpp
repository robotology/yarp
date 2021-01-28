/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/MessageStack.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <mutex>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

class MessageStackWorker : public PortReader
{
public:
    Semaphore go;
    Semaphore gone;
    std::mutex mutex;
    Bottle last;

    MessageStackWorker() :
            go(0),
            gone(0),
            mutex()
    {
    }

    virtual bool read(ConnectionReader& reader) override {
        go.wait();
        mutex.lock();
        bool ok = last.read(reader);
        mutex.unlock();
        gone.post();
        return ok;
    }
};

TEST_CASE("os::MessageStackTest", "[yarp::os]")
{
    SECTION("check basics of stack operation")
    {
        MessageStack stack(2);
        MessageStackWorker worker;
        stack.attach(worker);
        Bottle b;
        b.fromString("hello");
        stack.stack(b);
        stack.stack(b);
        Time::delay(1);
        CHECK(worker.last.size() == (size_t) 0); // no read without permission
        worker.go.post();
        worker.gone.wait();
        CHECK(worker.last.toString() == "hello"); // got a message
        worker.last.clear();
        worker.go.post();
        worker.gone.wait();
        CHECK(worker.last.toString() == "hello"); // got another message

        b.fromString("world");
        stack.stack(b, "the");
        worker.go.post();
        worker.gone.wait();
        CHECK(worker.last.toString() == "the world"); // got a prefixed message
    }
}
