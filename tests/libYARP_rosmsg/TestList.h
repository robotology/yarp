/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef HARNESS_ROSMSG_TESTLIST_H
#define HARNESS_ROSMSG_TESTLIST_H

#include <yarp/os/impl/UnitTest.h>
#include <iostream>


extern yarp::os::impl::UnitTest& getROSPropertiesTest();

class TestList
{
public:
    static void collectTests()
    {
        yarp::os::impl::UnitTest& root = yarp::os::impl::UnitTest::getRoot();
        root.add(getROSPropertiesTest());
    }
};

#endif // HARNESS_ROSMSG_TESTLIST_H
