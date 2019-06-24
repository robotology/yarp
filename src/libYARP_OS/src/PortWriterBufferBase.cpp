/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/PortWriterBuffer.h>

#include <yarp/os/Port.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PortCorePackets.h>

using namespace yarp::os::impl;
using namespace yarp::os;


PortWriterBufferManager::~PortWriterBufferManager() = default;


class PortWriterBufferBase::Private : public PortWriterBufferManager
{
public:
    Private(PortWriterBufferBase& owner) :
            owner(owner),
            stateSema(1),
            completionSema(0),
            port(nullptr),
            current(nullptr),
            callback(nullptr),
            finishing(false),
            outCt(0)
    {
    }

    ~Private() override
    {
        release();
        finishWrites();
        stateSema.wait();
    }

    int getCount()
    {
        stateSema.wait();
        int ct = packets.getCount();
        stateSema.post();
        return ct;
    }

    void finishWrites()
    {
        YARP_DEBUG(Logger::get(), "finishing writes");
        bool done = false;
        while (!done) {
            stateSema.wait();
            if (port != nullptr) {
                if (!port->isOpen()) {
                    outCt = 0;
                }
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

    const void* get()
    {
        if (callback != nullptr) {
            // (Safe to check outside mutex)
            // oops, there is already a prepared and unwritten
            // object.  best remove it.
            YARP_DEBUG(Logger::get(), "releasing unused buffer");
            release();
        }
        stateSema.wait();
        PortCorePacket* packet = packets.getFreePacket();
        yAssert(packet != nullptr);
        if (packet->getContent() == nullptr) {
            YARP_DEBUG(Logger::get(), "creating a writer buffer");
            //packet->setContent(owner.create(*this, packet), true);
            yarp::os::PortWriterWrapper* wrapper = owner.create(*this, packet);
            //packet->setContent(wrapper, true);
            packet->setContent(wrapper->getInternal(), false, wrapper, true);
        }
        stateSema.post();

        current = packet->getContent();
        callback = packet->getCallback();
        return callback;
    }

    bool release()
    {
        stateSema.wait();
        const PortWriter* cback = callback;
        current = nullptr;
        callback = nullptr;
        stateSema.post();
        if (cback != nullptr) {
            stateSema.wait();
            outCt++;
            stateSema.post();
            cback->onCompletion();
        }
        return cback != nullptr;
    }

    void onCompletion(void* tracker) override
    {
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


    void attach(Port& port)
    {
        stateSema.wait();
        this->port = &port;
        port.enableBackgroundWrite(true);
        stateSema.post();
    }

    void detach()
    {
        // nothing to do
    }

    void write(bool strict)
    {
        if (strict) {
            finishWrites();
        }
        stateSema.wait();
        const PortWriter* active = current;
        const PortWriter* cback = callback;
        current = nullptr;
        callback = nullptr;
        stateSema.post();
        if (active != nullptr && port != nullptr) {
            stateSema.wait();
            outCt++;
            stateSema.post();
            port->write(*active, cback);
        }
    }

private:
    PortWriterBufferBase& owner;
    PortCorePackets packets;
    yarp::os::Semaphore stateSema;
    yarp::os::Semaphore completionSema;
    Port* port;
    const PortWriter* current;
    const PortWriter* callback;
    bool finishing;
    int outCt;
};




PortWriterBufferBase::PortWriterBufferBase() :
        mPriv(new Private(*this))
{
}

PortWriterBufferBase::~PortWriterBufferBase()
{
    delete mPriv;
}

const void* PortWriterBufferBase::getContent() const
{
    return mPriv->get();
}

bool PortWriterBufferBase::releaseContent()
{
    return mPriv->release();
}


int PortWriterBufferBase::getCount()
{
    return mPriv->getCount();
}

void PortWriterBufferBase::attach(Port& port)
{
    mPriv->attach(port);
}

void PortWriterBufferBase::detach()
{
    mPriv->detach();
}

void PortWriterBufferBase::write(bool strict)
{
    mPriv->write(strict);
}

void PortWriterBufferBase::waitForWrite()
{
    mPriv->finishWrites();
}
