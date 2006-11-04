// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/NetInt32.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>

using namespace yarp::os;
using namespace yarp::dev;


#ifdef YARP2_LINUX
#define CHECK_FOR_LEAKS
#endif

#ifdef CHECK_FOR_LEAKS
// this is just for memory leak checking, and only works in linux
#warning "memory leak detection on"
#include <mcheck.h>
#endif


int main(int argc, char *argv[]) {

#ifdef CHECK_FOR_LEAKS
    mtrace();
#endif

    int result = 0;

    Network::init();

    Property p;
    p.fromCommand(argc,argv);

    if (p.check("file")) {
        p.fromConfigFile(p.check("file",Value("default.ini")).asString());
    }


    PolyDriver dd;
    bool ok = dd.open(p);
    result = ok?0:1;
    if (ok) {
        dd.close();
    }

    Network::fini();

    return result;
}

