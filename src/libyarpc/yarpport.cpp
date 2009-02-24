// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008, 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include "yarp.h"
#include "yarpimpl.h"


YARP_DEFINE(yarpPortPtr) yarpPortCreate(yarpNetworkPtr network) {
    yarpPortPtr port = new yarpPort;
    if (port!=NULL) {
        port->implementation = new Port();
        if (port->implementation==NULL) {
            delete port;
            port = NULL;
        }
    }
    return port;
}


YARP_DEFINE(yarpPortPtr) yarpPortCreateOpen(yarpNetworkPtr network,
                                            const char *name) {
    yarpPortPtr port = yarpPortCreate(network);
    if (port==NULL) return NULL;
    int result = yarpPortOpen(port,name);
    if (result<0) {
        delete port;
        port = NULL;
        return NULL;
    }
    return port;
}


YARP_DEFINE(void) yarpPortFree(yarpPortPtr port) {
    if (port!=NULL) {
        if (port->implementation!=NULL) {
            delete (Port*)(port->implementation);
            port->implementation = NULL;
        }
        delete port;
    }
}

YARP_DEFINE(int) yarpPortOpen(yarpPortPtr port, const char *name) {
    YARP_OK(port);
    bool ok = YARP_PORT(port).open(name);
    return ok?0:-1;
}

YARP_DEFINE(int) yarpPortOpenEx(yarpPortPtr port, yarpContactPtr contact) {
    YARP_OK(port);
    YARP_OK(contact);
    bool ok = YARP_PORT(port).open(YARP_CONTACT(contact));
    return ok?0:-1;
}

YARP_DEFINE(int) yarpPortClose(yarpPortPtr port) {
    YARP_OK(port);
    YARP_PORT(port).close();
    return 0;
}

YARP_DEFINE(int) yarpPortEnableBackgroundWrite(yarpPortPtr port,
                                               int writeInBackgroundFlag) {
    YARP_OK(port);
    YARP_PORT(port).enableBackgroundWrite(writeInBackgroundFlag);
    return 0;
}

YARP_DEFINE(int) yarpPortWrite(yarpPortPtr port, 
                               yarpPortablePtr msg) {
    YARP_OK(port);
    YARP_OK(msg);
    return YARP_PORT(port).write(YARP_PORTABLE(msg))?0:-1;
}


YARP_DEFINE(int) yarpPortRead(yarpPortPtr port, 
                              yarpPortablePtr msg,
                              int willReply) {
    YARP_OK(port);
    YARP_OK(msg);
    return YARP_PORT(port).read(YARP_PORTABLE(msg),willReply)?0:-1;
}


YARP_DEFINE(int) yarpPortReply(yarpPortPtr port, 
                               yarpPortablePtr msg) {
    YARP_OK(port);
    YARP_OK(msg);
    return YARP_PORT(port).reply(YARP_PORTABLE(msg))?0:-1;
}


YARP_DEFINE(int) yarpPortWriteWithReply(yarpPortPtr port, 
                                        yarpPortablePtr msg,
                                        yarpPortablePtr reply) {
    YARP_OK(port);
    return YARP_PORT(port).write(YARP_PORTABLE(msg),YARP_PORTABLE(reply))?0:-1;
}


