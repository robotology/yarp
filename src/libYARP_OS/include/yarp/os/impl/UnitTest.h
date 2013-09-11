// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_UNITTEST_
#define _YARP2_UNITTEST_

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

    virtual String getName() = 0;

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
