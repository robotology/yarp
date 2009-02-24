// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
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



YARP_DEFINE(yarpPortablePtr) yarpPortableCreate() {
    yarpPortablePtr portable = new yarpPortable;
    if (portable!=NULL) {
        portable->implementation = new PortableAdaptor(portable);
        if (portable->implementation==NULL) {
            delete portable;
            portable = NULL;
        }
        portable->read = NULL;
        portable->write = NULL;
        portable->onCompletion = NULL;
        portable->onCommencement = NULL;
    }
    return portable;
}


YARP_DEFINE(void) yarpPortableFree(yarpPortablePtr portable) {
    if (portable!=NULL) {
        if (portable->implementation!=NULL) {
            delete (Portable*)(portable->implementation);
            portable->implementation = NULL;
        }
        delete portable;
    }
}

YARP_DEFINE(int) yarpPortableSetWriteHandler(yarpPortablePtr portable, int (*write) (yarpWriterPtr connection)) {
    YARP_OK(portable);    
    portable->write = write;
}

YARP_DEFINE(int) yarpPortableSetReadHandler(yarpPortablePtr portable, int (*read) (yarpReaderPtr connection)) {
    YARP_OK(portable);
    portable->read = read;
}


YARP_DEFINE(int) yarpPortableSetOnCompletionHandler(yarpPortablePtr portable, int(*onCompletion)()) {
    YARP_OK(portable);
    portable->onCompletion = onCompletion;
}

YARP_DEFINE(int) yarpPortableSetOnCommencementHandler(yarpPortablePtr portable, int(*onCommencement)()) {
    YARP_OK(portable);
    portable->onCommencement = onCommencement;
}


