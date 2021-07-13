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
     * Read an integer from a connection.
     *
     */
YARP_DEFINE(int) yarpReaderExpectInt32(yarpReaderPtr reader, int *data) {
    YARP_OK(reader);
    int x = YARP_READER(reader).expectInt32();
    if (data!=NULL) {
        *data = x;
    }
    return YARP_READER(reader).isError()?-1:0;
}

    /**
     *
     * Read a floating point number from a connection.
     *
     */
YARP_DEFINE(int) yarpReaderExpectDouble(yarpReaderPtr reader, double *data) {
    YARP_OK(reader);
    double x = YARP_READER(reader).expectFloat64();
    if (data!=NULL) {
        *data = x;
    }
    return YARP_READER(reader).isError()?-1:0;
}


    /**
     *
     * Read text from a connection.
     *
     */
YARP_DEFINE(int) yarpReaderExpectText(yarpReaderPtr reader,
                                      yarpStringPtr str,
                                      char terminal) {
    YARP_OK(reader);
    YARP_STRING(str) = YARP_READER(reader).expectText(terminal);
    return YARP_READER(reader).isError()?-1:0;
}

    /**
     *
     * Read a block of data from a connection.
     *
     */
YARP_DEFINE(int) yarpReaderExpectBlock(yarpReaderPtr reader,
                                       char *data,
                                       int len) {
    YARP_OK(reader);
    return !YARP_READER(reader).expectBlock(data,len);
}


    /**
     *
     * Check if connection is text-mode
     *
     */
YARP_DEFINE(int) yarpReaderIsTextMode(yarpReaderPtr reader) {
    YARP_OK(reader);
    return YARP_READER(reader).isTextMode();
}
