/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/PortReaderBufferBase.h>

#include <yarp/os/Os.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/PortCorePacket.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <list>
#include <mutex>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(PORTREADERBUFFERBASE, "yarp.os.PortReaderBufferBase")
} // namespace

#ifndef DOXYGEN_SHOULD_SKIP_THIS
class PortReaderPacket
{
public:
    PortReaderPacket *prev_, *next_;

    // if non-null, contains a buffer that the packet owns
    PortReader* reader;

    std::string envelope;

    // if nun-null, refers to an external buffer
    // by convention, overrides reader
    PortReader* external;
    PortWriter* writer; // if a callback is needed

    PortReaderPacket()
    {
        prev_ = next_ = nullptr;
        reader = nullptr;
        external = nullptr;
        writer = nullptr;
        reset();
    }

    virtual ~PortReaderPacket()
    {
        resetExternal();
        reset();
    }

    void reset()
    {
        if (reader != nullptr) {
            delete reader;
            reader = nullptr;
        }
        writer = nullptr;
        envelope = "";
    }

    PortReader* getReader()
    {
        return reader;
    }

    void setReader(PortReader* reader)
    {
        resetExternal();
        reset();
        this->reader = reader;
    }

    PortReader* getExternal()
    {
        return external;
    }

    void setExternal(PortReader* reader, PortWriter* writer)
    {
        resetExternal();
        this->external = reader;
        this->writer = writer;
    }

    void setEnvelope(const Bytes& bytes)
    {
        envelope = std::string(bytes.get(), bytes.length());
        //envelope.set(bytes.get(), bytes.length(), 1);
    }

    void resetExternal()
    {
        if (writer != nullptr) {
            writer->onCompletion();
            writer = nullptr;
        }
        external = nullptr;
    }
};


class PortReaderPool
{
private:
    std::list<PortReaderPacket*> inactive;
    std::list<PortReaderPacket*> active;

public:
    size_t getCount()
    {
        return active.size();
    }

    size_t getFree()
    {
        return inactive.size();
    }

    PortReaderPacket* getInactivePacket()
    {
        if (inactive.empty()) {
            PortReaderPacket* obj = nullptr;
            obj = new PortReaderPacket();
            inactive.push_back(obj);
        }
        PortReaderPacket* next = inactive.front();
        yCAssert(PORTREADERBUFFERBASE, next != nullptr);
        inactive.remove(next);
        return next;
    }

    PortReaderPacket* getActivePacket()
    {
        PortReaderPacket* next = nullptr;
        if (getCount() >= 1) {
            next = active.front();
            yCAssert(PORTREADERBUFFERBASE, next != nullptr);
            active.remove(next);
        }
        return next;
    }

    void addActivePacket(PortReaderPacket* packet)
    {
        if (packet != nullptr) {
            active.push_back(packet);
        }
    }

    void addInactivePacket(PortReaderPacket* packet)
    {
        if (packet != nullptr) {
            inactive.push_back(packet);
        }
    }

    void reset()
    {
        while (!active.empty()) {
            delete active.back();
            active.pop_back();
        }
        while (!inactive.empty()) {
            delete inactive.back();
            inactive.pop_back();
        }
    }
};


class PortReaderBufferBase::Private
{
private:
    PortReaderBufferBase& owner;
    PortReaderPacket* prev;

public:
    PortReaderBufferBaseCreator* creator;
    unsigned int maxBuffer;
    bool prune;
    yarp::os::PortReader* replier;
    double period;
    double last_recv;

    PortReaderPool pool;

    int ct;
    Port* port;
    yarp::os::Semaphore contentSema;
    yarp::os::Semaphore consumeSema;
    std::mutex stateMutex;

    Private(PortReaderBufferBase& owner, unsigned int maxBuffer) :
            owner(owner),
            prev(nullptr),
            creator(nullptr),
            maxBuffer(maxBuffer),
            prune(false),
            replier(nullptr),
            period(-1),
            last_recv(-1),
            ct(0),
            port(nullptr),
            contentSema(0),
            consumeSema(0),
            stateMutex()
    {
    }

    virtual ~Private()
    {
        Port* closePort = nullptr;
        stateMutex.lock();
        if (port != nullptr) {
            closePort = port;
        }
        stateMutex.unlock();
        if (closePort != nullptr) {
            closePort->close();
        }
        stateMutex.lock();
        clear();
        stateMutex.unlock();
    }

    void clear()
    {
        if (prev != nullptr) {
            pool.addInactivePacket(prev);
            prev = nullptr;
        }
        pool.reset();
        ct = 0;
    }


    std::string getName()
    {
        if (port != nullptr) {
            return port->getName();
        }
        return {};
    }

