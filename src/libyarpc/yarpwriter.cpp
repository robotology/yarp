/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

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

