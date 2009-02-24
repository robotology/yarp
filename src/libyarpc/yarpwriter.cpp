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
     * Write an integer to a connection.
     *
     */
YARP_DEFINE(int) yarpWriterAppendInt(yarpWriterPtr writer, int data) {
    YARP_OK(writer);
    YARP_WRITER(writer).appendInt(data);
    return YARP_WRITER(writer).isError()?-1:0;
}

