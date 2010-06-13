// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

extern "C" int __yarp_is_initialized;

extern "C" void yarpCustomFini() {
}



void yarp::os::Network::init() {
    if (__yarp_is_initialized==0) {
        initMinimum();
        yarpCustomInit();
    }
    __yarp_is_initialized++;
}


void yarp::os::Network::fini() {
    if (__yarp_is_initialized>0) {
        yarpCustomFini();
        finiMinimum();
        __yarp_is_initialized--;
    }
}

