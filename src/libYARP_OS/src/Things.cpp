/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Authors: Ali Paikan and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Things.h>
#include <cstdio>

using namespace yarp::os;

Things::Things() {
    conReader = nullptr;
    writer = nullptr;
    reader = nullptr;
    portable = nullptr;
    beenRead = false;
}

Things::~Things() {
    if (portable) delete portable;
    portable = nullptr;
}
