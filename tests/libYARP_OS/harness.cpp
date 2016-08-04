/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/UnitTest.h>

#include <yarp/os/Network.h>

#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;


#ifdef YARP2_LINUX
#define CHECK_FOR_LEAKS
#endif

#ifdef CHECK_FOR_LEAKS
// this is just for memory leak checking, and only works in linux
#warning "memory leak detection on"
#include <mcheck.h>
#endif

#include <yarp/serversql/yarpserversql.h>

int main(int argc, char *argv[]) {
    //return yarp_test_main(argc,argv);
#ifdef CHECK_FOR_LEAKS
    mtrace();
#endif

    Network yarp;

    Property opts;
    opts.put("portdb",":memory:");
    opts.put("subdb",":memory:");
    opts.put("local",1);
    NameStore *store = yarpserver_create(opts);
    yarp.queryBypass(store);

    bool done = false;
    int result = 0;

    if (argc>1) {
        int verbosity = 0;
        while (ConstString(argv[1])=="verbose") {
            verbosity++;
            argc--;
            argv++;
        }
        if (verbosity>0) {
            Network::setVerbosity(verbosity);
        }

        if (ConstString(argv[1])=="regression") {
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
        Network::main(argc,argv);
    }

    yarp.queryBypass(NULL);
    if (store) delete store;

    return result;
}

