/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/LogStream.h>

#include <yarp/os/impl/UnitTest.h>



class LogStreamTest : public yarp::os::impl::UnitTest {
public:
    virtual yarp::os::ConstString getName() override { return "LogStreamTest"; }

    void checkLogStream() {
        int i = 13;
        std::vector<int> v(4);
        v[0] = 1;
        v[1] = 2;
        v[2] = 3;
        v[3] = 4;

        yTrace("This is a trace");
        yTrace("This is %s (%d)", "a trace", i);
        yTrace();
        yTrace() << "This is" << "another" << "trace" << i;
        yTrace() << v;

        yDebug("This is a debug");
        yDebug("This is %s (%d)", "a debug", i);
        yDebug();
        yDebug() << "This is" << "another" << "debug" << i;
        yDebug() << v;

        yInfo("This is info");
        yInfo("This is %s (%d)", "info", i);
        yInfo();
        yInfo() << "This is" << "more" << "info" << i;
        yInfo() << v;

        yWarning("This is a warning");
        yWarning("This is %s (%d)", "a warning", i);
        yWarning();
        yWarning() << "This is" << "another" << "warning" << i;
        yWarning() << v;

        yError("This is an error");
        yError("This is %s (%d)", "an error", i);
        yError();
        yError() << "This is" << "another" << "error" << i;
        yError() << v;
    }

    virtual void runTests() override {
        checkLogStream();
    }
};

static LogStreamTest theLogStreamTest;

yarp::os::impl::UnitTest& getLogStreamTest() {
    return theLogStreamTest;
}
