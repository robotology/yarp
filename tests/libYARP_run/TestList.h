/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_RUN_TESTLIST_H
#define YARP_RUN_TESTLIST_H

#include <yarp/os/impl/UnitTest.h>

using yarp::os::impl::UnitTest;

namespace yarp {
namespace run {
namespace impl {

extern UnitTest& getRunTest();

class TestList
{
public:
    static void collectTests() {
#ifdef BROKEN_TEST
        UnitTest& root = UnitTest::getRoot();
        root.add(getRunTest());
#endif
    }
};

} // namespace impl
} // namespace run
} // namespace yarp


#endif // YARP_RUN_TESTLIST_H
