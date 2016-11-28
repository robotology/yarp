/*
 * Copyright: (C) 2016  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
