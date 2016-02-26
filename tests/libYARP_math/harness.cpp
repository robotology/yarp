/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/UnitTest.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Companion.h>

#include "TestList.h"
//
using namespace yarp::os::impl;

int main(int argc, char *argv[]) {
    yarp::os::Network yarp;

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
        }
    } 
    if (!done) {
        Companion::main(argc,argv);
    }

    return result;
}
