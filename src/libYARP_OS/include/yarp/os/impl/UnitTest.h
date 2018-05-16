/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_UNITTEST_H
#define YARP_OS_IMPL_UNITTEST_H

#include <string>
#include <yarp/os/Bottle.h>

#include <vector>

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
class YARP_OS_impl_API yarp::os::impl::UnitTest
{
public:
    UnitTest();

    UnitTest(UnitTest *parent);

    virtual ~UnitTest()
    {
        clear();
    }

    void report(int severity, const std::string& problem);

    virtual std::string getName() { return "isolated test"; }

    static void startTestSystem();
    static UnitTest& getRoot();
    static void stopTestSystem();

    void add(UnitTest& unit);
    void clear();

    virtual int run();

    virtual int run(int argc, char *argv[]);

    virtual void runTests()
    {
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

    bool checkEqualImpl(const std::string& x, const std::string& y,
                        const char *desc,
                        const char *txt1,
                        const char *txt2,
                        const char *fname,
                        int fline);

    std::string humanize(const std::string& txt);

    void saveEnvironment(const char *key);
    void restoreEnvironment();

    bool isOk()
    {
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
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<UnitTest*>) subTests;
    bool hasProblem;
    yarp::os::Bottle env;
    static UnitTest *theRoot;

    void count(int severity);
};

// add info

#define checkEqual(x, y, desc) checkEqualImpl(x, y, desc, #x, #y, __FILE__, __LINE__)
#define checkEqualish(x, y, desc) checkEqualishImpl(x, y, desc, #x, #y, __FILE__, __LINE__)
#define checkTrue(x, desc) checkEqualImpl((x)?true:false, true, desc, #x, "true", __FILE__, __LINE__)
#define checkFalse(x, desc) checkEqualImpl((x)?true:false, false, desc, #x, "false", __FILE__, __LINE__)


#endif // YARP_OS_IMPL_UNITTEST_H
