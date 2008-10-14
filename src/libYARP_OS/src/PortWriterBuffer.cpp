// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/PortWriterBuffer.h>
#include <yarp/os/Port.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SemaphoreImpl.h>

#include <yarp/os/impl/PortCorePacket.h>

using namespace yarp::os::impl;
using namespace yarp::os;


class PortWriterBufferBaseHelper : public PortWriterBufferManager {
public:
    PortWriterBufferBaseHelper(PortWriterBufferBase& owner) : 
        owner(owner), stateSema(1), completionSema(0) {
        current = NULL;
        callback = NULL;
        port = NULL;
        finishing = false;
        outCt = 0;
    }

    virtual ~PortWriterBufferBaseHelper() {
        finishWrites();
        stateSema.wait();
    }

    int getCount() {
        stateSema.wait();
        int ct = packets.getCount();
        stateSema.post();
        return ct;
    }

    void finishWrites() {
        YARP_DEBUG(Logger::get(), "finishing writes");
        bool done = false;
        while (!done) {
            stateSema.wait();
            done = (outCt == 0);
            if (!done) {
                finishing = true;
            }
            stateSema.post();
            if (!done) {
                completionSema.wait();
            }
        }
        YARP_DEBUG(Logger::get(), "finished writes");
    }

    void *get() {
        stateSema.wait();
        PortCorePacket *packet = packets.getFreePacket();
        YARP_ASSERT(packet!=NULL);
        if (packet->getContent()==NULL) {
            YARP_DEBUG(Logger::get(), "creating a writer buffer");
            //packet->setContent(owner.create(*this,packet),true);
            yarp::os::PortWriterWrapper *wrapper = 
                owner.create(*this,packet);
            //packet->setContent(wrapper,true);
            packet->setContent(wrapper->getInternal(), false,
                             wrapper,true);
        }
        stateSema.post();

        current = packet->getContent();
        callback = packet->getCallback();
        return callback;
    }

    virtual void onCompletion(void *tracker) {
        stateSema.wait();
        YARP_DEBUG(Logger::get(), "freeing up a writer buffer");
        packets.freePacket((PortCorePacket*)tracker,false);
        outCt--;
        bool sig = finishing;
        finishing = false;
        stateSema.post();
        if (sig) {
            completionSema.post();
        }
    }


    void attach(Port& port) {
        stateSema.wait();
        this->port = &port;
        port.enableBackgroundWrite(true);
        stateSema.post();
    }

    void write(bool strict) {
        if (strict) {
            finishWrites();
        }
        stateSema.wait();
        PortWriter *active = current;
        PortWriter *cback = callback;
        stateSema.post();
        if (active!=NULL && port!=NULL) {
            outCt++;
            port->write(*active,cback);
        } 
    }

private:
    PortWriterBufferBase& owner;
    PortCorePackets packets;
    SemaphoreImpl stateSema;
    SemaphoreImpl completionSema;
    Port *port;
    PortWriter *current;
    PortWriter *callback;
    bool finishing;
    int outCt;
};

#define HELPER(x) (*((PortWriterBufferBaseHelper*)(x)))


PortWriterBufferBase::PortWriterBufferBase() {
    implementation = NULL;
    init();
}

void PortWriterBufferBase::init() {
    YARP_ASSERT(implementation==NULL);
    implementation = new PortWriterBufferBaseHelper(*this);
    YARP_ASSERT(implementation!=NULL);
}


PortWriterBufferBase::~PortWriterBufferBase() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}


void *PortWriterBufferBase::getContent() {
    return HELPER(implementation).get();
}


int PortWriterBufferBase::getCount() {
    return HELPER(implementation).getCount();
}

void PortWriterBufferBase::attach(Port& port) {
    HELPER(implementation).attach(port);
}

void PortWriterBufferBase::write(bool strict) {
    HELPER(implementation).write(strict);
}





