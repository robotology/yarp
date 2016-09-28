/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Things.h>
#include <stdio.h>

using namespace yarp::os;

Things::Things() { 
    conReader = YARP_NULLPTR;
    writer = YARP_NULLPTR;
    reader = YARP_NULLPTR;
    portable = YARP_NULLPTR;
    beenRead = false;
}

Things::~Things() {
    if (portable) delete portable;
    portable = YARP_NULLPTR;
}
