/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp.h"
#include "yarpimpl.h"

#include <yarp/os/Bottle.h>

static int __yarpBottleWrite(yarpWriterPtr connection, void *client);
static int __yarpBottleRead(yarpReaderPtr connection, void *client);

static yarpPortableCallbacksPtr __bottleCallbacks() {
    static int init = 0;
    static yarpPortableCallbacks callbacks;
    if (!init) {
        yarpPortableCallbacksInit(&callbacks);
        callbacks.write = __yarpBottleWrite;
        callbacks.read = __yarpBottleRead;
        init = 1;
    }
    return &callbacks;
}

YARP_DEFINE(int) yarpBottleInit(yarpBottlePtr bottle) {
    yarpPortableInit(bottle,__bottleCallbacks());
    bottle->implementation = new Bottle();
    bottle->client = bottle->implementation;
    return (bottle->implementation==NULL);
}


YARP_DEFINE(int) yarpBottleFini(yarpBottlePtr bottle) {
    yarpPortableFini(bottle);
    return 0;
}

YARP_DEFINE(void) yarpBottleAddInt32(yarpBottlePtr bottle, int x) {
    YARP_BOTTLE(bottle).addInt32(x);
}

YARP_DEFINE(void) yarpBottleAddDouble(yarpBottlePtr bottle, double x) {
    YARP_BOTTLE(bottle).addDouble(x);
}

YARP_DEFINE(void) yarpBottleAddString(yarpBottlePtr bottle, const char *x) {
    YARP_BOTTLE(bottle).addString(x);
}

YARP_DEFINE(int) yarpBottleWrite(yarpBottlePtr bottle,
                                 yarpWriterPtr connection) {
    return !YARP_BOTTLE(bottle).write(YARP_WRITER(connection));
}

YARP_DEFINE(int) yarpBottleRead(yarpBottlePtr bottle,
                                yarpReaderPtr connection) {
    return !YARP_BOTTLE(bottle).read(YARP_READER(connection));
}

YARP_DEFINE(int) __yarpBottleWrite(yarpWriterPtr connection, void *client) {
    return !((Bottle *)client)->write(YARP_WRITER(connection));
}

YARP_DEFINE(int) __yarpBottleRead(yarpReaderPtr connection, void *client) {
    return !((Bottle *)client)->read(YARP_READER(connection));
}

YARP_DEFINE(int) yarpBottleToString(yarpBottlePtr bottle,
                                    yarpStringPtr result) {
    YARP_STRING(result) = YARP_BOTTLE(bottle).toString();
    return 0;
}
