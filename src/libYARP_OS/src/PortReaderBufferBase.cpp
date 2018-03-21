/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Portable.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Os.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SemaphoreImpl.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <yarp/os/impl/PortCorePacket.h>

#include <list>

using namespace yarp::os::impl;
using namespace yarp::os;



class PortReaderPacket
{
public:
    PortReaderPacket *prev_, *next_;

    // if non-null, contains a buffer that the packet owns
    PortReader *reader;

    ConstString envelope;

    // if nun-null, refers to an external buffer
    // by convention, overrides reader
    PortReader *external;
    PortWriter *writer; // if a callback is needed

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
        if (reader!=nullptr) {
            delete reader;
            reader = nullptr;
        }
        writer = nullptr;
        envelope = "";
    }

    PortReader *getReader()
    {
        return reader;
    }

    void setReader(PortReader *reader)
    {
        resetExternal();
        reset();
        this->reader = reader;
    }

    PortReader *getExternal()
    {
        return external;
    }

    void setExternal(PortReader *reader, PortWriter *writer)
    {
        resetExternal();
        this->external = reader;
        this->writer = writer;
    }

    void setEnvelope(const Bytes& bytes)
    {
        envelope = ConstString(bytes.get(), bytes.length());
        //envelope.set(bytes.get(), bytes.length(), 1);
    }

    void resetExternal()
    {
        if (writer!=nullptr) {
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

    PortReaderPacket *getInactivePacket()
    {
        if (inactive.empty()) {
            PortReaderPacket *obj = nullptr;
            obj = new PortReaderPacket();
            inactive.push_back(obj);
        }
        PortReaderPacket *next = inactive.front();
        yAssert(next!=nullptr);
        inactive.remove(next);
        return next;
    }

    PortReaderPacket *getActivePacket()
    {
        PortReaderPacket *next = nullptr;
        if (getCount()>=1) {
            next = active.front();
            yAssert(next!=nullptr);
            active.remove(next);
        }
        return next;
    }

    void addActivePacket(PortReaderPacket *packet)
    {
        if (packet!=nullptr) {
            active.push_back(packet);
        }
    }

    void addInactivePacket(PortReaderPacket *packet)
    {
        if (packet!=nullptr) {
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
    Port *port;
    SemaphoreImpl contentSema;
    SemaphoreImpl consumeSema;
    SemaphoreImpl stateSema;

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
            stateSema(1)
    {
    }

    virtual ~Private()
    {
        Port *closePort = nullptr;
        stateSema.wait();
        if (port!=nullptr) {
            closePort = port;
        }
        stateSema.post();
        if (closePort!=nullptr) {
            closePort->close();
        }
        stateSema.wait();
        clear();
        //stateSema.post();  // never give back mutex
    }

    void clear()
    {
        if (prev!=nullptr) {
            pool.addInactivePacket(prev);
            prev = nullptr;
        }
        pool.reset();
        ct = 0;
    }


    ConstString getName()
    {
        if (port!=nullptr) {
            return port->getName();
        }
        return "";
    }

    PortReaderPacket *get()
    {
        PortReaderPacket *result = nullptr;
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
        return pool.getCount();
    }

    PortReaderPacket *getContent()
    {
        if (prev!=nullptr) {
            pool.addInactivePacket(prev);
            prev = nullptr;
        }
        if (pool.getCount()>=1) {
            prev = pool.getActivePacket();
            ct--;
        }
        return prev;
    }


    bool getEnvelope(PortReader& envelope)
    {
        if (prev==nullptr) {
            return false;
        }
        StringInputStream sis;
        sis.add(prev->envelope.c_str());
        sis.add("\r\n");
        StreamConnectionReader sbr;
        Route route;
        sbr.reset(sis, nullptr, route, 0, true);
        return envelope.read(sbr);
    }

    PortReaderPacket *dropContent()
    {
        // don't affect "prev"
        PortReaderPacket *drop = nullptr;

        if (pool.getCount()>=1) {
            drop = pool.getActivePacket();
            if (drop!=nullptr) {
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

    void *acquire()
    {
        if (prev!=nullptr) {
            void *result = prev;
            prev = nullptr;
            return result;
        }
        return nullptr;
    }

    void release(void *key)
    {
        if (key!=nullptr) {
            pool.addInactivePacket((PortReaderPacket*)key);
        }
    }
};



PortReaderBufferBaseCreator::~PortReaderBufferBaseCreator()
{
}

PortReaderBufferBase::PortReaderBufferBase(unsigned int maxBuffer) :
        mPriv(new Private(*this, maxBuffer))
{
}

PortReaderBufferBase::~PortReaderBufferBase()
{
    delete mPriv;
}

yarp::os::PortReader *PortReaderBufferBase::create()
{
    if (mPriv->creator!=nullptr) {
        return mPriv->creator->create();
    }
    return nullptr;
}

int PortReaderBufferBase::check()
{
    mPriv->stateSema.wait();
    int count = mPriv->checkContent();
    mPriv->stateSema.post();
    return count;
}


void PortReaderBufferBase::interrupt()
{
    // give read a chance
    mPriv->contentSema.post();
}

PortReader *PortReaderBufferBase::readBase(bool& missed, bool cleanup)
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
        double diff = target-now;
        if (diff>0) {
            ok = mPriv->contentSema.waitWithTimeout(diff);
        } else {
            ok = mPriv->contentSema.check();
            if (ok) mPriv->contentSema.wait();
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
            if (diff>0) {
                SystemClock::delaySystem(diff);
            }
            mPriv->last_recv = target;
        }
    }
    mPriv->stateSema.wait();
    PortReaderPacket *readerPacket = mPriv->getContent();
    PortReader *reader = nullptr;
    if (readerPacket!=nullptr) {
        PortReader *external = readerPacket->getExternal();
        if (external==nullptr) {
            reader = readerPacket->getReader();
        } else {
            reader = external;
        }
    }
    mPriv->stateSema.post();
    if (reader!=nullptr) {
        mPriv->consumeSema.post();
    }
    return reader;
}


bool PortReaderBufferBase::read(ConnectionReader& connection)
{
    if (connection.getReference()!=nullptr) {
        //printf("REF %ld %d\n", (long int)connection.getReference(),
        //     connection.isValid());
        return acceptObjectBase(connection.getReference(), nullptr);
    }

    if (mPriv->replier != nullptr) {
        if (connection.getWriter()) {
            return mPriv->replier->read(connection);
        }
    }
    PortReaderPacket *reader = nullptr;
    while (reader==nullptr) {
        mPriv->stateSema.wait();
        reader = mPriv->get();
        if (reader && reader->getReader() == nullptr) {
            PortReader *next = create();
            yAssert(next != nullptr);
            reader->setReader(next);
        }

        mPriv->stateSema.post();
        if (reader==nullptr) {
            mPriv->consumeSema.wait();
        }
    }
    bool ok = false;
    if (connection.isValid()) {
        yAssert(reader->getReader()!=nullptr);
        ok = reader->getReader()->read(connection);
        reader->setEnvelope(connection.readEnvelope());
    } else {
        // this is a disconnection
        // don't talk to this port ever again
        mPriv->port = nullptr;
    }
    if (ok) {
        mPriv->stateSema.wait();
        bool pruned = false;
        if (mPriv->ct>0 && mPriv->prune) {
            PortReaderPacket *readerPacket =
                mPriv->dropContent();
            pruned = (readerPacket!=nullptr);
        }
        //mPriv->configure(reader, false, true);
        mPriv->pool.addActivePacket(reader);
        mPriv->ct++;
        mPriv->stateSema.post();
        if (!pruned) {
            mPriv->contentSema.post();
        }
        //YARP_ERROR(Logger::get(), ">>>>>>>>>>>>>>>>> adding data");
    } else {
        mPriv->stateSema.wait();
        mPriv->pool.addInactivePacket(reader);
        mPriv->stateSema.post();
        //YARP_ERROR(Logger::get(), ">>>>>>>>>>>>>>>>> skipping data");

        // important to give reader a shot anyway, allowing proper closing
        YARP_DEBUG(Logger::get(), "giving PortReaderBuffer chance to close");
        mPriv->contentSema.post();
    }
    return ok;
}



void PortReaderBufferBase::setCreator(PortReaderBufferBaseCreator *creator)
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

ConstString PortReaderBufferBase::getName() const
{
    return mPriv->getName();
}

unsigned int PortReaderBufferBase::getMaxBuffer()
{
    return mPriv->maxBuffer;
}

bool PortReaderBufferBase::isClosed()
{
    return mPriv->port==nullptr;
}

void PortReaderBufferBase::attachBase(Port& port)
{
    mPriv->attach(port);
}


/////////////////////
///
/// Careful!  merge with ::read -- very similar code
/// Until merge, don't change one without looking at other :-(

bool PortReaderBufferBase::acceptObjectBase(PortReader *obj,
                                            yarp::os::PortWriter *wrapper)
{
    // getting an object here should be basically the same as
    // receiving from a Port -- except no need to create/read
    // the object

    PortReaderPacket *reader = nullptr;
    while (reader==nullptr) {
        mPriv->stateSema.wait();
        reader = mPriv->get();
        mPriv->stateSema.post();
        if (reader==nullptr) {
            mPriv->consumeSema.wait();
        }
    }
    bool ok = true;
    if (ok) {
        reader->setExternal(obj, wrapper);

        mPriv->stateSema.wait();
        bool pruned = false;
        if (mPriv->ct>0 && mPriv->prune) {
            PortReaderPacket *readerPacket =
                mPriv->dropContent();
            pruned = (readerPacket!=nullptr);
        }
        //mPriv->configure(reader, false, true);
        mPriv->pool.addActivePacket(reader);
        mPriv->ct++;
        mPriv->stateSema.post();
        if (!pruned) {
            mPriv->contentSema.post();
        }
        //YARP_ERROR(Logger::get(), ">>>>>>>>>>>>>>>>> adding data");
    } else {
        mPriv->stateSema.wait();
        mPriv->pool.addInactivePacket(reader);
        mPriv->stateSema.post();
        //YARP_ERROR(Logger::get(), ">>>>>>>>>>>>>>>>> skipping data");

        // important to give reader a shot anyway, allowing proper closing
        YARP_DEBUG(Logger::get(), "giving PortReaderBuffer chance to close");
        mPriv->contentSema.post();
    }

    return true;
}



bool PortReaderBufferBase::forgetObjectBase(PortReader *obj,
                                            yarp::os::PortWriter *wrapper)
{
    YARP_UNUSED(obj);
    YARP_UNUSED(wrapper);
    printf("Sorry, forgetting not implemented yet\n");
    return false;
}



void *PortReaderBufferBase::acquire()
{
    return mPriv->acquire();
}

void PortReaderBufferBase::release(void *key)
{
    mPriv->stateSema.wait();
    mPriv->release(key);
    mPriv->stateSema.post();
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
    YARP_ERROR(Logger::get(), "Missing or incorrectly typed onRead function");
}


#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.72
void PortReaderBufferBase::release(PortReader *completed)
{
    YARP_UNUSED(completed);
    //mPriv->stateSema.wait();
    //mPriv->configure(completed, true, false);
    //mPriv->stateSema.post();
    printf("release not implemented anymore; not needed\n");
    std::exit(1);
}

void PortReaderBufferBase::setAllowReuse(bool flag)
{
    YARP_UNUSED(flag);
}
#endif // YARP_NO_DEPRECATED
