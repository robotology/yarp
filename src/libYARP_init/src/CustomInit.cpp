/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
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

extern "C" int __yarp_is_initialized;

extern "C" void yarpCustomFini() {
}

yarp::os::Network::Network() {
    Network::init();
}

yarp::os::Network::~Network() {
    Network::fini();
}

void yarp::os::Network::init() {
    if (__custom_yarp_is_initialized==0) {
        initMinimum();
        yarpCustomInit();
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

