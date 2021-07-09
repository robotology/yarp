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
     * Create a port.  Ports start in a dormant, inactive state.
     *
     */
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


    /**
     *
     * Create and open a port.  If the port fails to open, NULL will
     * be returned.
     *
     */
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


    /**
     *
     * Destroy a port.
     *
     */
YARP_DEFINE(void) yarpPortFree(yarpPortPtr port) {
    if (port!=NULL) {
        if (port->implementation!=NULL) {
            delete (Port*)(port->implementation);
            port->implementation = NULL;
        }
        delete port;
    }
}

    /**
     *
     * Open a port, assigning it a name.
     *
     */
YARP_DEFINE(int) yarpPortOpen(yarpPortPtr port, const char *name) {
    YARP_OK(port);
    bool ok = YARP_PORT(port).open(name);
    return ok?0:-1;
}

    /**
     *
     * Open a port, using advanced contact information.
     *
     */
YARP_DEFINE(int) yarpPortOpenEx(yarpPortPtr port, yarpContactPtr contact) {
    YARP_OK(port);
    YARP_OK(contact);
    bool ok = YARP_PORT(port).open(YARP_CONTACT(contact));
    return ok?0:-1;
}

    /**
     *
     * Close a port.
     *
     */
YARP_DEFINE(int) yarpPortClose(yarpPortPtr port) {
    YARP_OK(port);
    YARP_PORT(port).close();
    return 0;
}

    /**
     *
     * Configure a port to write messages in the background, so calls to
     * write messages may return immediately.
     *
     */
YARP_DEFINE(int) yarpPortEnableBackgroundWrite(yarpPortPtr port,
                                               int writeInBackgroundFlag) {
    YARP_OK(port);
    YARP_PORT(port).enableBackgroundWrite(writeInBackgroundFlag);
    return 0;
}

    /**
     *
     * Write a message to a port.  The write handler of the msg structure
     * will be called.
     *
     */
YARP_DEFINE(int) yarpPortWrite(yarpPortPtr port,
                               yarpPortablePtr msg) {
    YARP_OK(port);
    Portable *amsg = MAKE_PORTABLE(msg);
    YARP_OK0(amsg);
    int result = YARP_PORT(port).write(*amsg)?0:-1;
    return result;
}


    /**
     *
     * Read a message from a port.  The read handler of the msg structure
     * will be called.  Set willReply to 1 if you intend to later
     * give a reply to the message.  You must then call yarpPortReply
     * before any other port methods.
     *
     */
YARP_DEFINE(int) yarpPortRead(yarpPortPtr port,
                              yarpPortablePtr msg,
                              int willReply) {
    YARP_OK(port);
    Portable *amsg = MAKE_PORTABLE(msg);
    YARP_OK0(amsg);
    return YARP_PORT(port).read(*amsg,willReply)?0:-1;
}


    /**
     *
     * Give a reply to a previously read message.
     *
     */
YARP_DEFINE(int) yarpPortReply(yarpPortPtr port,
                               yarpPortablePtr msg) {
    YARP_OK(port);
    Portable *amsg = MAKE_PORTABLE(msg);
    YARP_OK0(amsg);
    return YARP_PORT(port).reply(*amsg)?0:-1;
}


    /**
     *
     * Write a message to a port, then wait for a reply.
     *
     */
YARP_DEFINE(int) yarpPortWriteWithReply(yarpPortPtr port,
                                        yarpPortablePtr msg,
                                        yarpPortablePtr reply) {
    YARP_OK(port);
    Portable *amsg = MAKE_PORTABLE(msg);
    YARP_OK0(amsg);
    Portable *areply = MAKE_PORTABLE(reply);
    YARP_OK0(areply);
    return YARP_PORT(port).write(*amsg,*areply)?0:-1;
}
