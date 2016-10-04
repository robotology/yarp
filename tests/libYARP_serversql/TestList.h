/*
 * Copyright: (C) 2016  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_TESTS_SERVERSQL_TESTLIST_H
#define YARP_TESTS_SERVERSQL_TESTLIST_H

#include <yarp/os/impl/UnitTest.h>


extern yarp::os::impl::UnitTest& getServerTest();


namespace yarp {
namespace serversql {
namespace impl {

class TestList {
public:
    static void collectTests() {
        yarp::os::impl::UnitTest& root = yarp::os::impl::UnitTest::getRoot();
        root.add(getServerTest());
    }
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_TESTS_SERVERSQL_TESTLIST_H
