/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include "yarp.h"
#include "yarpimpl.h"

    /**
     *
     * Write an integer to a connection.
     *
     */
YARP_DEFINE(int) yarpWriterAppendInt32(yarpWriterPtr writer, int data) {
    YARP_OK(writer);
    YARP_WRITER(writer).appendInt32(data);
    return YARP_WRITER(writer).isError()?-1:0;
}