    PortReaderPacket* get()
    {
        PortReaderPacket* result = nullptr;
        bool grab = true;
        if (pool.getFree() == 0) {
            grab = false;
            if (maxBuffer == 0 || pool.getCount() < maxBuffer) {
                grab = true;
            } else {
                // ok, can't get free, clean space.
                // here would be a good place to do buffer reuse.
            }
        }
        if (grab) {
            result = pool.getInactivePacket();
        }

        return result;
    }

    int checkContent()
    {
        return (int)pool.getCount();
    }

    PortReaderPacket* getContent()
    {
        if (prev != nullptr) {
            pool.addInactivePacket(prev);
            prev = nullptr;
        }
        if (pool.getCount() >= 1) {
            prev = pool.getActivePacket();
            ct--;
        }
        return prev;
    }


    bool getEnvelope(PortReader& envelope)
    {
        if (prev == nullptr) {
            return false;
        }
        StringInputStream sis;
        sis.add(prev->envelope);
        sis.add("\r\n");
        StreamConnectionReader sbr;
        Route route;
        sbr.reset(sis, nullptr, route, 0, true);
        return envelope.read(sbr);
    }

    PortReaderPacket* dropContent()
    {
        // don't affect "prev"
        PortReaderPacket* drop = nullptr;

        if (pool.getCount() >= 1) {
            drop = pool.getActivePacket();
            if (drop != nullptr) {
                pool.addInactivePacket(drop);
            }
            ct--;
        }
        return drop;
    }

    void attach(Port& port)
    {
        this->port = &port;
        port.setReader(owner);
    }

    void* acquire()
    {
        if (prev != nullptr) {
            void* result = prev;
            prev = nullptr;
            return result;
        }
        return nullptr;
    }

