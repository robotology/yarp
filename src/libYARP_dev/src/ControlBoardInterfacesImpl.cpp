// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"

#include <stdio.h>
using namespace yarp::dev;

bool StubImplPositionControlRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        fprintf(stderr, "%s:", func);
    else
        fprintf(stderr, "Function ");

    fprintf(stderr, "not yet implemented\n");

    return false;
}

bool StubImplEncodersRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        fprintf(stderr, "%s:", func);
    else
        fprintf(stderr, "Function ");

    fprintf(stderr, "not yet implemented\n");

    return false;
}

bool StubImplPidControlRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func)
        fprintf(stderr, "%s:", func);
    else
        fprintf(stderr, "Function ");

    fprintf(stderr, "not yet implemented\n");

    return false;
}
