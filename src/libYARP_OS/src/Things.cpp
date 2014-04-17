// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
    conReader = NULL;
    writer = NULL;
    portable = NULL;
}

Things::~Things() {
    if (portable) delete portable;
    portable = NULL;
}
