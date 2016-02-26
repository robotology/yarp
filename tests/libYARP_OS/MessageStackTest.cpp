/*
* Copyright (C) 2014 iCub Facility
* Authors: Paul Fitzpatrick
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
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

    virtual bool read(ConnectionReader& reader) {
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

    virtual void runTests() {
        checkBasic();
    }

    virtual String getName() {
        return "MessageStackTest";
    }
};

static MessageStackTest theMessageStackTest;

UnitTest& getMessageStackTest() {
    return theMessageStackTest;
}

