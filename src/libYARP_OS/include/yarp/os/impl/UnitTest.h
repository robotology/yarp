/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_UNITTEST
#define YARP2_UNITTEST

#include <yarp/os/impl/String.h>
#include <yarp/os/Bottle.h>

#include <yarp/os/impl/PlatformVector.h>

namespace yarp {
    namespace os {
        namespace impl {
            class UnitTest;
        }
    }
}

/**
 * Simple unit testing framework.  There are libraries out there for
 * this, but we don't want to add another dependency to YARP.
 */
class YARP_OS_impl_API yarp::os::impl::UnitTest {
public:
    UnitTest();

    UnitTest(UnitTest *parent);

    virtual ~UnitTest() {
        clear();
    }

    void report(int severity, const String& problem);

    virtual String getName() { return "isolated test"; }

    static void startTestSystem();
    static UnitTest& getRoot();
    static void stopTestSystem();

    void add(UnitTest& unit);
    void clear();

    virtual int run();

    virtual int run(int argc, char *argv[]);

    virtual void runTests() {
    }

    virtual void runSubTests(int argc, char *argv[]);


    bool checkEqualImpl(int x, int y, 
                        const char *desc,
                        const char *txt1,
                        const char *txt2,
                        const char *fname,
                        int fline);

    bool checkEqualishImpl(double x, double y, 
                           const char *desc,
                           const char *txt1,
                           const char *txt2,
                           const char *fname,
                           int fline);

    bool checkEqualImpl(const String& x, const String& y,
                        const char *desc,
                        const char *txt1,
                        const char *txt2,
                        const char *fname,
                        int fline);

    String humanize(const String& txt);

    void saveEnvironment(const char *key);
    void restoreEnvironment();

    bool isOk() {
        return !hasProblem;
    }

    /**
     *
     * Check if YARP has been compiled with YARP_TEST_HEAP set, and so
     * can monitor heap activity.
     *
     * @return true if YARP is compiled with hooks for monitoring heap
     * activity
     *
     */
    static bool heapMonitorSupported();

    /**
     *
     * Begin monitoring heap activity, specifically calls to
     * new/new[]/delete/delete[]. It is important to eventually call
     * heapMonitorEnd() to stop monitoring heap activity.  You must
     * guarantee that no heap activity is occurring in any other
     * threads when you call this method.
     *
     * @param expectAllocations set to true if heap activity is expected,
     * set to false if heap activity is unexpected (stack traces will be
     * shown for every new/delete in this case)
     *
     */
    void heapMonitorBegin(bool expectAllocations = true);

    /**
     *
     * @return a cumulative count of new/delete calls since the last call
     * to this method or to heapMonitorBegin()
     *
     */
    int heapMonitorOps();

    /**
     *
     * Stop monitoring heap activity.  You must guarantee that no
     * heap activity is occurring in any other threads when you call
     * this method.
     *
     * @return the output of heapMonitorOps()
     *
     */
    int heapMonitorEnd();

private:
    UnitTest *parent;
    PlatformVector<UnitTest *> subTests;
    bool hasProblem;
    yarp::os::Bottle env;
    static UnitTest *theRoot;

    void count(int severity);
};

// add info 

#define checkEqual(x,y,desc) checkEqualImpl(x,y,desc,#x,#y,__FILE__,__LINE__)
#define checkEqualish(x,y,desc) checkEqualishImpl(x,y,desc,#x,#y,__FILE__,__LINE__)
#define checkTrue(x,desc) checkEqualImpl((x)?true:false,true,desc,#x,"true",__FILE__,__LINE__)
#define checkFalse(x,desc) checkEqualImpl((x)?true:false,false,desc,#x,"false",__FILE__,__LINE__)


#endif
