/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_TESTS_WIRE_REP_UTILS_TESTLIST_H
#define YARP_TESTS_WIRE_REP_UTILS_TESTLIST_H

#include <yarp/os/impl/UnitTest.h>


extern yarp::os::impl::UnitTest& getWireTest();


namespace yarp {
namespace wire_rep_utils {
namespace impl {

class TestList {
public:
    static void collectTests() {
        yarp::os::impl::UnitTest& root = yarp::os::impl::UnitTest::getRoot();
        root.add(getWireTest());
    }
};

} // namespace impl
} // namespace wire_rep_utils
} // namespace yarp


#endif // YARP_TESTS_WIRE_REP_UTILS_TESTLIST_H
