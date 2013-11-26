// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/idl/Unwrapped.h>
#include <yarp/os/idl/WireState.h>

using namespace yarp::os::idl;

WireState::WireState() {
    len = -1;
    code = -1;
    need_ok = false;
    parent = 0 /*NULL*/;
}

bool WireState::isValid() const {
    return len>=0;
}
