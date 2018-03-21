/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPC_YARPIMPL_INC
#define YARPC_YARPIMPL_INC

#include "yarp.h"

#include <yarp/os/all.h>
using namespace yarp::os;

#define YARP_DEFINE(rt) rt

#define YARP_OK0(s) if(s==NULL) return -1;
#define YARP_OK(s) if(s->implementation==NULL) return -1;
#define YARP_COK(s) if(s->client==NULL) return -1;
//#define YARP_POK(s) if(s->portable.implementation==NULL) return -1;
#define YARP_PORT(s) (*((Port*)(s->implementation)))
#define YARP_CONTACT(s) (*((Contact*)(s->implementation)))
#define YARP_NETWORK(s) (*((Network*)((s==NULL)?(yarpNetworkGet()):s->implementation)))
#define YARP_READER(s) (*((ConnectionReader*)(s->implementation)))
#define YARP_WRITER(s) (*((ConnectionWriter*)(s->implementation)))
#define YARP_STRING(s) (*((ConstString*)(s->implementation)))
#define YARP_THREAD(s) (*((Thread*)(s->implementation)))
#define YARP_BOTTLE(s) (*((Bottle*)(s->implementation)))

class YarpImplPortableAdaptor : public yarp::os::Portable {
private:
    yarpPortablePtr ref;
    yarpPortableCallbacksPtr callbacks;
public:
    YarpImplPortableAdaptor(yarpPortablePtr ref,
                            yarpPortableCallbacksPtr c = NULL) : ref(ref) {
        if (c==NULL) {
            callbacks = yarpPortableCallbacksGet();
        } else {
            yarpPortableCallbacksComplete(c);
            callbacks = c;
        }
    }

    virtual bool read(yarp::os::ConnectionReader& connection) {
        yarpReader reader;
        reader.implementation = &connection;
        return (callbacks->read(&reader,ref->client)==0);
    }

    virtual bool write(yarp::os::ConnectionWriter& connection) {
        yarpWriter writer;
        writer.implementation = &connection;
        return (callbacks->write(&writer,ref->client)==0);
    }

    virtual void onCommencement() {
        callbacks->onCommencement(ref->client);
    }

    virtual void onCompletion() {
        callbacks->onCompletion(ref->client);
    }

    static Portable *checkPortable(yarpPortablePtr ref) {
        if (ref!=NULL) {
            if (ref->adaptor==NULL) {
                ref->adaptor = new YarpImplPortableAdaptor(ref);
            }
            return (Portable *)ref->adaptor;
        }
        return NULL;
    }
};

#define MAKE_PORTABLE(s) (YarpImplPortableAdaptor::checkPortable(s))

#endif
