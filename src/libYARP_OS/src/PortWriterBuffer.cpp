/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/PortWriterBuffer.h>
#include <yarp/os/Port.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SemaphoreImpl.h>

#include <yarp/os/impl/PortCorePackets.h>

using namespace yarp::os::impl;
using namespace yarp::os;


class PortWriterBufferBaseHelper : public PortWriterBufferManager {
public:
    PortWriterBufferBaseHelper(PortWriterBufferBase& owner) :
        owner(owner), stateSema(1), completionSema(0) {
        current = nullptr;
        callback = nullptr;
        port = nullptr;
        finishing = false;
        outCt = 0;
    }

    virtual ~PortWriterBufferBaseHelper() {
        release();
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
            if (port) {
                if (!port->isOpen()) outCt = 0;
            }
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
        if (callback!=nullptr) {
            // (Safe to check outside mutex)
            // oops, there is already a prepared and unwritten
            // object.  best remove it.
            YARP_DEBUG(Logger::get(), "releasing unused buffer");
            release();
        }
        stateSema.wait();
        PortCorePacket *packet = packets.getFreePacket();
        yAssert(packet!=nullptr);
        if (packet->getContent()==nullptr) {
            YARP_DEBUG(Logger::get(), "creating a writer buffer");
            //packet->setContent(owner.create(*this, packet), true);
            yarp::os::PortWriterWrapper *wrapper =
                owner.create(*this, packet);
            //packet->setContent(wrapper, true);
            packet->setContent(wrapper->getInternal(), false,
                             wrapper, true);
        }
        stateSema.post();

        current = packet->getContent();
        callback = packet->getCallback();
        return callback;
    }

    bool release() {
        stateSema.wait();
        PortWriter *cback = callback;
        current = nullptr;
        callback = nullptr;
        stateSema.post();
        if (cback!=nullptr) {
            stateSema.wait();
            outCt++;
            stateSema.post();
            cback->onCompletion();
        }
        return cback!=nullptr;
    }

    virtual void onCompletion(void *tracker) override {
        stateSema.wait();
        YARP_DEBUG(Logger::get(), "freeing up a writer buffer");
        packets.freePacket((PortCorePacket*)tracker, false);
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

    void detach() {
        // nothing to do
    }

    void write(bool strict) {
        if (strict) {
            finishWrites();
        }
        stateSema.wait();
        PortWriter *active = current;
        PortWriter *cback = callback;
        current = nullptr;
        callback = nullptr;
        stateSema.post();
        if (active!=nullptr && port!=nullptr) {
            stateSema.wait();
            outCt++;
            stateSema.post();
            port->write(*active, cback);
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


PortWriterBufferManager::~PortWriterBufferManager() {
}


#define HELPER(x) (*((PortWriterBufferBaseHelper*)(x)))


PortWriterBufferBase::PortWriterBufferBase() {
    implementation = nullptr;
    init();
}

void PortWriterBufferBase::init() {
    yAssert(implementation==nullptr);
    implementation = new PortWriterBufferBaseHelper(*this);
    yAssert(implementation!=nullptr);
}


PortWriterBufferBase::~PortWriterBufferBase() {
    if (implementation!=nullptr) {
        delete &HELPER(implementation);
        implementation = nullptr;
    }
}


void *PortWriterBufferBase::getContent() {
    return HELPER(implementation).get();
}

bool PortWriterBufferBase::releaseContent() {
    return HELPER(implementation).release();
}


int PortWriterBufferBase::getCount() {
    return HELPER(implementation).getCount();
}

void PortWriterBufferBase::attach(Port& port) {
    HELPER(implementation).attach(port);
}

void PortWriterBufferBase::detach() {
    HELPER(implementation).detach();
}

void PortWriterBufferBase::write(bool strict) {
    HELPER(implementation).write(strict);
}

void PortWriterBufferBase::waitForWrite() {
    HELPER(implementation).finishWrites();
}

