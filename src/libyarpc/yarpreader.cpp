// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include "yarp.h"
#include "yarpimpl.h"

    /**
     *
     * Read an integer from a connection.
     *
     */
YARP_DEFINE(int) yarpReaderExpectInt(yarpReaderPtr reader, int *data) {
    YARP_OK(reader);
    int x = YARP_READER(reader).expectInt();
    if (data!=NULL) {
        *data = x;
    }
    return YARP_READER(reader).isError()?-1:0;
}


