/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/MessageStack.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class MessageStackWorker : public PortReader {
public:
    Semaphore go;
    Semaphore gone;
    Semaphore mutex;
    Bottle last;

    MessageStackWorker() : go(0), gone(0), mutex(1) {
    }

    virtual bool read(ConnectionReader& reader) override {
        go.wait();
        mutex.wait();
        bool ok = last.read(reader);
        mutex.post();
        gone.post();
        return ok;
    }
};

class MessageStackTest : public UnitTest {
public:

    void checkBasic() {
        report(0,"check basics of stack operation");
        MessageStack stack(2);
        MessageStackWorker worker;
        stack.attach(worker);
        Bottle b;
        b.fromString("hello");
        stack.stack(b);
        stack.stack(b);
        Time::delay(1);
        checkEqual(worker.last.size(),0,"no read without permission");
        worker.go.post();
        worker.gone.wait();
        checkEqual(worker.last.toString(),"hello","got a message");
        worker.last.clear();
        worker.go.post();
        worker.gone.wait();
        checkEqual(worker.last.toString(),"hello","got another message");

        b.fromString("world");
        stack.stack(b,"the");
        worker.go.post();
        worker.gone.wait();
        checkEqual(worker.last.toString(),"the world","got a prefixed message");
    }

    virtual void runTests() override {
        checkBasic();
    }

    virtual ConstString getName() override {
        return "MessageStackTest";
    }
};

static MessageStackTest theMessageStackTest;

UnitTest& getMessageStackTest() {
    return theMessageStackTest;
}

