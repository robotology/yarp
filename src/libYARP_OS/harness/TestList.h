// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_TESTLIST_
#define _YARP2_TESTLIST_

#include <yarp/UnitTest.h>

namespace yarp {
    class TestList;
}

// need to made one function for each new test, and add to collectTests()
// method
extern yarp::UnitTest& getBottleTest();
extern yarp::UnitTest& getStringTest();
extern yarp::UnitTest& getAddressTest();
extern yarp::UnitTest& getStringInputStreamTest();
extern yarp::UnitTest& getTimeTest();
extern yarp::UnitTest& getThreadTest();
extern yarp::UnitTest& getPortCommandTest();
extern yarp::UnitTest& getStringOutputStreamTest();
extern yarp::UnitTest& getStreamConnectionReaderTest();
extern yarp::UnitTest& getBufferedConnectionWriterTest();
extern yarp::UnitTest& getProtocolTest();
extern yarp::UnitTest& getNameServerTest();
extern yarp::UnitTest& getPortCoreTest();
extern yarp::UnitTest& getElectionTest();
extern yarp::UnitTest& getNameConfigTest();
extern yarp::UnitTest& getPortTest();
extern yarp::UnitTest& getNetTypeTest();
extern yarp::UnitTest& getBinPortableTest();
extern yarp::UnitTest& getPropertyTest();
extern yarp::UnitTest& getVocabTest();
extern yarp::UnitTest& getValueTest();
extern yarp::UnitTest& getPortablePairTest();
extern yarp::UnitTest& getTerminatorTest();
extern yarp::UnitTest& getRateThreadTest();
extern yarp::UnitTest& getStampTest();
extern yarp::UnitTest& getModuleTest();

class yarp::TestList {
public:
    static void collectTests() {
        UnitTest& root = UnitTest::getRoot();
        root.add(getBottleTest());
        root.add(getStringTest());
        root.add(getAddressTest());
        root.add(getStringInputStreamTest());
        root.add(getTimeTest());
        root.add(getPortCommandTest());
        root.add(getStringOutputStreamTest());
        root.add(getStreamConnectionReaderTest());
        root.add(getBufferedConnectionWriterTest());
        root.add(getThreadTest());
        root.add(getRateThreadTest());
        root.add(getProtocolTest());
        root.add(getNameServerTest());
        root.add(getPortCoreTest());
        root.add(getElectionTest());
        root.add(getNameConfigTest());
        root.add(getPortTest());
        root.add(getNetTypeTest());
        root.add(getBinPortableTest());
        root.add(getPropertyTest());
        root.add(getVocabTest());
        root.add(getValueTest());
        root.add(getPortablePairTest());
        root.add(getTerminatorTest());
        root.add(getStampTest());
        root.add(getModuleTest());
    }
};

#endif

