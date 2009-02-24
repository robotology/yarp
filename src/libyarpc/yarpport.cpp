// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008, 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include "yarp.h"
#include "yarpimpl.h"


class PortableAdaptor : public Portable {
private:
    yarpPortablePtr ref;
public:
    PortableAdaptor(yarpPortablePtr ref) : ref(ref) {}

    virtual bool read(ConnectionReader& connection) {
        if (ref->read==NULL) return false;
        yarpReader reader;
        reader.implementation = &connection;
        return (ref->read(&reader)==0);
    }

    virtual bool write(ConnectionWriter& connection) {
        if (ref->write==NULL) return false;
        yarpWriter writer;
        writer.implementation = &connection;
        return (ref->write(&writer)==0);
    }

    virtual void onCommencement() {
        if (ref->onCommencement==NULL) return;
        ref->onCommencement();
    }

    virtual void onCompletion() {
        if (ref->onCompletion==NULL) return;
        ref->onCompletion();
    }
};


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

YARP_DEFINE(void) yarpPortFree(yarpPortPtr port) {
    if (port!=NULL) {
        if (port->implementation!=NULL) {
            delete (Port*)(port->implementation);
            port->implementation = NULL;
        }
        delete port;
    }
}

YARP_DEFINE(int) yarpPortOpen(yarpPortPtr port, yarpContactPtr contact) {
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


YARP_DEFINE(int) yarpPortWrite(yarpPortPtr port, 
                               yarpPortablePtr msg) {
    YARP_OK(port);
    if (msg==NULL) return -1;
    if (msg->write==NULL) return -1;
    PortableAdaptor adapt(msg);
    return YARP_PORT(port).write(adapt)?0:-1;
}


YARP_DEFINE(int) yarpPortRead(yarpPortPtr port, 
                              yarpPortablePtr msg,
                              int willReply) {
    YARP_OK(port);
    if (msg==NULL) return -1;
    if (msg->read==NULL) return -1;
    PortableAdaptor adapt(msg);
    return YARP_PORT(port).read(adapt,willReply)?0:-1;
}


YARP_DEFINE(int) yarpPortReply(yarpPortPtr port, 
                               yarpPortablePtr msg) {
    YARP_OK(port);
    if (msg==NULL) return -1;
    if (msg->write==NULL) return -1;
    PortableAdaptor adapt(msg);
    return YARP_PORT(port).reply(adapt)?0:-1;
}


YARP_DEFINE(int) yarpPortWriteWithReply(yarpPortPtr port, 
                                        yarpPortablePtr msg,
                                        yarpPortablePtr reply) {
    YARP_OK(port);
    if (msg==NULL) return -1;
    if (reply==NULL) return -1;
    if (msg->write==NULL) return -1;
    if (reply->read==NULL) return -1;
    PortableAdaptor adapt1(msg), adapt2(reply);
    return YARP_PORT(port).write(adapt1,adapt2)?0:-1;
}


