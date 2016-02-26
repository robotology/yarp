/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

