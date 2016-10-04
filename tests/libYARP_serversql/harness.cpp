/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/UnitTest.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Network.h>
#include <yarp/serversql/yarpserversql.h>

#include "TestList.h"


using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::serversql::impl;


int main(int argc, char *argv[]) {
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

    delete store;
    return result;
}
