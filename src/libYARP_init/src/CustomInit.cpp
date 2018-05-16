/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

static int __custom_yarp_is_initialized = 0;

// customizable initialization and shutdown functions

#ifdef PLUGIN_INIT_FUNCTION
extern "C" YARP_IMPORT void PLUGIN_INIT_FUNCTION();
#endif

#ifdef PLUGIN_INIT_FUNCTION2
extern "C" YARP_IMPORT void PLUGIN_INIT_FUNCTION2();
#endif

extern "C" void yarpCustomInit() {
#ifdef PLUGIN_INIT_FUNCTION
  PLUGIN_INIT_FUNCTION();
#endif
#ifdef PLUGIN_INIT_FUNCTION2
  PLUGIN_INIT_FUNCTION2();
#endif
}

#include <yarp/os/impl/LogForwarder.h>

extern "C" int __yarp_is_initialized;

extern "C" void yarpCustomFini()
{
}

yarp::os::Network::Network() {
    Network::init();
}

yarp::os::Network::Network(yarp::os::yarpClockType clockType, yarp::os::Clock *custom) {
    init(clockType, custom);
}

yarp::os::Network::~Network() {
    Network::fini();
}

void yarp::os::Network::init() {
    init(yarp::os::YARP_CLOCK_DEFAULT);
}


void yarp::os::Network::init(yarp::os::yarpClockType clockType, yarp::os::Clock *custom) {
    if (__custom_yarp_is_initialized==0) {
        initMinimum(clockType);
        // If we init the clock inside the initMinum, it will loop into itself twice
        // calling again the initMinimun, ending with __yarp_is_initialized counter
        // increased twice.
        yarpCustomInit();
        // Network::yarpClockInit(clockType, custom);
    }
    __custom_yarp_is_initialized++;
}


void yarp::os::Network::fini() {
    if (__custom_yarp_is_initialized==1) {
        yarpCustomFini();
        finiMinimum();
    }
    if (__custom_yarp_is_initialized>0) __custom_yarp_is_initialized--;
}
