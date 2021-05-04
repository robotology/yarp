/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_TESTS_HARNESS_H
#define YARP_TESTS_HARNESS_H

#if !defined(WITHOUT_NETWORK)
#  include <yarp/os/YarpPluginSelector.h>
#endif // WITHOUT_NETWORK

#include <iostream>
#include <catch.hpp>

extern int yarp_tests_skipped;

#define YARP_SKIP_TEST(...) \
{ \
    ++yarp_tests_skipped; \
    FAIL(__VA_ARGS__); \
}

#if !defined(WITHOUT_NETWORK)
#define YARP_REQUIRE_PLUGIN(name, type) \
{ \
    bool has_plugin = yarp::os::YarpPluginSelector::checkPlugin(name, type); \
    if (!has_plugin) { \
        YARP_SKIP_TEST("Required plugin is missing: " << type << " - " << name); \
    } \
}
#endif // WITHOUT_NETWORK

#endif // YARP_TESTS_HARNESS_H
