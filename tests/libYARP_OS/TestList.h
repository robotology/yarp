/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_TESTLIST_
#define _YARP2_TESTLIST_

#include <yarp/os/impl/UnitTest.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TestList;
        }
    }
}

// need to made one function for each new test, and add to collectTests()
// method
extern yarp::os::impl::UnitTest& getBottleTest();
extern yarp::os::impl::UnitTest& getStringTest();
extern yarp::os::impl::UnitTest& getAddressTest();
extern yarp::os::impl::UnitTest& getStringInputStreamTest();
extern yarp::os::impl::UnitTest& getTimeTest();
extern yarp::os::impl::UnitTest& getThreadTest();
extern yarp::os::impl::UnitTest& getPortCommandTest();
extern yarp::os::impl::UnitTest& getStringOutputStreamTest();
extern yarp::os::impl::UnitTest& getStreamConnectionReaderTest();
extern yarp::os::impl::UnitTest& getBufferedConnectionWriterTest();
extern yarp::os::impl::UnitTest& getProtocolTest();
extern yarp::os::impl::UnitTest& getNameServerTest();
extern yarp::os::impl::UnitTest& getPortCoreTest();
extern yarp::os::impl::UnitTest& getElectionTest();
extern yarp::os::impl::UnitTest& getNameConfigTest();
extern yarp::os::impl::UnitTest& getPortTest();
extern yarp::os::impl::UnitTest& getNetTypeTest();
extern yarp::os::impl::UnitTest& getBinPortableTest();
extern yarp::os::impl::UnitTest& getPropertyTest();
extern yarp::os::impl::UnitTest& getVocabTest();
extern yarp::os::impl::UnitTest& getValueTest();
extern yarp::os::impl::UnitTest& getPortablePairTest();
extern yarp::os::impl::UnitTest& getTerminatorTest();
extern yarp::os::impl::UnitTest& getRateThreadTest();
extern yarp::os::impl::UnitTest& getStampTest();
extern yarp::os::impl::UnitTest& getRFModuleTest();
extern yarp::os::impl::UnitTest& getPortReaderBufferTest();
extern yarp::os::impl::UnitTest& getNetworkTest();
extern yarp::os::impl::UnitTest& getResourceFinderTest();
extern yarp::os::impl::UnitTest& getDgramTwoWayStreamTest();
extern yarp::os::impl::UnitTest& getSemaphoreTest();
extern yarp::os::impl::UnitTest& getEventTest();
extern yarp::os::impl::UnitTest& getRunTest();
extern yarp::os::impl::UnitTest& getNodeTest();
extern yarp::os::impl::UnitTest& getPublisherTest();
extern yarp::os::impl::UnitTest& getLogTest();
extern yarp::os::impl::UnitTest& getLogStreamTest();
extern yarp::os::impl::UnitTest& getMessageStackTest();
extern yarp::os::impl::UnitTest& getUnitTestTest();

class yarp::os::impl::TestList {
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
        root.add(getRFModuleTest());
        root.add(getPortReaderBufferTest());
        root.add(getNetworkTest());
        root.add(getResourceFinderTest());
        root.add(getDgramTwoWayStreamTest());
        root.add(getSemaphoreTest());
        root.add(getEventTest());
        root.add(getNodeTest());
        root.add(getPublisherTest());
        root.add(getLogTest());
        root.add(getLogStreamTest());
        root.add(getMessageStackTest());
        root.add(getUnitTestTest());

#ifdef YARPRUN_TEST
        root.add(getRunTest());
#endif

    }
};

#endif

