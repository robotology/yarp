// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009, 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPC_YARPIMPL_INC
#define YARPC_YARPIMPL_INC

#include "yarp.h"

#include <yarp/os/all.h>
using namespace yarp::os;

#define YARP_DEFINE(rt) rt

#define YARP_OK0(s) if(s==NULL) return -1;
#define YARP_OK(s) if(s->implementation==NULL) return -1;
#define YARP_PORT(s) (*((Port*)(s->implementation)))
#define YARP_CONTACT(s) (*((Contact*)(s->implementation)))
#define YARP_NETWORK(s) (*((Network*)(s->implementation)))
#define YARP_READER(s) (*((ConnectionReader*)(s->implementation)))
#define YARP_WRITER(s) (*((ConnectionWriter*)(s->implementation)))
#define YARP_STRING(s) (*((ConstString*)(s->implementation)))

class YarpImplPortableAdaptor : public yarp::os::Portable {
private:
    yarpPortablePtr ref;
public:
    YarpImplPortableAdaptor(yarpPortablePtr ref) : ref(ref) {}

    virtual bool read(yarp::os::ConnectionReader& connection) {
        if (ref->callbacks->read==NULL) return false;
        yarpReader reader;
        reader.implementation = &connection;
        return (ref->callbacks->read(&reader,ref->client)==0);
    }

    virtual bool write(yarp::os::ConnectionWriter& connection) {
        if (ref->callbacks->write==NULL) return false;
        yarpWriter writer;
        writer.implementation = &connection;
        return (ref->callbacks->write(&writer,ref->client)==0);
    }

    virtual void onCommencement() {
        if (ref->callbacks->onCommencement==NULL) return;
        ref->callbacks->onCommencement(ref->client);
    }

    virtual void onCompletion() {
        if (ref->callbacks->onCompletion==NULL) return;
        ref->callbacks->onCompletion(ref->client);
    }

    static Portable *checkPortable(yarpPortablePtr ref) {
        if (ref!=NULL) {
            if (ref->adaptor==NULL) {
                ref->adaptor = new YarpImplPortableAdaptor(ref);
            }
        }
        return (Portable *)ref->adaptor;
    }
};

#define MAKE_PORTABLE(s) (YarpImplPortableAdaptor::checkPortable(s))

#endif
