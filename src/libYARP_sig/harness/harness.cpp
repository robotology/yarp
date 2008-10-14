// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/UnitTest.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/NetInt32.h>

#include "TestList.h"

using namespace yarp::os::impl;


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

    ACE::init();

    bool done = false;
    int result = 0;

    if (argc>1) {
        int verbosity = 0;
        while (String(argv[1])==String("verbose")) {
            verbosity++;
            argc--;
            argv++;
        }
        if (verbosity>0) {
            Logger::get().setVerbosity(verbosity);
        }
    
        if (String(argv[1])==String("regression")) {
            done = true;
            UnitTest::startTestSystem();
            TestList::collectTests();  // just in case automation doesn't work
            if (argc>2) {
                result = UnitTest::getRoot().run(argc-2,argv+2);
            } else {
                result = UnitTest::getRoot().run();
            }
            UnitTest::stopTestSystem();
            NameClient::removeNameClient();
        }
    } 
    if (!done) {
        Companion::main(argc,argv);
    }
    ACE::fini();

    return result;
}