    void release(void* key)
    {
        if (key != nullptr) {
            pool.addInactivePacket((PortReaderPacket*)key);
        }
    }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS



PortReaderBufferBase::PortReaderBufferBase(unsigned int maxBuffer) :
        mPriv(new Private(*this, maxBuffer))
{
}

PortReaderBufferBase::~PortReaderBufferBase()
{
    delete mPriv;
}

yarp::os::PortReader* PortReaderBufferBase::create()
{
    if (mPriv->creator != nullptr) {
        return mPriv->creator->create();
    }
    return nullptr;
}

int PortReaderBufferBase::check()
{
    mPriv->stateMutex.lock();
    int count = mPriv->checkContent();
    mPriv->stateMutex.unlock();
    return count;
}


void PortReaderBufferBase::interrupt()
{
    // give read a chance
    mPriv->contentSema.post();
}

PortReader* PortReaderBufferBase::readBase(bool& missed, bool cleanup)
{
    missed = false;
    if (mPriv->period < 0 || cleanup) {
        mPriv->contentSema.wait();
    } else {
        bool ok = false;
        double now = SystemClock::nowSystem();
        double target = now + mPriv->period;
        if (mPriv->last_recv > 0) {
            target = mPriv->last_recv + mPriv->period;
        }
        double diff = target - now;
        if (diff > 0) {
            ok = mPriv->contentSema.waitWithTimeout(diff);
        } else {
            ok = mPriv->contentSema.check();
            if (ok) {
                mPriv->contentSema.wait();
            }
        }
        if (!ok) {
            missed = true;
            if (mPriv->last_recv > 0) {
                mPriv->last_recv += mPriv->period;
            }
            return nullptr;
        }
        now = SystemClock::nowSystem();
        if (mPriv->last_recv < 0) {
            mPriv->last_recv = now;
        } else {
            diff = target - now;
            if (diff > 0) {
                SystemClock::delaySystem(diff);
            }
            mPriv->last_recv = target;
        }
    }
    mPriv->stateMutex.lock();
    PortReaderPacket* readerPacket = mPriv->getContent();
    PortReader* reader = nullptr;
    if (readerPacket != nullptr) {
        PortReader* external = readerPacket->getExternal();
        if (external == nullptr) {
            reader = readerPacket->getReader();
        } else {
            reader = external;
        }
    }
    mPriv->stateMutex.unlock();
    if (reader != nullptr) {
        mPriv->consumeSema.post();
    }
    return reader;
}


bool PortReaderBufferBase::read(ConnectionReader& connection)
{
    if (connection.getReference() != nullptr) {
        //printf("REF %ld %d\n", (long int)connection.getReference(),
        //     connection.isValid());
        return acceptObjectBase(connection.getReference(), nullptr);
    }

    if (mPriv->replier != nullptr) {
        if (connection.getWriter() != nullptr) {
            return mPriv->replier->read(connection);
        }
    }
    PortReaderPacket* reader = nullptr;
    while (reader == nullptr) {
        mPriv->stateMutex.lock();
        reader = mPriv->get();
        if ((reader != nullptr) && reader->getReader() == nullptr) {
            PortReader* next = create();
            yCAssert(PORTREADERBUFFERBASE, next != nullptr);
            reader->setReader(next);
        }

        mPriv->stateMutex.unlock();
        if (reader == nullptr) {
            mPriv->consumeSema.wait();
        }
    }
    bool ok = false;
    if (connection.isValid()) {
        yCAssert(PORTREADERBUFFERBASE, reader->getReader() != nullptr);
        ok = reader->getReader()->read(connection);
        reader->setEnvelope(connection.readEnvelope());
    } else {
        // this is a disconnection
        // don't talk to this port ever again
        mPriv->port = nullptr;
    }
    if (ok) {
        mPriv->stateMutex.lock();
        bool pruned = false;
        if (mPriv->ct > 0 && mPriv->prune) {
            PortReaderPacket* readerPacket = mPriv->dropContent();
            pruned = (readerPacket != nullptr);
        }
        //mPriv->configure(reader, false, true);
        mPriv->pool.addActivePacket(reader);
        mPriv->ct++;
        mPriv->stateMutex.unlock();
        if (!pruned) {
            mPriv->contentSema.post();
        }
        yCTrace(PORTREADERBUFFERBASE, ">>>>>>>>>>>>>>>>> adding data");
    } else {
        mPriv->stateMutex.lock();
        mPriv->pool.addInactivePacket(reader);
        mPriv->stateMutex.unlock();
        yCTrace(PORTREADERBUFFERBASE, ">>>>>>>>>>>>>>>>> skipping data");

        // important to give reader a shot anyway, allowing proper closing
        yCDebug(PORTREADERBUFFERBASE, "giving PortReaderBuffer chance to close");
        mPriv->contentSema.post();
    }
    return ok;
}


void PortReaderBufferBase::setCreator(PortReaderBufferBaseCreator* creator)
{
    mPriv->creator = creator;
}

void PortReaderBufferBase::setReplier(yarp::os::PortReader& reader)
{
    mPriv->replier = &reader;
}

void PortReaderBufferBase::setPrune(bool flag)
{
    mPriv->prune = flag;
}

void PortReaderBufferBase::setTargetPeriod(double period)
{
    mPriv->period = period;
}

std::string PortReaderBufferBase::getName() const
{
    return mPriv->getName();
}

unsigned int PortReaderBufferBase::getMaxBuffer()
{
    return mPriv->maxBuffer;
}

bool PortReaderBufferBase::isClosed()
{
    return mPriv->port == nullptr;
}

void PortReaderBufferBase::attachBase(Port& port)
{
    mPriv->attach(port);
}


/////////////////////
///
/// Careful!  merge with ::read -- very similar code
/// Until merge, don't change one without looking at other :-(

bool PortReaderBufferBase::acceptObjectBase(PortReader* obj,
                                            yarp::os::PortWriter* wrapper)
{
    // getting an object here should be basically the same as
    // receiving from a Port -- except no need to create/read
    // the object

    PortReaderPacket* reader = nullptr;
    while (reader == nullptr) {
        mPriv->stateMutex.lock();
        reader = mPriv->get();
        mPriv->stateMutex.unlock();
        if (reader == nullptr) {
            mPriv->consumeSema.wait();
        }
    }

    reader->setExternal(obj, wrapper);

    mPriv->stateMutex.lock();
    bool pruned = false;
    if (mPriv->ct > 0 && mPriv->prune) {
        PortReaderPacket* readerPacket = mPriv->dropContent();
        pruned = (readerPacket != nullptr);
    }
    //mPriv->configure(reader, false, true);
    mPriv->pool.addActivePacket(reader);
    mPriv->ct++;
    mPriv->stateMutex.unlock();
    if (!pruned) {
        mPriv->contentSema.post();
    }
    yCTrace(PORTREADERBUFFERBASE, ">>>>>>>>>>>>>>>>> adding data");

    return true;
}


bool PortReaderBufferBase::forgetObjectBase(PortReader* obj,
                                            yarp::os::PortWriter* wrapper)
{
    YARP_UNUSED(obj);
    YARP_UNUSED(wrapper);
    printf("Sorry, forgetting not implemented yet\n");
    return false;
}


void* PortReaderBufferBase::acquire()
{
    return mPriv->acquire();
}

void PortReaderBufferBase::release(void* key)
{
    mPriv->stateMutex.lock();
    mPriv->release(key);
    mPriv->stateMutex.unlock();
}


bool PortReaderBufferBase::getEnvelope(PortReader& envelope)
{
    return mPriv->getEnvelope(envelope);
}

void PortReaderBufferBase::clear()
{
    mPriv->clear();
}

void typedReaderMissingCallback()
{
    yCError(PORTREADERBUFFERBASE, "Missing or incorrectly typed onRead function");
}
