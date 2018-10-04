/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Log.h>

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif

TEST_CASE("OS::LogTest", "[yarp::os]") {
    int i = 42;

    SECTION("Test yTrace") {
        yTrace("This is a trace");
        yTrace("This is %s (%d)", "a trace", i);
    }

    SECTION("Test yDebug") {
        yDebug("This is a debug");
        yDebug("This is %s (%d)", "a debug", i);
    }

    SECTION("Test yInfo") {
        yInfo("This is info");
        yInfo("This is %s (%d)", "info", i);
    }

    SECTION("Test yWarning") {
        yWarning("This is a warning");
        yWarning("This is %s (%d)", "a warning", i);
    }

    SECTION("Test yError") {
        yError("This is an error");
        yError("This is %s (%d)", "an error", i);
    }

    SECTION("Other log tests") {
        yInfo("This is a very long long sentence. The purpose is to check if the logger is able to handle and display nicely very long messages, eventually displaying them on multiple lines or using other mechanisms (e.g. pop-up, user hints etc)");

        yInfo("This is a sentence that spawn on multiple lines:\n.This is line 2.\nThis is line 3.\nThis is the last line.");

        yInfo("\nThis is text contains an end of line at the beginning of the sentence");

        yInfo("This is text contains an end of line at the end of the sentence\n");

    }
}
