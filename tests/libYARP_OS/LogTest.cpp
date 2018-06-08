/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Log.h>

#include <yarp/os/impl/UnitTest.h>


class LogTest : public yarp::os::impl::UnitTest {
public:
    virtual std::string getName() const override { return "LogTest"; }

    void checkLog() {
        int i = 13;

        yTrace("This is a trace");
        yTrace("This is %s (%d)", "a trace", i);

        yDebug("This is a debug");
        yDebug("This is %s (%d)", "a debug", i);

        yInfo("This is info");
        yInfo("This is %s (%d)", "info", i);

        yWarning("This is a warning");
        yWarning("This is %s (%d)", "a warning", i);

        yError("This is an error");
        yError("This is %s (%d)", "an error", i);

        yInfo("This is a very long long sentence. The purpose is to check if the logger is able to handle and display nicely very long messages, eventually displaying them on multiple lines or using other mechanisms (e.g. pop-up, user hints etc)");

        yInfo("This is a sentence that spawn on multiple lines:\n.This is line 2.\nThis is line 3.\nThis is the last line.");

        yInfo("\nThis is text contains an end of line at the beginning of the sentence");

        yInfo("This is text contains an end of line at the end of the sentence\n");

    }

    virtual void runTests() override {
        checkLog();
    }
};

static LogTest theLogTest;

yarp::os::impl::UnitTest& getLogTest() {
    return theLogTest;
}
