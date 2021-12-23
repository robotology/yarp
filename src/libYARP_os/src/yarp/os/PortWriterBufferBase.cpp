/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/PortWriterBuffer.h>

#include <yarp/os/Port.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/PortCorePackets.h>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(PORTWRITERBUFFERBASE, "yarp.os.PortWriterBufferBase")
} // namespace

PortWriterBufferManager::~PortWriterBufferManager() = default;


class PortWriterBufferBase::Private : public PortWriterBufferManager
{
public:
    Private(PortWriterBufferBase& owner) :
            owner(owner)
    {
    }

    ~Private() override
    {
        release();
        finishWrites();
    }

    int getCount()
    {
        stateMutex.lock();
        int ct = packets.getCount();
        stateMutex.unlock();
        return ct;
    }

    void finishWrites()
    {
        yCDebug(PORTWRITERBUFFERBASE, "finishing writes");
        bool done = false;
        while (!done) {
            stateMutex.lock();
            if (port != nullptr) {
                if (!port->isOpen()) {
                    outCt = 0;
                }
            }
            done = (outCt == 0);
            if (!done) {
                finishing = true;
            }
            stateMutex.unlock();
            if (!done) {
                completionSema.wait();
            }
        }
        yCDebug(PORTWRITERBUFFERBASE, "finished writes");
    }

    const void* get()
    {
        if (callback != nullptr) {
            // (Safe to check outside mutex)
            // oops, there is already a prepared and unwritten
            // object.  best remove it.
            yCDebug(PORTWRITERBUFFERBASE, "releasing unused buffer");
            release();
        }
        stateMutex.lock();
        PortCorePacket* packet = packets.getFreePacket();
        yCAssert(PORTWRITERBUFFERBASE, packet != nullptr);
        if (packet->getContent() == nullptr) {
            yCDebug(PORTWRITERBUFFERBASE, "creating a writer buffer");
            //packet->setContent(owner.create(*this, packet), true);
            yarp::os::PortWriterWrapper* wrapper = owner.create(*this, packet);
            //packet->setContent(wrapper, true);
            packet->setContent(wrapper->getInternal(), false, wrapper, true);
        }
        stateMutex.unlock();

        current = packet->getContent();
        callback = packet->getCallback();
        return callback;
    }

    bool release()
    {
        stateMutex.lock();
        const PortWriter* cback = callback;
        current = nullptr;
        callback = nullptr;
        stateMutex.unlock();
        if (cback != nullptr) {
            stateMutex.lock();
            outCt++;
            stateMutex.unlock();
            cback->onCompletion();
        }
        return cback != nullptr;
    }

    void onCompletion(void* tracker) override
    {
        stateMutex.lock();
        yCDebug(PORTWRITERBUFFERBASE, "freeing up a writer buffer");
        packets.freePacket((PortCorePacket*)tracker, false);
        outCt--;
        bool sig = finishing;
        finishing = false;
        stateMutex.unlock();
        if (sig) {
            completionSema.post();
        }
    }


    void attach(Port& port)
    {
        stateMutex.lock();
        this->port = &port;
        port.enableBackgroundWrite(true);
        stateMutex.unlock();
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
        stateMutex.lock();
        const PortWriter* active = current;
        const PortWriter* cback = callback;
        current = nullptr;
        callback = nullptr;
        stateMutex.unlock();
        if (active != nullptr && port != nullptr) {
            stateMutex.lock();
            outCt++;
            stateMutex.unlock();
            port->write(*active, cback);
        }
    }

private:
    PortWriterBufferBase& owner;
    PortCorePackets packets;
    std::mutex stateMutex;
    yarp::os::Semaphore completionSema {0};
    Port* port {nullptr};
    const PortWriter* current {nullptr};
    const PortWriter* callback {nullptr};
    bool finishing {false};
    int outCt {0};
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
