// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_LOCALCARRIER_
#define _YARP2_LOCALCARRIER_

#include <yarp/AbstractCarrier.h>
#include <yarp/SemaphoreImpl.h>

namespace yarp {
    class LocalCarrier;
}

class yarp::LocalCarrier : public AbstractCarrier {
public:
    LocalCarrier *peer;

    LocalCarrier() {
        peer = NULL;
    }

    virtual Carrier *create() {
        return new LocalCarrier();
    }

    virtual String getName() {
        return "local";
    }

    virtual bool requireAck() {
        return true;
    }

    virtual bool isConnectionless() {
        return false;
    }

    virtual String getSpecifierName() {
        return "LOCALITY";
    }

    virtual bool checkHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = getSpecifierName();
            for (int i=0; i<8; i++) {
                if (!(target[i]==header.get()[i])) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    virtual void getHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = getSpecifierName();
            for (int i=0; i<8; i++) {
                header.get()[i] = target[i];
            }
        }   
    }

    virtual void setParameters(const Bytes& header) {
    }

    virtual void becomeLocal(Protocol& proto, bool sender) {
        YARP_ERROR(Logger::get(),"local carrier is not yet implemented");
        ACE_OS::exit(1);
        proto.takeStreams(NULL);
    }

    virtual void write(Protocol& proto, SizedWriter& writer) {
        // will need to modify that so that we can work
        // through a shared object reference instead (the target
        // is in the same process space as we are)

        // ideally, we'd just pass a reference to the writer object
        // and have it queried on the other side -- need to work a
        // bit on how SizedWriter operates so that this becomes efficiently
        // doable

        // default behavior upon a write request
        ACE_UNUSED_ARG(writer);
        proto.sendIndex();
        proto.sendContent();
        proto.expectAck();
    }

    virtual void respondToHeader(Protocol& proto) {
        // i am the receiver

        becomeLocal(proto,false);
    }


    virtual void expectReplyToHeader(Protocol& proto) {
        // i am the sender

        becomeLocal(proto,true);
    }

};

#endif

