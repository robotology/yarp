/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/impl/UnitTest.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Network.h>

#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::wire_rep_utils::impl;


#ifdef YARP2_LINUX
#define CHECK_FOR_LEAKS
#endif

#ifdef CHECK_FOR_LEAKS
// this is just for memory leak checking, and only works in linux
#warning "memory leak detection on"
#include <mcheck.h>
#endif


int main(int argc, char *argv[]) {
    //return yarp_test_main(argc,argv);
#ifdef CHECK_FOR_LEAKS
    mtrace();
#endif

    Network yarp;

    bool done = false;
    int result = 0;

    if (argc>1) {
        int verbosity = 0;
        while (ConstString(argv[1])==ConstString("verbose")) {
            verbosity++;
            argc--;
            argv++;
        }
        if (verbosity>0) {
            Logger::get().setVerbosity(verbosity);
        }

        if (ConstString(argv[1])==ConstString("regression")) {
            done = true;
            UnitTest::startTestSystem();
            TestList::collectTests();  // just in case automation doesn't work
            if (argc>2) {
                result = UnitTest::getRoot().run(argc-2,argv+2);
            } else {
                result = UnitTest::getRoot().run();
            }
            UnitTest::stopTestSystem();
        }
    }
    if (!done) {
        Companion::main(argc,argv);
    }

    return result;
}

