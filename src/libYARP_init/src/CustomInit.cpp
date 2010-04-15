// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>
#include <yarp/os/Network.h>

// customizable initialization and shutdown functions

#ifdef PLUGIN_INIT_FUNCTION
extern "C" void PLUGIN_INIT_FUNCTION();
#endif

#ifdef PLUGIN_INIT_FUNCTION2
extern "C" void PLUGIN_INIT_FUNCTION2();
#endif

extern "C" void yarpCustomInit() {
#ifdef PLUGIN_INIT_FUNCTION
  PLUGIN_INIT_FUNCTION();
#endif
#ifdef PLUGIN_INIT_FUNCTION2
  PLUGIN_INIT_FUNCTION2();
#endif
}

extern "C" void yarpCustomFini() {
}



void yarp::os::Network::init() {
    initMinimum();
    yarpCustomInit();
}


void yarp::os::Network::fini() {
    yarpCustomFini();
    finiMinimum();
}

