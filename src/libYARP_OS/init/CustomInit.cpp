// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Network.h>

// customizable initialization and shutdown functions

#ifdef PLUGIN_INIT_FUNCTION
extern "C" void PLUGIN_INIT_FUNCTION();
#endif

extern "C" void yarpCustomInit() {
#ifdef PLUGIN_INIT_FUNCTION
  PLUGIN_INIT_FUNCTION();
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

